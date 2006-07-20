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
#include <assert.h>

#include <qsqldatabase.h>
#include <qsqlcursor.h>
#include <qsqlselectcursor.h>
#include <qsqlrecord.h>
#include <qsqlquery.h>
#include <qbuffer.h>
#include <qpixmap.h>
#include <qdatetime.h>

#include <kdebug.h>

#include "sqldbbackend.h"
#include "object.h"
#include "tokenizer.h"

SqlDbBackend::SqlDbBackend( QSqlDatabase *db, IsolationLevel isolationLevel )
{
	assert( db );
	m_db = db;
	setIsolationLevel( isolationLevel );
	m_oidFieldName = "dboid";
	m_sequenceFieldName = "dbseq";
	m_preloadCollections = true;
}

SqlDbBackend::~SqlDbBackend()
{
}

void SqlDbBackend::setIsolationLevel( IsolationLevel isolationLevel )
{
	m_isolationLevel = isolationLevel;
	switch ( m_isolationLevel ) {
		case ReadUncommited:
			m_db->exec( "SET SESSION CHARACTERISTICS AS TRANSACTION ISOLATION LEVEL READ UNCOMMITED" );
			break;
		case ReadCommited:
			m_db->exec( "SET SESSION CHARACTERISTICS AS TRANSACTION ISOLATION LEVEL READ COMMITED" );
			break;
		case RepeatableRead:
			m_db->exec( "SET SESSION CHARACTERISTICS AS TRANSACTION ISOLATION LEVEL REPEATABLE READ" );
			break;
		case Serializable:
			m_db->exec( "SET SESSION CHARACTERISTICS AS TRANSACTION ISOLATION LEVEL SERIALIZABLE" );
			break;
	}
}

SqlDbBackend::IsolationLevel SqlDbBackend::isolationLevel() const
{
	return m_isolationLevel;
}

QSqlDatabase* SqlDbBackend::database( )
{
	return m_db;
}

void SqlDbBackend::setup( Manager* manager )
{
	m_manager = manager;
}

void SqlDbBackend::shutdown()
{
}

const ClassInfo* SqlDbBackend::findObject( const OidType& oid, const ClassInfo *classInfo, QSqlCursor& cursor )
{
	const ChildrenInfo& children = classInfo->children();
	const ClassInfo* info;
	ChildrenInfoConstIterator it( children.constBegin() );
	ChildrenInfoConstIterator end( children.constEnd() );
	for ( ; it != end; ++it ) {
		cursor.setName( (*it)->name() );
		cursor.select( "to_number( " + oidFieldName() + ", '9999999999G0') = " + oidToString( oid ) );
		if ( cursor.next() )
			return *it;

		info = findObject( oid, *it, cursor );
		if ( info )
			return info;
	}
	return 0;
}

bool SqlDbBackend::load( const OidType& oid, Object *object )
{
	assert( object );
	if ( oid == 0 )
		ERROR( "Oid cannot be zero" );
	object->setOid( 0 );
	QSqlCursor cursor( object->classInfo()->name() );
	cursor.select( "to_number( " + oidFieldName() + ", '9999999999G0') = " + oidToString( oid ) );

	if ( ! cursor.next() ) {
		const ClassInfo* info = findObject( oid, object->classInfo(), cursor );
		if ( ! info )
			ERROR( "Oid not found" );
		// delete object;
		// object = info->createInstance();
	}

	return load( cursor, object );
}

bool SqlDbBackend::load( const QSqlCursor &cursor, Object *object )
{
	assert( object );
	Property prop;
	object->setOid( variantToOid( cursor.value( oidFieldName() ) ) );
	object->setSeq( variantToSeq( cursor.value( sequenceFieldName() ) ) );

	PropertiesIterator pIt( object->propertiesBegin() );
	PropertiesIterator pEnd( object->propertiesEnd() );
	for ( ; pIt != pEnd; ++pIt ) {
		prop = pIt.data();

		if ( prop.readOnly() || ! cursor.contains( prop.name() ) )
			continue;

		switch ( pIt.data().type() ) {
			case QVariant::Pixmap:
				object->setProperty( pIt.data().name(), QPixmap( cursor.value( pIt.data().name() ).toByteArray() ) );
				break;
			case QVariant::DateTime:
				object->setProperty( pIt.data().name(), cursor.value( pIt.data().name() ).toDateTime() );
				break;
			case QVariant::Date:
				object->setProperty( pIt.data().name(), cursor.value( pIt.data().name() ).toDate() );
				break;
			case QVariant::Time:
				object->setProperty( pIt.data().name(), cursor.value( pIt.data().name() ).toTime() );
				break;
			case QVariant::ByteArray:
				object->setProperty( pIt.data().name(), cursor.value( pIt.data().name() ).toByteArray() );
				break;
			default:
				object->setProperty( pIt.data().name(), cursor.value( pIt.data().name() ) );
		}
	}

	// Prepare for relation loading
	/*
	ObjectsIterator oIt( object->objectsBegin() );
	ObjectsIterator oEnd( object->objectsEnd() );
	for (; oIt != oEnd; ++oIt ) {
		if ( ! cursor.contains( oIt.key() ) )
			continue;
		object->setObject( oIt.key(), variantToOid( cursor.value( oIt.key() ) ) );
	}
	*/
	object->setModified( false );
	return true;
}

bool SqlDbBackend::load( OidType* relatedOid, const OidType& oid, const RelationInfo* relationInfo )
{
	QString searchTable;
	QString searchField;
	QString resultField;

	if ( oid == 0 ) {
		*relatedOid = 0;
		return false;
	}

	if ( relationInfo->isOneToOne() ) {
		QString related = relationInfo->relatedClassInfo()->name();
		QString parent = relationInfo->parentClassInfo()->name();
		if ( related >= parent ) {
			searchTable = parent;
			searchField = oidFieldName();
			resultField = relationInfo->name();
		} else {
			searchTable = related;
			searchField = relationInfo->name();
			resultField = oidFieldName();
		}
	} else {
		//cursor = new QSqlCursor( relationInfo->parentClassInfo()->name() );
		searchTable = relationInfo->parentClassInfo()->name();
		searchField = oidFieldName();
		resultField = relationInfo->name();
	}

	QSqlCursor cursor( searchTable );
	cursor.select( "to_number( " + searchField + ", '9999999999G0') = " + oidToString( oid ) );
	if ( ! cursor.next() ) {
		*relatedOid = 0;
		return false;
	}
	if ( ! cursor.contains( resultField ) ) {
		*relatedOid = 0;
		return false;
	}
	*relatedOid = variantToOid( cursor.value( resultField ) );

/*
	QSqlCursor cursor( relationInfo->parentClassInfo()->name() );
	cursor.select( "to_number( " + oidFieldName() + ", '9999999999G0') = " + oidToString( oid ) );
	if ( ! cursor.next() ) {
		*relatedOid = 0;
		return false;
	}
	if ( ! cursor.contains( relationInfo->name() ) ) {
		*relatedOid = 0;
		return false;
	}
	*relatedOid = variantToOid( cursor.value( relationInfo->name() ) );
*/
	return true;
}

bool SqlDbBackend::load( Collection *collection )
{
	assert( collection );
	QString searchTable;
	QString searchField;
	QString resultField;

	collection->simpleClear();

	if ( collection->collectionInfo()->isNToOne() ) {
		searchTable = collection->collectionInfo()->parentClassInfo()->name();
		searchField = oidFieldName();
		resultField = collection->collectionInfo()->name();
	} else {
		searchTable = collection->collectionInfo()->name();
		searchField = collection->collectionInfo()->parentClassInfo()->name();
		resultField = collection->collectionInfo()->childrenClassInfo()->name();
	}

	QSqlCursor cursor( searchTable );
	cursor.select( searchField + " = " + oidToString( collection->parentOid() ) );
	if ( ! cursor.contains( resultField ) ) {
		collection->setModified( false );
		return false;
	}
	while ( cursor.next() )
		collection->simpleAdd( variantToOid( cursor.value( resultField ) ) );

	collection->setModified( false );
	return true;
}

bool SqlDbBackend::save( Object *object )
{
	assert( object );
	bool update;
	QSqlRecord *record;
	QSqlCursor cursor( object->classInfo()->name() );

	if ( object->oid() == 0 ) {
		object->setOid( newOid() );
		cursor.select();
		record = cursor.primeInsert();
		update = false;
	} else {
		cursor.select( "to_number( " + oidFieldName() + ", '9999999999G0') = " + oidToString( object->oid() ) );
		if ( ! cursor.next() ) {
			cursor.select();
			record = cursor.primeInsert();
			update = false;
		} else {
			record = cursor.primeUpdate();
			update = true;
		}
	}

	// We don't mark any field as generated. So at first, none
	// would be included in the INSERT/UPDATE. Then we must make sure
	// we set the generated flag to each property field.
	// Note that this is necesary as we want relation fields to take their
	// default values when inserted.
	for ( uint i = 0; i < record->count(); ++i ) {
		record->setGenerated( i, false );
	}

	record->setValue( oidFieldName(), object->oid() );
	record->setGenerated( oidFieldName(), true );
	record->setValue( sequenceFieldName(), newSeq() );
	record->setGenerated( sequenceFieldName(), true );

	PropertiesIterator pIt( object->propertiesBegin() );
	PropertiesIterator pEnd( object->propertiesEnd() );
	Property prop;
	for ( ; pIt != pEnd; ++pIt ) {
		prop = pIt.data();
		if ( prop.readOnly() || ! record->contains( prop.name() ) )
			continue;

		if ( prop.type() == QVariant::Pixmap ) {
			QByteArray img;
			QBuffer buffer( img );
			buffer.open( IO_WriteOnly );
			prop.value().toPixmap().save( &buffer, "PNG" );
			record->setValue( prop.name(), img );
//		} else if( prop.type() == QVariant::List ){
//			QByteArray pin;
//			QDataStream stream( pin, IO_ReadWrite );
//			stream << prop.value();
//			record->setValue( prop.name(), stream );
		} else {
			record->setValue( prop.name(), prop.value() );
		}
		record->setGenerated( prop.name(), true );
	}

	if ( update ) {
		if (! cursor.update() ) {
			kdDebug() << k_funcinfo << " -> " << cursor.lastError().text() << endl;
			kdDebug() << k_funcinfo << " -> " << cursor.executedQuery() << endl;
			ERROR( "Update failed" );
		}
	} else {
		if ( ! cursor.insert() ) {
			kdDebug() << k_funcinfo << " -> " << cursor.lastError().text() << endl;
			kdDebug() << k_funcinfo << " -> " << cursor.executedQuery() << endl;
			ERROR( "Insert failed" );
		}
	}
	return true;
}

bool SqlDbBackend::save( const OidType& oid, const RelationInfo* relationInfo, const OidType& relatedOid )
{
	QString searchTable;
	OidType searchOid;
	OidType setOid;
	bool update;
	QSqlRecord *record;

	if ( relationInfo->isOneToOne() ) {
		QString related = relationInfo->relatedClassInfo()->name();
		QString parent = relationInfo->parentClassInfo()->name();
		if ( related >= parent ) {
			searchTable = parent;
			searchOid = oid;
			setOid = relatedOid;
		} else {
			searchTable = related;
			searchOid = relatedOid;
			setOid = oid;
		}
	} else {
		searchTable = relationInfo->parentClassInfo()->name();
		searchOid = oid;
		setOid = relatedOid;
	}

	// We shouldn't store anything of any object with oid zero
	if ( searchOid == 0 )
		return true;

	kdDebug() << "Search table: " << searchTable << endl;
	QSqlCursor cursor( searchTable );
	//TODO: Probably the object should always already exist as commitObjects() is
	// called before commitRelations(). So maybe if record isn't found should be
	// considered an error.
	if ( oid == 0 ) {
		// Creates a unique oid. TODO: Should it ever happen?
		cursor.select();
		record = cursor.primeInsert();
		update = false;
	} else {
		cursor.select( "to_number( " + oidFieldName() + ", '9999999999G0') = " + oidToString( searchOid ) );
		if ( ! cursor.next() ) {
			cursor.select();
			record = cursor.primeInsert();
			update = false;
		} else {
			record = cursor.primeUpdate();
			update = true;
		}
	}

	// We don't mark any field as generated. So at first, none
	// would be included in the INSERT/UPDATE. Then we must make sure
	// we set the generated flag to each property field.
	// Note that this is necesary as we want relation fields to take their
	// default values when inserted.
	for ( uint i = 0; i < record->count(); ++i ) {
		record->setGenerated( i, false );
	}

	record->setValue( oidFieldName(), searchOid );
	record->setGenerated( oidFieldName(), true );
	record->setValue( sequenceFieldName(), newSeq() );
	record->setGenerated( sequenceFieldName(), true );

	if ( setOid != 0 )
		record->setValue( relationInfo->name(), setOid );
	else
		record->setNull( relationInfo->name() );
	record->setGenerated( relationInfo->name(), true );

	if ( update ) {
		if (! cursor.update() ) {
			kdDebug() << k_funcinfo << " -> " << cursor.lastError().text() << endl;
			kdDebug() << k_funcinfo << " -> " << cursor.executedQuery() << endl;
			//delete cursor;
			ERROR( "Update failed" );
		}
	} else {
		if ( ! cursor.insert() ) {
			kdDebug() << k_funcinfo << " -> " << cursor.lastError().text() << endl;
			kdDebug() << k_funcinfo << " -> " << cursor.executedQuery() << endl;
			//delete cursor;
			ERROR( "Insert failed" );
		}
	}
	//delete cursor;
	return true;
}

bool SqlDbBackend::save( Collection *collection )
{
	assert( collection );
	m_db->exec( "DELETE FROM " + collection->collectionInfo()->name() + " WHERE " + collection->collectionInfo()->parentClassInfo()->name() + " = " + oidToString( collection->parentOid() ) );

	if ( collection->collectionInfo()->name() == "Customer_CustomerOrder" ) {
		kdDebug() << "Found" << endl;
	}
	QSqlCursor cursor( collection->collectionInfo()->name() );
	QSqlRecord *record;

	CollectionIterator it( collection->begin() );
	CollectionIterator end( collection->end() );
	for ( ; it != end; ++it ) {
		record = cursor.primeInsert();
		record->setValue( collection->collectionInfo()->parentClassInfo()->name(), collection->parentOid() );
		record->setValue( collection->collectionInfo()->childrenClassInfo()->name(), (*it)->oid() );
		record->setValue( sequenceFieldName(), newSeq() );
		cursor.insert();
	}
	return true;
}


bool SqlDbBackend::remove( Object *object )
{
	assert( object );
	if ( object->oid() == 0 )
		ERROR( "Oid = 0" );

	QSqlQuery query;
	query.prepare( "DELETE FROM " + object->classInfo()->name().lower() + " WHERE to_number( " + oidFieldName() + ", '9999999999G0') = " + oidToString( object->oid() ) );
	query.exec();

	if ( query.numRowsAffected() > 0 ) {
		// Deixem l'objecte a null
		object->setOid( 0 );
		return true;
	} else
		return false;
}

OidType SqlDbBackend::newOid()
{
	QSqlQuery query = m_db->exec( "SELECT nextval('seq_dboid');" );
	if ( ! query.next() )
		ERROR( "Could not read next value from the sequence" );
	return variantToOid( query.value( 0 ) );
}

SeqType SqlDbBackend::newSeq()
{
	QSqlQuery query = m_db->exec( "SELECT nextval('seq_dbseq')" );
	if ( ! query.next() )
		ERROR( "Could not read next value from the sequence" );
	return variantToOid( query.value( 0 ) );
}

QString SqlDbBackend::filterFieldName( const CollectionInfo *collection ) const
{
	assert( collection );
	return collection->parentClassInfo()->name();
}

OidType SqlDbBackend::filterValue( const Collection *collection ) const
{
	assert( collection );
	if ( ! collection->parent() ) {
		kdDebug() << k_funcinfo << " -> " << "The collection doesn't have a parent" << endl;
		return 0;
	}
	if ( ! collection->parent()->inherits( "Object" ) ) {
		kdDebug() << k_funcinfo << " -> " << "The parent doesn't inherit from Object" << endl;
		return 0;
	}
	return static_cast<Object*>(collection->parent())->oid();
}

QString SqlDbBackend::idFieldName( const CollectionInfo *collection ) const
{
	assert( collection );
	if ( collection->isNToOne() )
		return oidFieldName();
	else
		return collection->childrenClassInfo()->name();
}

bool SqlDbBackend::remove( Collection *collection, const OidType& oid )
{
	assert( collection );
	if ( oid == 0 )
		ERROR( "Oid = 0" );
	if ( ! collection->contains( oid ) )
		ERROR( "Oid doesn't exist in collection" );

	QSqlQuery query;
	if ( ! collection->collectionInfo()->isNToOne() ) {
		query.prepare( "DELETE FROM " + collection->collectionInfo()->name().lower() + " WHERE to_number( " + filterFieldName( collection->collectionInfo() ) + ",'9999999999G0') = " + QString::number( filterValue( collection ) ) + " AND to_number(" + idFieldName( collection->collectionInfo() ) + ",'9999999999G0') = " + oidToString( oid ) );
		query.exec();
	} else {
		query.prepare( "UPDATE " + collection->collectionInfo()->name().lower() + " SET " + filterFieldName( collection->collectionInfo() ) + " = NULL WHERE to_number(" + idFieldName( collection->collectionInfo() ) + ",'9999999999G0') = " + oidToString( oid ) );
		query.exec();
	}

	if ( query.numRowsAffected() <= 0 )
		return false;

	return true;
}

bool SqlDbBackend::createSchema()
{
	QStringList tables;
	QStringList constraints;
	QString exec;
	PropertyInfo *prop;
	uint i;

	// This sequence is incremented every time a new object is created
	m_db->exec( "CREATE SEQUENCE seq_dboid;" );

	// This sequence is incremented every time a record is created or modified and is used in the dbseq field that will be created in each table.
	m_db->exec( "CREATE SEQUENCE seq_dbseq;" );

	// Create the tables. Iterate creating the classes that
	// have inheritance first.
	QStringList classList( Classes::parentsFirst() );
	QStringList::const_iterator it( classList.constBegin() );
	QStringList::const_iterator end( classList.constEnd() );
	ClassInfo *currentClass;
	for ( ; it != end; ++it ) {
		currentClass = Classes::classInfo( *it );


		exec = "CREATE TABLE " +  currentClass->name().lower() + " ( " + oidFieldName() + " BIGINT PRIMARY KEY, " + sequenceFieldName() + " BIGINT NOT NULL, ";

		// Create properties fields
		PropertiesInfoConstIterator pIt( currentClass->propertiesBegin() );
		PropertiesInfoConstIterator pEnd( currentClass->propertiesEnd() );
		for ( ; pIt != pEnd; ++pIt ) {
			prop = *pIt;
			if ( prop->readOnly() == false )
				exec += prop->name() + " " + sqlType( prop ) + ", ";
		}

		// Create related objects fields
		// For 1-1 relations only create the field in one of the two tables.
		// We assume that both classes have relation to each other.
		RelationInfosConstIterator oIt( currentClass->relationsBegin() );
		RelationInfosConstIterator oEnd( currentClass->relationsEnd() );
		RelationInfo *rObj;
		for ( ; oIt != oEnd; ++oIt ) {
			rObj = *oIt;
			// needs to be >= to consider cases where the parent and related class(table) are the same
			if ( ! rObj->isOneToOne() || rObj->relatedClassInfo()->name() >= rObj->parentClassInfo()->name() ) {
				exec += rObj->name().lower() + " BIGINT DEFAULT NULL, ";
				constraints << currentClass->name() + "-" + rObj->name() + "-" + rObj->relatedClassInfo()->name();
			}
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
				if ( rCol->childrenClassInfo()->name() == currentClass->name() && rCol->isNToOne() && constraints.grep( rCol->name() ).count() == 0 ) {
					exec += rCol->name().lower() + " BIGINT DEFAULT NULL, ";
					constraints << currentClass->name() + "-" + rCol->name() + "-" + rCol->parentClassInfo()->name();
				}
			}
		}

		CollectionInfosIterator colIt( currentClass->collectionsBegin() );
		CollectionInfosIterator colEnd( currentClass->collectionsEnd() );
		CollectionInfo *col;
		for ( ; colIt != colEnd; ++colIt ) {
			col = *colIt;
			if ( ! tables.grep( col->name() ).count() > 0 && ! col->isNToOne() ) {
				tables << col->name() + "-"  + filterFieldName( col ) + "-" + idFieldName( col );
			}
		}

		// Take off the colon and space
		exec = exec.left( exec.length() - 2 );
		exec += ")";
		if ( currentClass->parent() != 0 )
			exec += " INHERITS ( " + currentClass->parent()->name() + ")";

		m_db->exec( exec );

		if ( m_db->lastError().type() != QSqlError::None ) {
			kdDebug() << k_funcinfo << exec << endl;
			kdDebug() << k_funcinfo << m_db->lastError().text()  << endl;
		}
	}

	/*
	As PostgreSQL doesn't properly support foreign keys to inherited tables we will create
	foreign keys (references) only when the refered class hasn't any inherited classes.
	*/

	// Create relation tables (for N-M relations)
	QStringList list;
	for ( i = 0; i < tables.count(); ++i ) {
		list = QStringList::split( QString( "-" ), tables[ i ] );
		exec = "CREATE TABLE " + list[ 0 ].lower() + " ( " + list[ 1 ].lower() + " BIGINT NOT NULL ";
		if ( Classes::classInfo( list[ 1 ] )->children().count() == 0 )
			exec += " REFERENCES " + list[ 1 ].lower() + " DEFERRABLE INITIALLY DEFERRED";
		exec += ", " + list[ 2 ].lower() + " BIGINT NOT NULL ";
		if ( Classes::classInfo( list[2] )->children().count() == 0 )
			exec += " REFERENCES " + list[ 2 ].lower() + " DEFERRABLE INITIALLY DEFERRED";
		exec += ", " + sequenceFieldName() + " BIGINT NOT NULL , PRIMARY KEY( " + list[1].lower() + " , " + list[2].lower() + " ) );";

		m_db->exec( exec );
		if ( m_db->lastError().type() != QSqlError::None ) {
			kdDebug() << k_funcinfo << " -> " << exec << endl;
			kdDebug() << k_funcinfo << " -> " << m_db->lastError().text() << endl;
		}
	}

	// Create foreign keys in class tables
	for ( i = 0; i < constraints.count(); ++i ) {
		list = QStringList::split( QString( "-" ), constraints[ i ] );
		// If the related class  doesn't have children we can use a normal
		// foreign key in PostgreSQL. Otherwise we have to use our own trigger.
		kdDebug() << k_funcinfo << "'" << list[ 2 ] << "'" << endl;
		if ( Classes::classInfo( list[ 2 ] )->children().count() == 0 )
			exec = "ALTER TABLE " + list[ 0 ].lower() + " ADD FOREIGN KEY (" + list[ 1 ].lower() + ") REFERENCES " + list[ 2 ].lower() + "( " + oidFieldName() + " ) DEFERRABLE INITIALLY DEFERRED";
		//else
		//	exec = "CREATE DDL!!!";
		m_db->exec( exec );
		if ( m_db->lastError().type() != QSqlError::None ) {
			kdDebug() << k_funcinfo << " -> " << exec << endl;
			kdDebug() << k_funcinfo << " -> " << m_db->lastError().text() << endl;
		}
	}
	return true;
}

QString SqlDbBackend::sqlType( const PropertyInfo* info )
{
	// TODO: Take a look at the unsigned integer types. The mapping isn't correct.
	if ( info->isEnumType() )
		return "INTEGER";

	switch ( info->type() ) {
		case QVariant::CString:
			return "VARCHAR";
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
			kdDebug() << "Unclassified Type: " << QVariant::typeToName( info->type() ) << endl;
			assert( false );
			return "INTEGER";
	}
}

bool SqlDbBackend::hasChanged( Object * object )
{
	QSqlSelectCursor cursor( "SELECT " + sequenceFieldName() + " FROM " + object->classInfo()->name() + " WHERE to_number( " + oidFieldName() + ", '9999999999G0') = " + oidToString( object->oid() ) );

	// Has it been deleted?
	if ( ! cursor.next() )
		return true;

	return variantToSeq( cursor.value( 0 ) ) != object->seq();
}

bool SqlDbBackend::hasChanged( Collection* /*collection*/ )
{
	/* Right now we always consider we need to reload the collection */
	return true;
}

bool SqlDbBackend::hasChanged( const OidType& /*oid*/, const RelationInfo* /*related*/ )
{
	/* Right now we always consider we need to reload the oid */
	return true;
}

/*!
Commits the current transaction
*/
bool SqlDbBackend::commit()
{
	m_db->transaction();
	m_db->exec( "SET CONSTRAINTS ALL DEFERRED" );

	QValueVector< QPair<QString, OidType> >::const_iterator rit( m_removedObjects.begin() );
	QValueVector< QPair<QString, OidType> >::const_iterator rend( m_removedObjects.end() );
	for ( ; rit != rend; ++rit ) {
		m_db->exec( "DELETE FROM " + (*rit).first + " WHERE " + oidFieldName() + "=" + oidToString( (*rit).second ) );
	}

	commitObjects();
	commitRelations();
	commitCollections();

	if ( m_db->commit() ) {
		m_removedObjects.clear();
		return true;
	} else {
		return false;
	}
}

void SqlDbBackend::commitObjects()
{
	// Store objects
	ManagerObjectIterator it( m_manager->objects().begin() );
	ManagerObjectIterator end( m_manager->objects().end() );
	Object *obj;
	for ( ; it != end; ++it ) {
		obj = it.data().object();
		if ( obj->isModified() ) {
			save( obj );
		}
	}
}

void SqlDbBackend::commitRelations()
{
	ManagerRelationIterator it( m_manager->relations().begin() );
	ManagerRelationIterator end( m_manager->relations().end() );

	for ( ; it != end; ++it ) {
		if ( it.data().isModified() ) {
			kdDebug() << "Relation Oid: " << oidToString( it.data().oid() ) << endl;
			save( it.key().oid(), it.data().relationInfo(), it.data().oid() );
		}
	}
}

void SqlDbBackend::commitCollections()
{
	ManagerCollectionIterator cit( m_manager->collections().begin() ) ;
	ManagerCollectionIterator cend( m_manager->collections().end() );
	Collection *c;
	for ( ; cit != cend; ++cit ) {
		c = cit.data().collection();
		// Only save as collection N-M relations the rest will be treated
		// by the commitRelations() function as modifing the collection will
		// modify the relation too.
		if ( c->modified() && ! c->collectionInfo()->isNToOne() ) {
			save( c );
		}
	}
}

void SqlDbBackend::afterRollback()
{
	m_removedObjects.clear();
}

void SqlDbBackend::beforeRemove( Object* object )
{
	m_removedObjects.append( QPair<QString,OidType> (object->classInfo()->name(), object->oid()) );
}
/*
QString SqlDbBackend::pickToken( const QString& msg, int i )
{
	return msg.section( QRegExp( "\\s+|<|>|=|!" ), i, i, QString::SectionSkipEmpty );
}

QString SqlDbBackend::pickElement( const QString& msg, int i )
{
	return msg.section( QRegExp( "\\." ), i, i );
}

QString SqlDbBackend::expandDots( const QString& msg )
{
	element = pickElement( token, 0 );
	if ( ! Classes::contains( element ) )
		return QString::null;

	QString curClass;

	curClass = element;
	element = pickElement( token, 1 );

	if ( Classes::classInfo( curClass )->containsObject( element ) ) {
		relatedClassName = Classes::classInfo( curClass )->object( element )->childrenClassInfo()->name();

		curClass + "." + element + " = " + relatedClassName + "." + oidFieldName() + " AND " + relatedClassName;
	} else if ( Classes::classInfo( curClass )->containsCollection( element ) ) {

	}
}
*/

QString SqlDbBackend::expandDotsString( const QString& string )
{
	QString className;
	QString relationName;
	QString expanded;

	assert( string.contains( "." ) );

	MTokenizer tokenizer( string, "." );
	className = tokenizer.nextToken();
	relationName = tokenizer.nextToken();
	do {
		if ( ! Classes::contains( className ) ) {
			kdDebug() << k_funcinfo << "Class '" << className << "' not found." << endl;
			return string;
		}
		ClassInfo *classInfo = Classes::classInfo( className );
		if ( classInfo->containsObject( relationName ) ) {
			RelationInfo *rel = classInfo->object( relationName );
			if ( rel->isOneToOne() ) {
				expanded += className + "." + relationName + "=";
				expanded += rel->relatedClassInfo()->name() + "." + oidFieldName() + " AND ";
				className=rel->relatedClassInfo()->name();
			} else {
				expanded += className + "." + relationName + "=";
				expanded += rel->relatedClassInfo()->name() + "." + oidFieldName() + " AND ";
				className=rel->relatedClassInfo()->name();
			}
		} else if ( classInfo->containsCollection( relationName ) ) {
			CollectionInfo *rel = classInfo->collection( relationName );
			if ( rel->isNToOne() ) {
				expanded += className + "." + oidFieldName() + "=";
				expanded += rel->childrenClassInfo()->name() + "." + relationName + " AND ";
				className=rel->childrenClassInfo()->name();
			} else {
				expanded += className + "." + oidFieldName() + "=";
				expanded += relationName + "." + className + " AND ";
				expanded += relationName + "." + rel->childrenClassInfo()->name() + "=";
				expanded += rel->childrenClassInfo()->name() + "." + oidFieldName() + "AND ";
				className = rel->childrenClassInfo()->name();
			}
		} else {
			kdDebug() << k_funcinfo << "Class '" << className << "' doesn't contain any relation named '" << relationName << "'" << endl;
			expanded += className + "." + tokenizer.tail();
			kdDebug() << k_funcinfo << "Expanded string: " << expanded  << endl;
			return expanded;
		}
		relationName = tokenizer.nextToken();
	} while ( ! relationName.isNull() );
	expanded += className + "." + tokenizer.tail() + " ";
	kdDebug() << k_funcinfo << "Expanded string: " << expanded  << endl;
	return expanded;
}

bool SqlDbBackend::load( Collection* collection, const QString& query )
{
	assert( collection );
	QString newQuery;

	if ( Classes::contains( query ) ) {
		newQuery = "SELECT * FROM " + query;
	} else {
		QStringList seps;
		seps << " ";
		seps << ")";
		seps << "(";
		seps << "=";
		seps << ">";
		seps << "<";
		MTokenizer tokenizer( query, seps );
		QString token;
		int lastIndex = 0;
		kdDebug() << k_funcinfo << "A punt d'embuclar: " << query << endl;
		token = tokenizer.nextToken();
		while ( ! token.isNull() ) {
			kdDebug() << k_funcinfo << "Bucle: " << token << endl;
			if ( token.contains( "." ) && ! token.contains( "*" ) ) {
				newQuery += expandDotsString( token );
			} else {
				newQuery += query.mid( lastIndex, tokenizer.currentIndex() - lastIndex );
			}
			lastIndex = tokenizer.currentIndex();
			token = tokenizer.nextToken();
		}
		newQuery += tokenizer.tail();
	}
	kdDebug() << k_funcinfo << "FINAL QUERY: " << newQuery << endl;
	QSqlSelectCursor cursor( newQuery );
	cursor.select();
	while ( cursor.next() ) {
		collection->simpleAdd( variantToOid( cursor.value( oidFieldName() ) ) );
	}
	return true;
}

void SqlDbBackend::setOidFieldName( const QString & name )
{
	m_oidFieldName = name;
}

const QString & SqlDbBackend::oidFieldName( ) const
{
	return m_oidFieldName;
}

void SqlDbBackend::setSequenceFieldName( const QString & name )
{
	m_sequenceFieldName;
}

const QString & SqlDbBackend::sequenceFieldName( ) const
{
	return m_sequenceFieldName;
}

/*!
Allows to ensure collection objects are loaded at the same time the collection is loaded, thus speeding up things.
TODO: Well, it's currently unimplemented :)
*/
void SqlDbBackend::setPreloadCollections( bool preload )
{
	m_preloadCollections = preload;
}

bool SqlDbBackend::preloadCollections( ) const
{
	return m_preloadCollections;
}
