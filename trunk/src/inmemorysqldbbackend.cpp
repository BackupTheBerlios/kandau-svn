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

void InMemorySqlDbBackend::setup( Manager* manager )
{
	m_manager = manager;
	m_manager->setMaxObjects( Manager::Unlimited );
	init();
}

void InMemorySqlDbBackend::init()
{
	m_manager->reset();

	QStringList tables = m_db->tables();
	OidType maxOid, oid;
	ClassInfoIterator m_it( Classes::begin() );
	ClassInfoIterator m_end( Classes::end() );

	maxOid = 0;
	for ( ; m_it != m_end; ++m_it ) {
		ClassInfo* info = (*m_it);

		// Don't try to read from recordset if table doesn't exist
		if ( tables.grep( info->name(), false ).count() == 0 )
			continue;

		QSqlCursor cursor( info->name().lower());
		cursor.select();
		if ( m_db->lastError().type() != QSqlError::None ) {
			//kdDebug() << k_funcinfo << exec << endl;
			kdDebug() << k_funcinfo << m_db->lastError().text()  << endl;
		}

		while ( cursor.next() ) {
			oid = variantToOid( cursor.value( "dboid" ) );

			Object* object = Classes::classInfo( info->name() )->create( oid, m_manager, true );
			assert( object );
			loadObject( cursor, object );
			if ( oid > maxOid )
				maxOid = oid;
		}
	}
	m_currentOid = maxOid + 1;	// m_currentOid = max oid found + 1
}

void InMemorySqlDbBackend::loadObject( const QSqlCursor& cursor, Object* object )
{
	assert( object );
	PropertiesIterator pIt( object->propertiesBegin() );
	PropertiesIterator pEnd( object->propertiesEnd() );

//	object->setOid( variantToOid( cursor.value( "dboid" ) ) );
	// Load all properties
	for ( ; pIt != pEnd; ++pIt ) {
		if ( (*pIt).readOnly() == false )
			(*pIt).setValue( cursor.value( (*pIt).name() ) );
	}
	// Load all object relations
	RelatedObjectsConstIterator oIt( object->classInfo()->objectsBegin() );
	RelatedObjectsConstIterator oEnd( object->classInfo()->objectsEnd() );
	for ( ; oIt != oEnd; ++oIt )
		object->setObject( (*oIt)->name(), variantToOid( cursor.value( (*oIt)->name() ) ) );

	CollectionsIterator cIt( object->collectionsBegin() );
	CollectionsIterator cEnd( object->collectionsEnd() );
	for ( ; cIt != cEnd; ++cIt ) {
		if ( (*cIt)->collectionInfo()->isNToOne() )
			continue;
	
		QSqlCursor cCursor( (*cIt)->collectionInfo()->name().lower() );
		cCursor.select( object->classInfo()->name() + "=" + oidToString( object->oid() ) );
		if ( m_db->lastError().type() != QSqlError::None ) {
			//kdDebug() << k_funcinfo << exec << endl;
			kdDebug() << k_funcinfo << m_db->lastError().text()  << endl;
		}

		while ( cCursor.next() ) {
			(*cIt)->simpleAdd( variantToOid( cCursor.value( (*cIt)->collectionInfo()->childrenClassInfo()->name() ) ) );
		}
	}
}

void InMemorySqlDbBackend::saveObject( Object* object )
{
	assert( object );
	assert( object->classInfo() );
	QSqlCursor cursor( object->classInfo()->name().lower() );
	cursor.select();
	if ( m_db->lastError().type() != QSqlError::None ) {
		//kdDebug() << k_funcinfo << exec << endl;
		kdDebug() << k_funcinfo << m_db->lastError().text()  << endl;
	}

	QSqlRecord *buffer = cursor.primeInsert();

	buffer->setValue( "dboid", oidToVariant( object->oid() ) );

	PropertiesIterator pIt( object->propertiesBegin() );
	PropertiesIterator pEnd( object->propertiesEnd() );
	for ( ; pIt != pEnd; ++pIt ) {
		if ( (*pIt).readOnly() == false )
			buffer->setValue( (*pIt).name(), (*pIt).value() );
	}
	RelatedObjectsConstIterator oIt( object->classInfo()->objectsBegin() );
	RelatedObjectsConstIterator oEnd( object->classInfo()->objectsEnd() );
	for ( ; oIt != oEnd; ++oIt ) {
		if ( object->object( (*oIt)->name() ) != 0 )
			buffer->setValue( (*oIt)->name(), object->object( (*oIt)->name() )->oid() );
		else
			buffer->setNull( (*oIt)->name() );
	}
	cursor.insert();

	CollectionsIterator cIt( object->collectionsBegin() );
	CollectionsIterator cEnd( object->collectionsEnd() );
	for ( ; cIt != cEnd; ++cIt ) {
		if ( (*cIt)->collectionInfo()->isNToOne() )
			continue;
		
		// Ensure data is only inserted once
		if ( m_savedCollections.contains( (*cIt)->collectionInfo()->name() ) )
			continue;

		m_savedCollections << (*cIt)->collectionInfo()->name();

		QSqlCursor cCursor( (*cIt)->collectionInfo()->name().lower() );
		cCursor.select();
		if ( m_db->lastError().type() != QSqlError::None ) {
			//kdDebug() << k_funcinfo << exec << endl;
			kdDebug() << k_funcinfo << m_db->lastError().text()  << endl;
		}

		CollectionIterator coIt( (*cIt)->begin() );
		CollectionIterator coEnd( (*cIt)->end() );
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
	kdDebug() << "Entering InMemorySqlDbBackend::load(): This should never happen!" << endl;
	return true;
}

/* Collection management related functions */
bool InMemorySqlDbBackend::load( Collection* /*collection*/ )
{
	return true;
}

bool InMemorySqlDbBackend::load( OidType* /*relatedOid*/, const OidType& /*oid*/, const RelatedObject* /*related*/ )
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
	PropertyInfo *prop;
	uint i;

	// Create the tables
	ClassInfoIterator it ( Classes::begin() );
	ClassInfoIterator end ( Classes::end() );
	ClassInfo *currentClass;
	for ( ; it != end; ++it ) {
		currentClass = *it;
		exec = "CREATE TABLE " +  currentClass->name().lower() + " ( dboid BIGINT PRIMARY KEY, ";

		// Create properties fields
		PropertiesInfoConstIterator pIt( currentClass->propertiesBegin() );
		PropertiesInfoConstIterator pEnd( currentClass->propertiesEnd() );
		for ( ; pIt != pEnd; ++pIt ) {
			prop = *pIt;
			if ( prop->readOnly() == false )
				exec += prop->name() + " " + sqlType( prop->type() ) + ", ";
		}

		// Create related objects fields
		RelatedObjectsConstIterator oIt( currentClass->objectsBegin() );
		RelatedObjectsConstIterator oEnd( currentClass->objectsEnd() );
		RelatedObject *rObj;
		for ( ; oIt != oEnd; ++oIt ) {
			rObj = *oIt;
			exec += rObj->name().lower() + " BIGINT DEFAULT NULL, ";
			constraints << currentClass->name().lower() + "-" + rObj->name().lower() + "-" + rObj->relatedClassInfo()->name().lower();
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
				if ( rCol->childrenClassInfo()->name() == currentClass->name() && rCol->isNToOne() && constraints.grep( rCol->name().lower() ).count() == 0 ) {
					exec += rCol->name().lower() + " BIGINT DEFAULT NULL, ";
					constraints << currentClass->name().lower() + "-" + rCol->name().lower() + "-" + rCol->parentClassInfo()->name().lower();
				}
			}
		}

		RelatedCollectionsIterator colIt( currentClass->collectionsBegin() );
		RelatedCollectionsIterator colEnd( currentClass->collectionsEnd() );
		RelatedCollection *col;
		for ( ; colIt != colEnd; ++colIt ) {
			col = *colIt;
			if ( ! tables.grep( col->name().lower() ).count() > 0 && ! col->isNToOne() ) {
				//tables << col->name().lower() + "-"  + col->parentClassInfo()->name() + "-" + idFieldName( col );
				tables << col->name().lower() + "-"  + col->parentClassInfo()->name().lower() + "-" + idFieldName( col ).lower();
			}
		}

		// Take off the colon and space
		exec = exec.left( exec.length() - 2 );
		exec += ");";
		m_db->exec( exec );
		if ( m_db->lastError().type() != QSqlError::None ) {
			kdDebug() << k_funcinfo << exec << endl;
			kdDebug() << k_funcinfo << m_db->lastError().text()  << endl;
		}
	}

	// Creem les taules de relacions
	QStringList list;
	for ( i = 0; i < tables.count(); ++i ) {
		list = QStringList::split( QString( "-" ), tables[ i ] );
		kdDebug() << "Creant taula... " << list[0] << endl;
   		exec = "CREATE TABLE " + list[ 0 ] + " ( " + list[ 1 ] + " BIGINT NOT NULL REFERENCES " + list[ 1 ] + " DEFERRABLE INITIALLY DEFERRED, "+ list[ 2 ] + " BIGINT NOT NULL REFERENCES " + list[2] +" DEFERRABLE INITIALLY DEFERRED, PRIMARY KEY( "+ list[1] +" , " + list[2] + " ) );";

		m_db->exec( exec );
		if ( m_db->lastError().type() != QSqlError::None ) {
			kdDebug() << k_funcinfo << exec << endl;
			kdDebug() << k_funcinfo << m_db->lastError().text()  << endl;
		}
	}

	for ( i = 0; i < constraints.count(); ++i ) {
		list = QStringList::split( QString( "-" ), constraints[ i ] );
		exec = "ALTER TABLE " + list[ 0 ] + " ADD FOREIGN KEY (" + list[ 1 ] + ") REFERENCES " + list[ 2 ] + "( dboid ) DEFERRABLE INITIALLY DEFERRED";
		m_db->exec( exec );
		if ( m_db->lastError().type() != QSqlError::None ) {
			kdDebug() << k_funcinfo << exec << endl;
			kdDebug() << k_funcinfo << m_db->lastError().text()  << endl;
		}
	}
	return true;
}

bool InMemorySqlDbBackend::hasChanged( Object* /*object*/ )
{
	return false;
}

bool InMemorySqlDbBackend::hasChanged( Collection */*collection*/ )
{
	return false;
}

bool InMemorySqlDbBackend::hasChanged( const OidType& /*oid*/, const RelatedObject* /*related*/ )
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


/*!
Commits the current transaction
*/
bool InMemorySqlDbBackend::commit()
{
	m_db->transaction();
	m_db->exec( "SET CONSTRAINTS ALL DEFERRED" );
	if ( m_db->lastError().type() != QSqlError::None ) {
		//kdDebug() << k_funcinfo << exec << endl;
		kdDebug() << k_funcinfo << m_db->lastError().text()  << endl;
	}

	// Remove all existing information
	ClassInfoIterator m_it( Classes::begin() );
	ClassInfoIterator m_end( Classes::end() );
	for ( ; m_it != m_end; ++m_it ) {
		m_db->exec( "DELETE FROM " + (*m_it)->name() );
		if ( m_db->lastError().type() != QSqlError::None ) {
			//kdDebug() << k_funcinfo << exec << endl;
			kdDebug() << k_funcinfo << m_db->lastError().text()  << endl;
		}

		RelatedCollectionsIterator m_cIt( (*m_it)->collectionsBegin() );
		RelatedCollectionsIterator m_cEnd( (*m_it)->collectionsEnd() );
		for ( ; m_cIt != m_cEnd; ++m_cIt ) {
			if ( ! (*m_cIt)->isNToOne() )
				m_db->exec( "DELETE FROM " + (*m_cIt)->name() );
			if ( m_db->lastError().type() != QSqlError::None ) {
				//kdDebug() << k_funcinfo << exec << endl;
				kdDebug() << k_funcinfo << m_db->lastError().text()  << endl;
			}
		}
	}

	m_savedCollections.clear();
	ManagerObjectIterator it( m_manager->begin() );
	ManagerObjectIterator end( m_manager->end() );
	for ( ; it != end; ++it ) {
		saveObject( it.data().object() );
	}
	return m_db->commit();
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
			kdDebug() << "Unclassified Type: " << QVariant::typeToName( type ) << endl;
			assert( false );
			return "INTEGER";
	}
}

/*!
Aborts the current transaction
*/
void InMemorySqlDbBackend::afterRollback()
{
	init();
}

bool InMemorySqlDbBackend::load( Collection */*collection*/, const QString& /*query*/ )
{
	return true;
}
