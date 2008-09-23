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

#include <kprocess.h>

#include <object.h>
#include <classes.h>
#include <sqldbbackend.h>
#include <manager.h>

#include "inheritancetest.h"
#include "classaba.h"


void InheritanceTest::browsing()
{
	ClassInfo *info = Classes::classInfo( "ClassA" );
	const QValueList<const ClassInfo*>& children = info->children();
	QValueListConstIterator<const ClassInfo*> it = children.begin();
	CHECK( QString( "ClassAA" ), (*it)->name() );
	++it;
	if ( it != children.end() ) {
		CHECK( (*it)->name(), QString( "ClassAB" ) );
		CHECK( (*it)->parent()->name(), QString( "ClassA" ) );
		CHECK( (*it)->children().count(), 1 );
		if ( (*it)->children().count() == 1 ) {
			CHECK( (* (*it)->children().begin())->name(), QString( "ClassABA" ) );
		}
	} else {
		CHECK( true, false );
	}
	info = Classes::classInfo( "ClassAA" );
	CHECK( info->containsProperty( "stringA" ), true );
	CHECK( info->containsProperty( "dateA" ), true );
	CHECK( info->containsProperty( "stringAA" ), true );
	CHECK( info->containsObject( "ClassA_ClassB" ), true );
	CHECK( info->containsCollection( "a_multiple_b" ), true );
	CHECK( info->containsObject( "ClassAA_ClassAB" ), true );
	CHECK( info->objectsCount(), 2 );
	CHECK( info->collectionsCount(), 1 );


	info = Classes::classInfo( "ClassAB" );
	CHECK( info->containsProperty( "stringA" ), true );
	CHECK( info->containsProperty( "dateA" ), true );
	CHECK( info->containsProperty( "stringAB" ), true );
	CHECK( info->containsObject( "ClassA_ClassB" ), true );
	CHECK( info->containsCollection( "a_multiple_b" ), true );
	CHECK( info->containsCollection( "ab_multiple_b" ), true );
	CHECK( info->containsCollection( "ClassAA_ClassAB" ), true );
	CHECK( info->objectsCount(), 1 );
	CHECK( info->collectionsCount(), 3 );

	info = Classes::classInfo( "ClassABA" );
	CHECK( info->containsProperty( "stringA" ), true );
	CHECK( info->containsProperty( "dateA" ), true );
	CHECK( info->containsProperty( "stringAB" ), true );
	CHECK( info->containsProperty( "stringABA" ), true );
	CHECK( info->property( "stringA" )->inherited(), true );
	CHECK( info->property( "dateA" )->inherited(), true );
	CHECK( info->property( "stringAB" )->inherited(), true );
	CHECK( info->property( "stringABA" )->inherited(), false );
	CHECK( info->containsObject( "ClassA_ClassB" ), true );
	CHECK( info->containsCollection( "a_multiple_b" ), true );
	CHECK( info->containsCollection( "ab_multiple_b" ), true );
	CHECK( info->containsCollection( "ClassAA_ClassAB" ), true );
	CHECK( info->objectsCount(), 1 );
	CHECK( info->collectionsCount(), 3 );
}

void InheritanceTest::sql( )
{
	QString dbname = "test";

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
	
	OidType oidABA;
	ObjectRef<ClassABA> object = ClassABA::create();
	oidABA = object->oid();
	object->setStringABA( "ABA" );
	object->setStringAB( "AB" );
	object->setStringA( "A" );
	object->setDateA( QDate::currentDate() );
	m_manager->commit();
//	TODO: enable freeCache once it's working
//	m_manager->freeCache();
	m_manager->reset();
	CHECK( m_manager->objects().count(), 0 );

	ObjectRef<ClassA> objectA = ClassA::create( oidABA );
	CHECK( objectA.isNull(), false );
	if ( objectA.isNull() )
		return;
	CHECK( objectA->stringA(), QString( "A" ) );
	//CHECK( objectA->dateA(), QDate::currentDate() );
	
	m_manager->reset();
	CHECK( m_manager->objects().count(), 0 );

	ObjectRef<ClassAB> objectAB = ClassAB::create( oidABA );
	CHECK( objectAB.isNull(), false );
	if ( objectAB.isNull() )
		return;
	CHECK( objectAB->stringA(), QString( "A" ) );
	//CHECK( objectAB->dateA(), QDate::currentDate() );
	CHECK( objectAB->stringAB(), QString( "AB" ) );
}

void InheritanceTest::allTests()
{
	Classes::setup();
	browsing();
//	sql();
}
