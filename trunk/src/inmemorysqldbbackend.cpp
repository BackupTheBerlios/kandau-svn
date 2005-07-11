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
#include <qsqlcursor.h>
#include <qsqldatabase.h>

#include "inmemorysqldbbackend.h"
#include "classes.h"
#include "object.h"

InMemorySqlDbBackend::InMemorySqlDbBackend( QSqlDatabase * db )
{
	m_currentOid = 1;
	m_db = db;
}

InMemorySqlDbBackend::~InMemorySqlDbBackend()
{
}

/* Called at the Manager constructor */
void InMemorySqlDbBackend::setup()
{
	OidType maxOid, oid;
	ClassInfoIterator m_it( Classes::begin() );
	ClassInfoIterator m_end( Classes::end() );

	maxOid = 0;
	for ( ; m_it != m_end; ++m_it ) {
		ClassInfo* info = (*m_it);
		QSqlCursor cursor( info->name() );
		cursor.select();
		kdDebug() << k_funcinfo << " loading class: " << info->name() << endl;
		kdDebug() << k_funcinfo << " LastQuery: " << cursor.lastQuery() << endl;
		kdDebug() << k_funcinfo << " LastError: " << cursor.lastError().text() << endl;
		while ( cursor.next() ) {
			oid = variantToOid( cursor.value( "oid" ) );
			
			ObjectRef<Object> object = Classes::classInfo( info->name() )->create( oid );
			loadObject( cursor, object );
			kdDebug() << k_funcinfo << " object loaded: " << oidToString( object->oid() ) << endl;
			if ( oid > maxOid )
				maxOid = oid;
		}
	}
	m_currentOid = maxOid + 1;	// m_currentOid = max oid found + 1
}

void InMemorySqlDbBackend::loadObject( const QSqlCursor& cursor, ObjectRef<Object> object )
{
	PropertyIterator pIt( object->propertiesBegin() );
	PropertyIterator pEnd( object->propertiesEnd() );

	object->setOid( variantToOid( cursor.value( "dboid" ) ) );
	// Load all properties
	for ( ; pIt != pEnd; ++pIt )
		(*pIt).setValue( cursor.value( (*pIt).name() ) );
	// Load all object relations
	RelatedObjectsIterator oIt( object->classInfo()->objectsBegin() );
	RelatedObjectsIterator oEnd( object->classInfo()->objectsEnd() );
	for ( ; oIt != oEnd; ++oIt )
		object->setObject( (*oIt)->name(), variantToOid( cursor.value( (*oIt)->name() ) ) );

	CollectionIterator cIt( object->collectionsBegin() );
	CollectionIterator cEnd( object->collectionsEnd() );
	for ( ; cIt != cEnd; ++cIt ) {
		QSqlCursor cCursor( (*cIt)->collectionInfo()->name() );
		cCursor.select( object->classInfo()->name() + "=" + oidToString( object->oid() ) );
		while ( cCursor.next() ) {
			(*cIt)->simpleAdd( variantToOid( cCursor.value( (*cIt)->collectionInfo()->childrenClassInfo()->name() ) ) );
		}
	}
}

void InMemorySqlDbBackend::saveObject( ObjectRef<Object> object )
{
	QSqlCursor cursor( object->classInfo()->name() );
	cursor.select();
	QSqlRecord *buffer = cursor.primeInsert();

	buffer->setValue( "dboid", oidToVariant( object->oid() ) );

	PropertyIterator pIt( object->propertiesBegin() );
	PropertyIterator pEnd( object->propertiesEnd() );
	// Load all properties
	for ( ; pIt != pEnd; ++pIt )
		buffer->setValue( (*pIt).name(), (*pIt).value() );

	// Load all object relations
	RelatedObjectsConstIterator oIt( object->classInfo()->objectsBegin() );
	RelatedObjectsConstIterator oEnd( object->classInfo()->objectsEnd() );
	for ( ; oIt != oEnd; ++oIt ) {
		if ( object->object( (*oIt)->name() ) != 0 )
			buffer->setValue( (*oIt)->name(), object->object( (*oIt)->name() )->oid() );
		else
			buffer->setNull( (*oIt)->name() );
	}
	cursor.insert();

	CollectionIterator cIt( object->collectionsBegin() );
	CollectionIterator cEnd( object->collectionsEnd() );
	for ( ; cIt != cEnd; ++cIt ) {
		// Ensure data is only inserted once
		if ( m_savedCollections.contains( (*cIt)->collectionInfo()->name() ) )
			continue;

		m_savedCollections << (*cIt)->collectionInfo()->name();

		QSqlCursor cCursor( (*cIt)->collectionInfo()->name() );
		cCursor.select();

		ObjectIterator coIt( (*cIt)->begin() );
		ObjectIterator coEnd( (*cIt)->end() );
		for ( ; coIt != coEnd; ++coIt ) {
			buffer = cCursor.primeInsert();
			buffer->setValue( object->classInfo()->name(), object->oid() );
			buffer->setValue( (*coIt)->classInfo()->name(), (*coIt)->oid() );
			cCursor.insert();
		}
	}
}

void InMemorySqlDbBackend::shutdown()
{
	commit();
}

/* Object management related functions */
bool InMemorySqlDbBackend::load( const OidType& /*oid*/, Object* /*object*/ )
{
	//assert( false );
	kdDebug() << "Entering InMemorySqlDbBackend::load(): This should never happen!" << endl;
	return true;
}

bool InMemorySqlDbBackend::save( Object* /*object*/ )
{
	return true;
}

bool InMemorySqlDbBackend::remove( Object* /*object*/ )
{
	return true;
}

/* Collection management related functions */
bool InMemorySqlDbBackend::load( Collection* /*collection*/ )
{
	return true;
}

bool InMemorySqlDbBackend::add( Collection* /*collection*/, Object* /*object*/ )
{
	return true;
}

bool InMemorySqlDbBackend::remove( Collection* /*collection*/, const OidType& /*object*/ )
{
	return true;
}

QString InMemorySqlDbBackend::idFieldName( RelatedCollection *collection ) const
{
	assert( collection );
	if ( collection->isNToOne() )
		return "dboid";
	else
		return collection->childrenClassInfo()->name();
}

bool InMemorySqlDbBackend::createSchema()
{
	QStringList tables;
	QStringList constraints;
	QString exec;
	Object *object;
	Property prop;
	uint i;

	// Create the tables
	ClassInfoIterator it ( Classes::begin() );
	ClassInfoIterator end ( Classes::end() );
	ClassInfo *currentClass;
	for ( ; it != end; ++it ) {
		currentClass = *it;
		// We need to create an object because it's the only way to iterate through a class' properties right now
		object = currentClass->createInstance();
		exec = "CREATE TABLE " +  currentClass->name().lower() + " ( dboid BIGINT PRIMARY KEY, ";

		// Create properties fields
		PropertyIterator pIt( object->propertiesBegin() );
		PropertyIterator pEnd( object->propertiesEnd() );
		for ( ; pIt != pEnd; ++pIt ) {
			prop = *pIt;
			kdDebug() << "property: " << prop.name() << endl;
			exec += prop.name() + " " + sqlType( prop.type() ) + ", ";
		}
		delete object;

		// Create related objects fields
		RelatedObjectsConstIterator oIt( currentClass->objectsBegin() );
		RelatedObjectsConstIterator oEnd( currentClass->objectsEnd() );
		RelatedObject *rObj;
		for ( ; oIt != oEnd; ++oIt ) {
			rObj = *oIt;
			exec += rObj->name().lower() + " BIGINT DEFAULT NULL, ";
			constraints << currentClass->name().lower() + "-" + rObj->name().lower() + "-" + rObj->relatedClassInfo()->name().lower();
			//constraints << rObj->name();
			//constraints << tableName( object ) + "-" + tableName( oIt.key() );
		}

		// Search in all the classes if they have N - 1 relations with the current class
		ClassInfoIterator cIt( Classes::begin() );
		ClassInfoIterator cEnd( Classes::end() );
		ClassInfo *cInfo;
		for ( ; cIt != cEnd; ++cIt ) {
			cInfo = *cIt;
			RelatedCollectionsIterator colIt( cInfo->collectionsBegin() );
			RelatedCollectionsIterator colEnd( cInfo->collectionsEnd() );
			RelatedCollection *rCol;
			for ( ; colIt != colEnd; ++colIt ) {
				rCol = *colIt;
				if ( rCol->childrenClassInfo()->name() == currentClass->name() && rCol->isNToOne() ) {
					exec += rCol->name().lower() + " BIGINT DEFAULT NULL, ";
					constraints << currentClass->name().lower() + "-" + rCol->name().lower() + "-" + rCol->parentClassInfo()->name().lower();
					//constraints << rCol->name();
					//constraints << tableName( object ) + "-" + tableName( obj );
				}
			}
		}

		RelatedCollectionsIterator colIt( currentClass->collectionsBegin() );
		RelatedCollectionsIterator colEnd( currentClass->collectionsEnd() );
		RelatedCollection *col;
		for ( ; colIt != colEnd; ++colIt ) {
			col = *colIt;
			if ( ! tables.grep( col->name() ).count() > 0 && ! col->isNToOne() ) {
				tables << col->name().lower() + "-"  + col->parentClassInfo()->name().lower() + "-" + idFieldName( col ).lower();
			}
		}

		// Take off the colon and space
		exec = exec.left( exec.length() - 2 );
		exec += ");";
		m_db->exec( exec );
		kdDebug() << k_funcinfo << exec << endl;
		kdDebug() << k_funcinfo << m_db->lastError().text()  << endl;
	}

	// Creem les taules de relacions
	QStringList list;
	for ( i = 0; i < tables.count(); ++i ) {
		list = QStringList::split( QString( "-" ), tables[ i ] );
		kdDebug() << "Creant taula... " << list[0] << endl;
   		exec = "CREATE TABLE " + list[ 0 ] + " ( " + list[ 1 ] + " BIGINT NOT NULL REFERENCES " + list[ 1 ] + " DEFERRABLE INITIALLY DEFERRED, "+ list[ 2 ] + " BIGINT NOT NULL REFERENCES " + list[2] +" DEFERRABLE INITIALLY DEFERRED, PRIMARY KEY( "+ list[1] +" , " + list[2] + " ) );";

		kdDebug() << exec << endl;
		m_db->exec( exec );
		kdDebug() << m_db->lastError().text() << endl;
	}

	for ( i = 0; i < constraints.count(); ++i ) {
		list = QStringList::split( QString( "-" ), constraints[ i ] );
		exec = "ALTER TABLE " + list[ 0 ] + " ADD FOREIGN KEY (" + list[ 1 ] + ") REFERENCES " + list[ 2 ] + "( dboid ) DEFERRABLE INITIALLY DEFERRED";
		kdDebug() << exec << endl;
		m_db->exec( exec );
	}

	start();
	return true;
}

/*! Decides whether the object changed in the database since last load */
bool InMemorySqlDbBackend::hasChanged( Object* /*object*/ )
{
	return false;
}

/*!
This function must provide a new unique Oid. Used for newly created
objects.
*/
OidType InMemorySqlDbBackend::newOid()
{
	return m_currentOid++;
}


void InMemorySqlDbBackend::setRelation( const OidType& /*oid*/, const QString& /*relation*/, const OidType& /*oidRelated*/, const OidType& /*oldOid*/ )
{
}

/*!
Starts a transaction
*/
bool InMemorySqlDbBackend::start()
{
	return true;
}

/*!
Commits the current transaction
*/
bool InMemorySqlDbBackend::commit()
{
	m_db->transaction();
	m_db->exec( "SET CONSTRAINTS ALL DEFERRED" );

	// Remove all existing information
	ClassInfoIterator m_it( Classes::begin() );
	ClassInfoIterator m_end( Classes::end() );
	for ( ; m_it != m_end; ++m_it )
		m_db->exec( "DELETE FROM " + (*m_it)->name() );

	m_savedCollections.clear();
	ManagerObjectIterator it( Manager::self()->begin() );
	ManagerObjectIterator end( Manager::self()->end() );
	for ( ; it != end; ++it )
		saveObject( *it );
	m_db->commit();
	return true;
}

QString InMemorySqlDbBackend::sqlType( QVariant::Type type )
{
	// TODO: Review the unsigned integer. Type mapping might be incorrect.
	switch ( type ) {
		case QVariant::String:
			return "VARCHAR";
		case QVariant::UInt:
			return "INTEGER";
		case QVariant::Int:
			return "INTEGER";
		case QVariant::DateTime:
			return "TIMESTAMP";
		case QVariant::Double:
			return "NUMERIC(10,2)";
		case QVariant::ULongLong:
			return "NUMERIC(12)";
		case QVariant::Image:
			return "BYTEA";
		case QVariant::Pixmap:
			return "BYTEA";
		case QVariant::Date:
			return "DATE";
		case QVariant::Time:
			return "TIME";
		case QVariant::List:
			return "BYTEA";
		case QVariant::ByteArray:
			return "BYTEA";
		default:
			kdDebug() << "Tipus sense classificar: " << QVariant::typeToName( type ) << endl;
			assert( false );
			return "INTEGER";
	}
}

/*!
Aborts the current transaction
*/
bool InMemorySqlDbBackend::rollback()
{
	setup();
	return true;
}
