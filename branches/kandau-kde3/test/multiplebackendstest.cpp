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
#include <qfile.h>
#include <qsqldatabase.h>

#include <kdebug.h>
#include <kprocess.h>
#include <qsqlcursor.h>


#include <manager.h>
#include <xmldbbackend.h>
#include <sqldbbackend.h>
#include <inmemorysqldbbackend.h>
#include <object.h>
#include "customer.h"
#include "customerorder.h"
#include "article.h"

#include "multiplebackendstest.h"

using namespace Kandau;

void MultipleBackendsTest::xml2xml()
{
	DbBackendIface *backend1 = new XmlDbBackend( "database.xml" );
	Manager *manager1 = new Manager( backend1 );
	manager1->createSchema();
	
	DbBackendIface *backend2 = new XmlDbBackend( "database2.xml", true );
	Manager *manager2 = new Manager( backend2 );
	manager2->createSchema();
	manager1->copyTo( manager2 );
	manager2->commit();

	delete manager2;
	delete manager1;

	QFile file;
	QByteArray a1, a2;
	file.setName( "database.xml" );
	CHECK( file.open( IO_ReadOnly ), true );
	a1 = file.readAll();
	file.close();
	file.setName( "database2.xml" );
	CHECK( file.open( IO_ReadOnly ), true );
	a2 = file.readAll();
	file.close();
	CHECK( a1, a2 );
}

void MultipleBackendsTest::xml2sql()
{
	QString dbname = "testmultiple";
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

	DbBackendIface *backend1 = new XmlDbBackend( "database.xml" );
	Manager *manager1 = new Manager( backend1 );
	manager1->createSchema();

	QSqlDatabase *db = QSqlDatabase::addDatabase( "QPSQL7" );
	db->setDatabaseName( dbname );
	db->setUserName( "ak213" );
	db->setPassword( "ak" );
	db->setHostName( "localhost" );
	if ( ! db->open() ) {
		kdDebug() << "Failed to open database: " << db->lastError().text() << endl;
		return;
	}
	DbBackendIface *backend2 = new SqlDbBackend( db );
	Manager *manager2 = new Manager( backend2 );
	manager2->createSchema();
	manager1->copyTo( manager2 );
	manager2->commit();
	delete manager2;
	delete manager1;
	QSqlDatabase::removeDatabase( db );
}

void MultipleBackendsTest::sql2xml()
{
	QString dbname = "testmultiple";
	QSqlDatabase *db = QSqlDatabase::addDatabase( "QPSQL7" );
	db->setDatabaseName( dbname );
	db->setUserName( "ak213" );
	db->setPassword( "ak" );
	db->setHostName( "localhost" );
	if ( ! db->open() ) {
		kdDebug() << "Failed to open database: " << db->lastError().text() << endl;
		return;
	}
	DbBackendIface *backend1 = new InMemorySqlDbBackend( db );
	Manager *manager1 = new Manager( backend1 );

	DbBackendIface *backend2 = new XmlDbBackend( "database3.xml", true );
	Manager *manager2 = new Manager( backend2 );
	
	manager1->copyTo( manager2 );
	manager2->commit();

	delete manager1;
	delete manager2;
	QSqlDatabase::removeDatabase( db );
	
	QFile file;
	QByteArray a1, a2;
	file.setName( "database.xml" );
	CHECK( file.open( IO_ReadOnly ), true );
	a1 = file.readAll();
	file.close();
	file.setName( "database3.xml" );
	CHECK( file.open( IO_ReadOnly ), true );
	a2 = file.readAll();
	file.close();
	CHECK( a1, a2 );
}

void MultipleBackendsTest::creates()
{
	DbBackendIface *backend1 = new XmlDbBackend( "databaseA.xml", true );
	Manager *m1 = new Manager( backend1 );

	DbBackendIface *backend2 = new XmlDbBackend( "databaseB.xml", true );
	Manager *m2 = new Manager( backend2 );

	ObjectRef<Customer> c1 = Customer::create( m1 );
	c1->setCustomerName( "Customer Name" );
	ObjectRef<Customer> c2 = Customer::create( m2 );
	c2->setCustomerName( "Customer Name" );
	
	CHECK( m1->objects().count(), m2->objects().count() );
	
	CHECK( m1->commit(), true );
	CHECK( m2->commit(), true );
	
	
	QFile file;
	QByteArray a1, a2;
	file.setName( "databaseA.xml" );
	CHECK( file.open( IO_ReadOnly ), true );
	a1 = file.readAll();
	file.close();
	file.setName( "databaseB.xml" );
	CHECK( file.open( IO_ReadOnly ), true );
	a2 = file.readAll();
	file.close();
	CHECK( a1, a2 );

	delete m1;
	delete m2;
}

void MultipleBackendsTest::allTests()
{
	Classes::setup();

	xml2xml();
	xml2sql();
	sql2xml();
	creates();
}
