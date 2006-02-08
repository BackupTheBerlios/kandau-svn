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
#include <kprocess.h>
#include <qsqldatabase.h>

#include <classes.h>
#include <dynamicobject.h>
#include <sqldbbackend.h>

#include "dynamicobjectstest.h"
#include "customer.h"
#include "article.h"

void DynamicObjectsTest::allTests()
{
	QString dbname = "testdynamic";
	
	Classes::setup();

	Classes::addClass( "Test", DynamicObject::createInstance, 0 );
	ClassInfo *ci = Classes::classInfo( "Test" );
	ci->addObject( "Customer", "Customer_Test", &Customer::createInstance );
	ci->addCollection( "Article", "Article_Test" );
	ci->addProperty( "Property1", QVariant::String );
	ci->addProperty( "Property2", QVariant::ULongLong );

	Classes::setupRelations();

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
	m_manager->setMaxObjects( 1 );
	m_manager->createSchema();


	ObjectRef<Customer> customer = Customer::create();
	customer->setCustomerName( "Name of the customer" );

	ObjectRef<Article> a1 = Article::create();
	a1->setCode( "00001" );
	ObjectRef<Article> a2 = Article::create();
	a2->setCode( "00002" );

	ObjectRef<Object> obj = Classes::classInfo( "Test" )->create();
	CHECK( obj->property( "Property1" ).type(), QVariant::String );
	CHECK( obj->property( "Property2" ).type(), QVariant::ULongLong );
	CHECK( obj->containsObject( "Customer_Test" ), true );
	CHECK( obj->containsCollection( "Article_Test" ), true );
	obj->setProperty( "Property1", "Property number one" );
	obj->setProperty( "Property2", 2 );
	CHECK( obj->property( QString( "Property1" ) ).value().toString(), QString( "Property number one" ) );
	CHECK( obj->property( QString( "Property2" ) ).value().toULongLong(), 2 );
	
	obj->setObject( "Customer_Test", customer );
	obj->collection( "Article_Test" )->add( a1 );
	obj->collection( "Article_Test" )->add( a2 );

	m_manager->commit();

	CHECK( obj->property( "Property1" ).value().toString(), QString( "Property number one" ) );

	delete m_manager;
}
