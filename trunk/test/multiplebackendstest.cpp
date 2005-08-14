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

void MultipleBackendsTest::xml2xml()
{
	DbBackendIface *backend1 = new XmlDbBackend( "database.xml" );
	m_manager1 = new Manager( backend1 );
	m_manager1->createSchema();
	
	DbBackendIface *backend2 = new XmlDbBackend( "database2.xml" );
	m_manager2 = new Manager( backend2 );
	m_manager2->createSchema();
	m_manager1->copy( m_manager2 );
	m_manager2->commit();
	
	QFile file;
	QByteArray a1, a2;
	file.setName( "database.xml" );
	file.open( IO_ReadOnly );
	a1 = file.readAll();
	file.close();
	file.setName( "database2.xml" );
	file.open( IO_ReadOnly );
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

	QSqlDatabase *db = QSqlDatabase::addDatabase( "QPSQL7" );
	db->setDatabaseName( dbname );
	db->setUserName( "albert" );
	db->setPassword( "" );
	db->setHostName( "localhost" );
	if ( ! db->open() ) {
		kdDebug() << "Failed to open database: " << db->lastError().text() << endl;
		return;
	}
	DbBackendIface *backend3 = new SqlDbBackend( db );
	m_manager3 = new Manager( backend3 );
	m_manager3->createSchema();
	m_manager2->copy( m_manager3 );
	m_manager3->commit();
	//QSqlDatabase::removeDatabase( db );
}

void MultipleBackendsTest::sql2xml()
{
	QString dbname = "testmultiple";
	QSqlDatabase *db = QSqlDatabase::addDatabase( "QPSQL7" );
	db->setDatabaseName( dbname );
	db->setUserName( "albert" );
	db->setPassword( "" );
	db->setHostName( "localhost" );
	if ( ! db->open() ) {
		kdDebug() << "Failed to open database: " << db->lastError().text() << endl;
		return;
	}
	DbBackendIface *backend4 = new InMemorySqlDbBackend( db );
	m_manager4 = new Manager( backend4 );

	DbBackendIface *backend5 = new XmlDbBackend( "database3.xml" );
	m_manager5 = new Manager( backend5 );
	
	m_manager4->copy( m_manager5 );
	m_manager5->commit();

	QFile file;
	QByteArray a1, a2;
	file.setName( "database.xml" );
	file.open( IO_ReadOnly );
	a1 = file.readAll();
	file.close();
	file.setName( "database3.xml" );
	file.open( IO_ReadOnly );
	a2 = file.readAll();
	file.close();
	CHECK( a1, a2 );
	//QSqlDatabase::removeDatabase( db );
}

void MultipleBackendsTest::allTests()
{
	Classes::setup();

	xml2xml();
	xml2sql();
	sql2xml();

	delete m_manager1;
	delete m_manager2;
	delete m_manager3;
	delete m_manager4;
	delete m_manager5;
}
