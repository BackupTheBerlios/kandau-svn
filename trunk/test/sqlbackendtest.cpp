/***************************************************************************
 *   Copyright (C) 2004 by Albert Cervera Areny                            *
 *   albertca@hotpop.com                                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public License     *
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
	c->setDefaultArticle( a1 );
	c->setSecondDefaultArticle( a2 );
	c->discountedArticles()->add( a1 );
	c->discountedArticles()->add( a2 );
	c->adaptedArticles()->add( a1 );
	c->adaptedArticles()->add( a2 );
	order->setCustomer( c );

	CHECK( Manager::self()->commit(), true );
	
	// Check data has been saved correctly to the database
	QSqlCursor cursor( "customerorder" );
	cursor.select( "number = 50000" );
	CHECK( cursor.next(), true );
	CHECK( cursor.value( "date" ).toDate().toString() , QDate::currentDate().toString() );
	CHECK( variantToOid( cursor.value( "customerorder_customerorder" ) ), variantToOid( cursor.value( "dboid" ) ) );
	CHECK( variantToOid( cursor.value( "customer_customerorder" ) ), c->oid() );
	CHECK( cursor.contains( "numberOfArticles" ), false );
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

	cursor.setName( "discounted_articles" );
	cursor.select( "article = " + oidToString( a1->oid() ) + " AND customer = " + oidToString( c->oid() ) );
	CHECK( cursor.next(), true );
	CHECK( cursor.next(), false );
	
	cursor.setName( "adapted_articles" );
	cursor.select( "article = " + oidToString( a1->oid() ) + " AND customer = " + oidToString( c->oid() ) );
	CHECK( cursor.next(), true );
	CHECK( cursor.next(), false );

//	exit();
		
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
	CHECK( variantToOid( cursor.value( "default_article" ) ), a1->oid() );
	CHECK( variantToOid( cursor.value( "second_default_article" ) ), a2->oid() );
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
	//CHECK( variantToOid( cursor.value( "customer_customerorder" ) ), variantToOid( c2->oid() ) );
	CHECK( variantToOid( cursor.value( "customer_customerorder" ) ), variantToOid( c->oid() ) );
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
	CollectionIterator it( col.begin() );
	CollectionIterator end( col.end() );
	for ( ; it != end; ++it ) {
		article = static_cast<Article*>( *it );
		// As long as we don't have a way to sort collections, we won't be
		// able to make this test nicer
		if ( article->label() != "Article One"  && article->label() != "Article Two" ) {
			CHECK( true, false );
		}
	}
	
	ObjectRef<Customer> c1 = Customer::create();
	c1->setCustomerName( "foo" );
	ObjectRef<Customer> c2 = Customer::create();
	c2->setCustomerName( "bar" );

	ObjectRef<Article> a1 = Article::create();
	a1->setLabel( "foo" );
	a1->setDescription( "liluli foo liluli" );
	ObjectRef<CustomerOrder> o1 = CustomerOrder::create();
	o1->setNumber( 3000 );
	o1->setCustomer( c1 );
	o1->articles()->add( a1 );
	Manager::self()->commit();

	Collection col2( "SELECT Customer.* WHERE Customer.Customer_CustomerOrder.Article_CustomerOrder.description like '%foo%'" );
	Customer *customer;
	CollectionIterator it2( col2.begin() );
	CollectionIterator end2( col2.end() );
	CHECK( col2.begin() == col2.end(), false );
	for ( ; it2 != end2; ++it2 ) {
		customer = static_cast<Customer*>( *it2 );
		// As long as we don't have a way to sort collections, we won't be
		// able to make this test nicer
		CHECK( customer->customerName(), QString( "foo" ) );
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

void SqlBackendTest::freeing()
{
	Manager::self()->reset();
	Manager::self()->setMaxObjects( 1 );
	OidType oid1, oid2, oid3;
	ObjectRef<Article> a1 = Article::create();
	a1->setCode( "1" );
	ObjectRef<Article> a2 = Article::create();
	a2->setCode( "2" );
	ObjectRef<Article> a3 = Article::create();
	a3->setCode( "3" );
	oid1 = a1.oid();
	oid2 = a2.oid();
	oid3 = a3.oid();
	Manager::self()->commit();
	CHECK( Manager::self()->count(), 1 );
	ObjectRef<Article> b1 = Article::create( oid1 );
	CHECK( Manager::self()->count(), 1 );
	ObjectRef<Article> b2 = Article::create( oid2 );
	CHECK( Manager::self()->count(), 1 );
	ObjectRef<Article> b3 = Article::create( oid3 );
	CHECK( Manager::self()->count(), 1 );
	CHECK( b1->oid(), oid1 );
	CHECK( Manager::self()->count(), 1 );
	CHECK( b2->oid(), oid2 );
	CHECK( Manager::self()->count(), 1 );
	CHECK( b3->oid(), oid3 );
	CHECK( Manager::self()->count(), 1 );
	CHECK( b3->code(), QString( "3" ) );
	CHECK( Manager::self()->count(), 1 );
	CHECK( b1->code(), QString( "1" ) );
	CHECK( Manager::self()->count(), 1 );
	CHECK( b2->code(), QString( "2" ) );
	CHECK( Manager::self()->count(), 1 );
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

	// Create the database
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
	freeing();

	delete m_manager;
}
