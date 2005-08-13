/***************************************************************************
 *   Copyright (C) 2004 by Albert Cervera Areny                            *
 *   albertca@hotpop.com                                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include <qsqldatabase.h>

#include <kdebug.h>
#include <kprocess.h>
#include <qsqlcursor.h>

#include <sqldbbackend.h>
#include <manager.h>
#include <object.h>

#include "sqlbackendtest.h"
#include "customerorder.h"
#include "article.h"
#include "customer.h"

void SqlBackendTest::transactions()
{
	ObjectRef<CustomerOrder> order = CustomerOrder::create();
	order->setNumber( 50000 );
	order->setDate( QDate::currentDate() );
	order->setOrder( order );

	ObjectRef<Article> a1 = Article::create();
	a1->setCode( "1" );
	a1->setLabel( "Article One" );
	a1->setDescription( "Description of article number one" );
	order->articles()->add( a1 );

	ObjectRef<Article> a2 = Article::create();
	a2->setCode( "2" );
	a2->setLabel( "Article Two" );
	a2->setDescription( "Description of article number two" );
	a1->setArticle( a2 );

	ObjectRef<Customer> c = Customer::create();
	c->setCode( "0001" );
	c->setCustomerName( "Customer One" );
	c->setAddress( "Street" );
	c->setCity( "City" );
	c->setZipCode( "Zip Code" );
	c->setCountry( "Country" );
	order->setCustomer( c );

	CHECK( Manager::self()->commit(), true );
	
	// Check data has been saved correctly to the database
	QSqlCursor cursor( "customerorder" );
	cursor.select( "number = 50000" );
	CHECK( cursor.next(), true );
	CHECK( cursor.value( "date" ).toDate().toString() , QDate::currentDate().toString() );
	CHECK( variantToOid( cursor.value( "customerorder_customerorder" ) ), variantToOid( cursor.value( "dboid" ) ) );
	CHECK( variantToOid( cursor.value( "customer_customerorder" ) ), c->oid() );
	CHECK( cursor.next(), false ); 

	cursor.setName( "article" );
	cursor.select( "code = '1'" );
	CHECK( cursor.next(), true );
	CHECK( cursor.value( "label" ).toString(), QString("Article One") );
	CHECK( cursor.value( "description" ).toString(), QString("Description of article number one") );
	CHECK( cursor.next(), false );
	
	cursor.setName( "article_customerorder" );
	cursor.select( "article = " + oidToString( a1->oid() ) + " AND customerorder = " + oidToString( order->oid() ) );
	CHECK( cursor.next(), true );
	CHECK( cursor.next(), false );
	
	cursor.setName( "article" );
	cursor.select( "code = '2'" );
	CHECK( cursor.next(), true );
	CHECK( cursor.value( "label" ).toString(), QString("Article Two") );
	CHECK( cursor.value( "description" ).toString(), QString("Description of article number two") );
	CHECK( cursor.next(), false );
	
	cursor.setName( "customer" );
	cursor.select( "code = '0001'" );
	CHECK( cursor.next(), true );
	CHECK( cursor.value( "customername" ).toString(), QString("Customer One") );
	CHECK( cursor.value( "address" ).toString(), QString("Street") );
	CHECK( cursor.value( "city" ).toString(), QString("City") );
	CHECK( cursor.value( "zipcode" ).toString(), QString("Zip Code") );
	CHECK( cursor.value( "country" ).toString(), QString("Country") );
	CHECK( cursor.next(), false );
	
	
	a1->setDescription( "MODIFIED description of article number one" );
	// Check data has been saved correctly to the database
	CHECK( Manager::self()->commit(), true );
	cursor.setName( "article" );
	cursor.select( "code = '1'" );
	CHECK( cursor.next(), true );
	CHECK( cursor.value( "description" ).toString(), QString("MODIFIED description of article number one") );
	CHECK( cursor.next(), false );

	ObjectRef<Customer> c2 = Customer::create();
	c2->setCode( "0002" );
	c2->setCustomerName( "Customer Two" );
	c2->setAddress( "Street" );
	c2->setCity( "City" );
	c2->setZipCode( "Zip Code" );
	c2->setCountry( "Country" );
	order->setCustomer( c2 );
	CHECK( Manager::self()->commit(), true );
	
	// Check data has been saved correctly to the database
	cursor.setName( "customer" );
	cursor.select( "code = '0002'" );
	CHECK( cursor.next(), true );
	CHECK( cursor.value( "customername" ).toString(), QString("Customer Two") );
	CHECK( cursor.value( "address" ).toString(), QString("Street") );
	CHECK( cursor.value( "city" ).toString(), QString("City") );
	CHECK( cursor.value( "zipcode" ).toString(), QString("Zip Code") );
	CHECK( cursor.value( "country" ).toString(), QString("Country") );
	CHECK( cursor.next(), false );
	
	cursor.setName( "customerorder" );
	cursor.select( "number = 50000" );
	CHECK( cursor.next(), true );
	CHECK( variantToOid( cursor.value( "customer_customerorder" ) ), variantToOid( c2->oid() ) );
	CHECK( cursor.next(), false );


	order->articles()->add( a2 );
	CHECK( Manager::self()->commit(), true );
	
	// Check still both articles are in the database
	cursor.setName( "article_customerorder" );
	cursor.select( "article = " + oidToString( a1->oid() ) + " AND customerorder = " + oidToString( order->oid() ) );
	CHECK( cursor.next(), true );
	CHECK( cursor.next(), false );
	cursor.select( "article = " + oidToString( a2->oid() ) + " AND customerorder = " + oidToString( order->oid() ) );
	CHECK( cursor.next(), true );
	CHECK( cursor.next(), false );
	

	order->articles()->remove( a1 );
	CHECK( Manager::self()->commit(), true );
	
	// Check a1 has been removed from the list
	cursor.setName( "article_customerorder" );
	cursor.select( "article = " + oidToString( a1->oid() ) + " AND customerorder = " + oidToString( order->oid() ) );
	CHECK( cursor.next(), false );

	order->articles()->add( a1 );
	CHECK( Manager::self()->rollback(), true );

	// Check a1 hasn't appeared again
	cursor.setName( "article_customerorder" );
	cursor.select( "article = " + oidToString( a1->oid() ) + " AND customerorder = " + oidToString( order->oid() ) );
	CHECK( cursor.next(), false );

	order->setCustomer( c );
	CHECK( Manager::self()->commit(), true );
	
	cursor.setName( "customerorder" );
	cursor.select( "number = 50000" );
	CHECK( cursor.next(), true );
	CHECK( variantToOid( cursor.value( "customer_customerorder" ) ), c->oid() );
	CHECK( cursor.next(), false );
	

	order->setCustomer( c2 );
	CHECK( Manager::self()->rollback(), true );

	// Check customer hasn't changed
	cursor.setName( "customerorder" );
	cursor.select( "number = 50000" );
	CHECK( cursor.next(), true );
	CHECK( variantToOid( cursor.value( "customer_customerorder" ) ), c->oid() );
	CHECK( cursor.next(), false );

	CHECK( Manager::self()->commit(), true );

	// Check customer hasn't changed
	cursor.setName( "customerorder" );
	cursor.select( "number = 50000" );
	CHECK( cursor.next(), true );
	CHECK( variantToOid( cursor.value( "customer_customerorder" ) ), c->oid() );
	CHECK( cursor.next(), false );

	Manager::self()->remove( c2 );
	CHECK( Manager::self()->commit(), true );
	
		// Check customer hasn't changed
	cursor.setName( "customer" );
	cursor.select( "code = '0002'" );
	CHECK( cursor.next(), false );
}

void SqlBackendTest::collections()
{
	Collection col( "Article" );
	Article *article;
	ObjectIterator it( col.begin() );
	ObjectIterator end( col.end() );
	for ( ; it != end; ++it ) {
		article = static_cast<Article*>( *it );
		// As long as we don't have a way to sort collections, we won't be
		// able to make this test nicer
		if ( article->label() != "Article One"  && article->label() != "Article Two" ) {
			CHECK( true, false );
		}
	}
}

void SqlBackendTest::cache()
{
	QMap<int,OidType> list;
	int maxObjects = 100;

	Manager::self()->reset();
	Manager::self()->setMaxObjects( maxObjects );

	// Check modified objects are not freed
	ObjectRef<Article> a = Article::create();
	a->setCode( "Code1" );
	for ( int i = 0; i < maxObjects * 2; ++i ) {
		ObjectRef<CustomerOrder> o1 = CustomerOrder::create();
		list[i*3] = o1->oid();
		ObjectRef<Customer> c1 = Customer::create();
		list[i*3+1] = c1->oid();
		o1->setCustomer( c1 );
		ObjectRef<Article> a1 = Article::create();
		list[i*3+2] = a1->oid();
		o1->articles()->add( a1 );
	}
	CHECK( a->code(), QString( "Code1" ) );

	// Check when commiting object cache is purged to maxObjects
	CHECK( Manager::self()->commit(), true );
	CHECK( Manager::self()->count(), maxObjects );
	CHECK( Manager::self()->countObjectRelations(), maxObjects );
	CHECK( Manager::self()->countCollectionRelations(), maxObjects );

	// Check data has been saved correctly to DB
	for ( int i = 0; i < maxObjects * 2; ++i ) {
		QSqlCursor cursor( "customerorder" );
		cursor.select( "dboid = " + oidToString( list[i*3] ) );
		CHECK( cursor.next(), true );
		CHECK( variantToOid( cursor.value( "customer_customerorder" ) ), list[i*3 + 1] );
		CHECK( cursor.next(), false );

		cursor.setName( "article" );
		cursor.select( "dboid = " + oidToString( list[i*3+2] ) );
		CHECK( cursor.next(), true );
		CHECK( cursor.next(), false );

		cursor.setName( "customer" );
		cursor.select( "dboid = " + oidToString( list[i*3+1] ) );
		CHECK( cursor.next(), true );
		CHECK( cursor.next(), false );

		cursor.setName( "article_customerorder" );
		cursor.select( "article = " + oidToString( list[i*3+2] ) + " AND customerorder = " + oidToString( list[i*3] ) );
		CHECK( cursor.next(), true );
		CHECK( cursor.next(), false );
	}
	
	// Can we still reach article 1?
	CHECK( a->code(), QString( "Code1" ) );
	
	// Ensure we're still under maxObjects
	CHECK( Manager::self()->count(), maxObjects );
	CHECK( Manager::self()->countObjectRelations(), maxObjects );
	CHECK( Manager::self()->countCollectionRelations(), maxObjects );
}

void SqlBackendTest::allTests()
{
	QString dbname = "test";

	Classes::setup();

	// Drop the database if already exists
	KProcess *proc = new KProcess;
	*proc << "dropdb";
	*proc << dbname;
	proc->start();
	proc->wait();
	delete proc;

	// Create de database
	proc = new KProcess;
	*proc << "createdb";
	*proc << dbname;
	CHECK( proc->start(), true );
	proc->wait();
	if ( ! proc->normalExit() || proc->exitStatus() != 0 ) {
		CHECK( true, false );
		delete proc;
		return;
	}
	delete proc;

	QSqlDatabase *db = QSqlDatabase::addDatabase( "QPSQL7" );
	db->setDatabaseName( dbname );
	db->setUserName( "albert" );
	db->setPassword( "" );
	db->setHostName( "localhost" );
	if ( ! db->open() ) {
		kdDebug() << "Failed to open database: " << db->lastError().text() << endl;
		return;
	}
	DbBackendIface *backend = new SqlDbBackend( db );

	m_manager = new Manager( backend );
	m_manager->createSchema();

	transactions();
	collections();
	cache();

	delete m_manager;
}
