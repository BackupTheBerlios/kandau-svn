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

#include <QSqlError>

#include <QStringList>
#include <Q3SqlCursor>

#include "inmemorysqldbbackend.h"
#include "classes.h"
#include "collection.h"
#include "object.h"

// TODO Qt3 -> Qt4 by Percy
//QString::lower() -> QString::toLower()
//QSqlError::None -> QSqlError::NoError
//QStringList::split -> QString::split

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
        if ( tables.filter( info->name(), Qt::CaseInsensitive ).count() == 0 )
			continue;

        QSqlQuery query(*m_db);
        query.exec(QString("SELECT * FROM %1").arg(info->name()));

        if ( m_db->lastError().type() != QSqlError::NoError ) {
			//kdDebug() << k_funcinfo << exec << endl;
			kdDebug() << k_funcinfo << m_db->lastError().text()  << endl;
		}

		while ( query.isActive() && query.next() ) {
			oid = variantToOid( query.record().value( "dboid" ) );

			Object* object = Classes::classInfo( info->name() )->create( oid, m_manager, true );
			assert( object );
			loadObject( query.record(), object );
			if ( oid > maxOid )
				maxOid = oid;
		}
	}
	m_currentOid = maxOid + 1;	// m_currentOid = max oid found + 1
}

void InMemorySqlDbBackend::loadObject( const QSqlRecord& cursor, Object* object )
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
	RelationInfosConstIterator oIt( object->classInfo()->relationsBegin() );
	RelationInfosConstIterator oEnd( object->classInfo()->relationsEnd() );
	for ( ; oIt != oEnd; ++oIt )
		object->setObject( (*oIt)->name(), variantToOid( cursor.value( (*oIt)->name() ) ) );

	CollectionsIterator cIt( object->collectionsBegin() );
	CollectionsIterator cEnd( object->collectionsEnd() );
	for ( ; cIt != cEnd; ++cIt ) {
		if ( (*cIt)->collectionInfo()->isNToOne() )
			continue;

        QString queryString;
        QString whereClause;
        QSqlQuery query(*m_db);

        queryString = QString("SELECT * FROM %1 ").arg( (*cIt)->collectionInfo()->name().toLower() );
        whereClause = QString("WHERE %1 = %2").arg( object->classInfo()->name(),
                                                    oidToString( object->oid() ) );

        queryString += whereClause;
        qDebug() << queryString;
        query.exec(queryString);


        if ( m_db->lastError().type() != QSqlError::NoError ) {
			kdDebug() << k_funcinfo << m_db->lastError().text()  << endl;
		}

		while ( query.isActive() && query.next() ) {
            QSqlRecord record = query.record();
			(*cIt)->simpleAdd( variantToOid( record.value( (*cIt)->collectionInfo()->childrenClassInfo()->name() ) ) );
		}
	}
}

void InMemorySqlDbBackend::saveObject( Object* object )
{
	assert( object );
	assert( object->classInfo() );

    Q3SqlCursor cursor( object->classInfo()->name().toLower() );
	cursor.select();
	//if ( m_db->lastError().type() != QSqlError::NoError ) { // TODO Qt3 -> Qt4 by Percy
    if ( m_db->lastError().type() != QSqlError::NoError ) {
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
	RelationInfosConstIterator oIt( object->classInfo()->relationsBegin() );
	RelationInfosConstIterator oEnd( object->classInfo()->relationsEnd() );
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

		// Q3SqlCursor cCursor( (*cIt)->collectionInfo()->name().toLower() ); // TODO Qt3 -> Qt4 by Percy
        Q3SqlCursor cCursor( (*cIt)->collectionInfo()->name().toLower() );
		cCursor.select();
		// if ( m_db->lastError().type() != QSqlError::NoError ) { // TODO Qt3 -> Qt4 by Percy
        if ( m_db->lastError().type() != QSqlError::NoError ) {
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

bool InMemorySqlDbBackend::load( OidType* /*relatedOid*/, const OidType& /*oid*/, const RelationInfo* /*related*/ )
{
	return true;
}


QString InMemorySqlDbBackend::idFieldName( CollectionInfo *collection ) const
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
		//exec = "CREATE TABLE " +  currentClass->name().toLower() + " ( dboid BIGINT PRIMARY KEY, "; // TODO Qt3 -> Qt4 by Percy
        exec = "CREATE TABLE " +  currentClass->name().toLower() + " ( dboid BIGINT PRIMARY KEY, ";

		// Create properties fields
		PropertiesInfoConstIterator pIt( currentClass->propertiesBegin() );
		PropertiesInfoConstIterator pEnd( currentClass->propertiesEnd() );
		for ( ; pIt != pEnd; ++pIt ) {
			prop = *pIt;
			if ( prop->readOnly() == false )
				exec += prop->name() + " " + sqlType( prop->type() ) + ", ";
		}

		// Create related objects fields
		RelationInfosConstIterator oIt( currentClass->relationsBegin() );
		RelationInfosConstIterator oEnd( currentClass->relationsEnd() );
		RelationInfo *rObj;
		for ( ; oIt != oEnd; ++oIt ) {
			rObj = *oIt;
			exec += rObj->name().toLower() + " BIGINT DEFAULT NULL, ";
			constraints << currentClass->name().toLower() + "-" + rObj->name().toLower() + "-" + rObj->relatedClassInfo()->name().toLower();
		}

		// Search in all the classes if they have N - 1 relations with the current class
		ClassInfoIterator cIt( Classes::begin() );
		ClassInfoIterator cEnd( Classes::end() );
		ClassInfo *cInfo;
		for ( ; cIt != cEnd; ++cIt ) {
			cInfo = *cIt;
			CollectionInfosIterator colIt( cInfo->collectionsBegin() );
			CollectionInfosIterator colEnd( cInfo->collectionsEnd() );
			CollectionInfo *rCol;
			for ( ; colIt != colEnd; ++colIt ) {
				rCol = *colIt;
				if ( rCol->childrenClassInfo()->name() == currentClass->name() && rCol->isNToOne() && constraints.filter( rCol->name().toLower() ).count() == 0 ) {
					exec += rCol->name().toLower() + " BIGINT DEFAULT NULL, ";
					constraints << currentClass->name().toLower() + "-" + rCol->name().toLower() + "-" + rCol->parentClassInfo()->name().toLower();
				}
			}
		}

		CollectionInfosIterator colIt( currentClass->collectionsBegin() );
		CollectionInfosIterator colEnd( currentClass->collectionsEnd() );
		CollectionInfo *col;
		for ( ; colIt != colEnd; ++colIt ) {
			col = *colIt;
			if ( ! tables.filter( col->name().toLower() ).count() > 0 && ! col->isNToOne() ) {
				//tables << col->name().toLower() + "-"  + col->parentClassInfo()->name() + "-" + idFieldName( col );
				tables << col->name().toLower() + "-"  + col->parentClassInfo()->name().toLower() + "-" + idFieldName( col ).toLower();
			}
		}

		// Take off the colon and space
		exec = exec.left( exec.length() - 2 );
		exec += ");";
		m_db->exec( exec );
		if ( m_db->lastError().type() != QSqlError::NoError ) {
			kdDebug() << k_funcinfo << exec << endl;
			kdDebug() << k_funcinfo << m_db->lastError().text()  << endl;
		}
	}

	// Creem les taules de relacions
	QStringList list;
	for ( i = 0; i < tables.count(); ++i ) {
		list = tables[ i ].split( QString( "-" ) );
   		exec = "CREATE TABLE " + list[ 0 ] + " ( " + list[ 1 ] + " BIGINT NOT NULL REFERENCES " + list[ 1 ] + " DEFERRABLE INITIALLY DEFERRED, "+ list[ 2 ] + " BIGINT NOT NULL REFERENCES " + list[2] +" DEFERRABLE INITIALLY DEFERRED, PRIMARY KEY( "+ list[1] +" , " + list[2] + " ) );";

		m_db->exec( exec );
		if ( m_db->lastError().type() != QSqlError::NoError ) {
			kdDebug() << k_funcinfo << exec << endl;
			kdDebug() << k_funcinfo << m_db->lastError().text()  << endl;
		}
	}

	for ( i = 0; i < constraints.count(); ++i ) {
		list = constraints[ i ].split( QString( "-" ) );
		exec = "ALTER TABLE " + list[ 0 ] + " ADD FOREIGN KEY (" + list[ 1 ] + ") REFERENCES " + list[ 2 ] + "( dboid ) DEFERRABLE INITIALLY DEFERRED";
		m_db->exec( exec );
		if ( m_db->lastError().type() != QSqlError::NoError ) {
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

bool InMemorySqlDbBackend::hasChanged( const OidType& /*oid*/, const RelationInfo* /*related*/ )
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
	if ( m_db->lastError().type() != QSqlError::NoError ) {
		//kdDebug() << k_funcinfo << exec << endl;
		kdDebug() << k_funcinfo << m_db->lastError().text()  << endl;
	}

	// Remove all existing information
	ClassInfoIterator m_it( Classes::begin() );
	ClassInfoIterator m_end( Classes::end() );
	for ( ; m_it != m_end; ++m_it ) {
		m_db->exec( "DELETE FROM " + (*m_it)->name() );
		if ( m_db->lastError().type() != QSqlError::NoError ) {
			//kdDebug() << k_funcinfo << exec << endl;
			kdDebug() << k_funcinfo << m_db->lastError().text()  << endl;
		}

		CollectionInfosIterator m_cIt( (*m_it)->collectionsBegin() );
		CollectionInfosIterator m_cEnd( (*m_it)->collectionsEnd() );
		for ( ; m_cIt != m_cEnd; ++m_cIt ) {
			if ( ! (*m_cIt)->isNToOne() )
				m_db->exec( "DELETE FROM " + (*m_cIt)->name() );
			if ( m_db->lastError().type() != QSqlError::NoError ) {
				//kdDebug() << k_funcinfo << exec << endl;
				kdDebug() << k_funcinfo << m_db->lastError().text()  << endl;
			}
		}
	}

	m_savedCollections.clear();
	ManagerObjectIterator it( m_manager->objects().begin() );
	ManagerObjectIterator end( m_manager->objects().end() );
	for ( ; it != end; ++it ) {
		// saveObject( it.data().object() ); // DEPRECATED Qt3 -> Qt4 by Percy
        saveObject( it.value().object() );
	}
	return m_db->commit();
}

QString InMemorySqlDbBackend::sqlType( QVariant::Type type )
{
	// TODO: Review the unsigned integer. Type mapping might be incorrect.
	switch ( type ) {
		//case QVariant::CString: // TODO DEPRECATED Qt3 -> Qt4 by Percy
        //case QVariant::ByteArray:
			//return "VARCHAR";
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
