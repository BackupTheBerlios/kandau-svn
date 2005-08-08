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

	a1->setDescription( "MODIFIED description of article number one" );
	CHECK( Manager::self()->commit(), true );

	ObjectRef<Customer> c2 = Customer::create();
	c2->setCode( "0002" );
	c2->setCustomerName( "Customer Two" );
	c2->setAddress( "Street" );
	c2->setCity( "City" );
	c2->setZipCode( "Zip Code" );
	c2->setCountry( "Country" );
	order->setCustomer( c2 );
	CHECK( Manager::self()->commit(), true );

	order->articles()->add( a2 );
	CHECK( Manager::self()->commit(), true );

	order->articles()->remove( a1 );
	CHECK( Manager::self()->commit(), true );

	order->articles()->add( a1 );
	CHECK( Manager::self()->rollback(), true );

	order->setCustomer( c );
	CHECK( Manager::self()->commit(), true );

	order->setCustomer( c2 );
	CHECK( Manager::self()->rollback(), true );

	CHECK( Manager::self()->commit(), true );

	Manager::self()->remove( c2 );
	CHECK( Manager::self()->commit(), true );
}

void SqlBackendTest::collections()
{
	/*
	Collection col( "SELECT * FROM Article" );
	Collection col( "SELECT article WHERE nom like '%A%'" );
	Collection col( "customerorder.article_customerorder.* WHERE customerorder.customer_customerorder.city='Barcelona' AND customerorder.article_customerorder.description LIKE '%pepet%'" );
	*/
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
	int maxObjects = 100;

	Manager::self()->reset();
	Manager::self()->setMaxObjects( maxObjects );
	
	// Check modified objects are not freed
	ObjectRef<Article> a1 = Article::create();
	a1->setCode( "Code1" );
	for ( int i = 0; i < maxObjects * 2; ++i ) {
		ObjectRef<CustomerOrder> o1 = CustomerOrder::create();
		o1->setCustomer( Customer::create() );
		o1->articles()->add( Article::create() );
	}
	CHECK( a1->code(), QString( "Code1" ) );
	
	// Check when commiting object cache is purged to maxObjects
	CHECK( Manager::self()->commit(), true );
	CHECK( Manager::self()->count(), maxObjects );
	CHECK( Manager::self()->countObjectRelations(), maxObjects );
	CHECK( Manager::self()->countCollectionRelations(), maxObjects );
	
	// Can we still reach article 1?
	CHECK( a1->code(), QString( "Code1" ) );
	
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
