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

SqlDbBackend::SqlDbBackend( QSqlDatabase *db )
{
	assert( db );
	m_db = db;
}

SqlDbBackend::~SqlDbBackend()
{
}

QSqlDatabase* SqlDbBackend::database( )
{
	return m_db;
}

void SqlDbBackend::setup()
{
	start();
}

void SqlDbBackend::shutdown()
{
	assert( m_db );
//	m_db->exec( "ROLLBACK;" );
}

bool SqlDbBackend::load( const OidType& oid, Object *object )
{
	assert( object );
	if ( oid == 0 )
		ERROR( "Oid cannot be zero" );
	object->setOid( 0 );
	QSqlCursor cursor( object->classInfo()->name() );
	cursor.select( "to_number( dboid, '9999999999G0') = " + oidToString( oid ) );
	if ( ! cursor.next() )
		ERROR( "Oid not found" );
	return load( cursor, object );
}

bool SqlDbBackend::load( const QSqlCursor &cursor, Object *object )
{
	assert( object );
	Property prop;
	object->setOid( variantToOid( cursor.value( "dboid" ) ) );
	object->setSeq( variantToSeq( cursor.value( "dbseq" ) ) );

	PropertyIterator pIt( object->propertiesBegin() );
	PropertyIterator pEnd( object->propertiesEnd() );
	for ( ; pIt != pEnd; ++pIt ) {
		prop = pIt.data();

		if ( ! cursor.contains( prop.name() ) )
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
	ObjectIterator oIt( object->objectsBegin() );
	ObjectIterator oEnd( object->objectsEnd() );
	for (; oIt != oEnd; ++oIt ) {
		if ( ! cursor.contains( oIt.key() ) )
			continue;
		object->setObject( oIt.key(), variantToOid( cursor.value( oIt.key() ) ) );
	}

	return true;
}
/*
bool SqlDbBackend::save( Collection *collection )
{
	assert( collection );

}
*/
bool SqlDbBackend::save( Object *object )
{
	assert( object );
	bool update;
	QSqlRecord *record;
	QSqlCursor cursor( object->classInfo()->name() );

	kdDebug() << k_funcinfo << endl;

	if ( object->oid() == 0 ) {
		// Crea un oid únic
		object->setOid( newOid() );
		cursor.select();
		record = cursor.primeInsert();
		update = false;
	} else {
		cursor.select( "to_number( dboid, '9999999999G0') = " + oidToString( object->oid() ) );
		if ( ! cursor.next() ) {
			//if ( ! force )
			//	ERROR( "Oid diferent of zero but not found" );
			cursor.select();
			record = cursor.primeInsert();
			update = false;
		} else {
			record = cursor.primeUpdate();
			update = true;
		}
	}

	record->setValue( "dboid", object->oid() );
	record->setValue( "dbseq", newSeq() );

	PropertyIterator pIt( object->propertiesBegin() );
	PropertyIterator pEnd( object->propertiesEnd() );
	Property prop;
	for ( ; pIt != pEnd; ++pIt ) {
		prop = pIt.data();
		if ( ! record->contains( prop.name() ) )
			continue;

		if ( prop.type() == QVariant::Pixmap ) {
			QByteArray img;
			QBuffer buffer( img );
			buffer.open( IO_WriteOnly );
			prop.value().toPixmap().save( &buffer, "PNG" );
			record->setValue( prop.name(), img );
		} else if( prop.type() == QVariant::List ){
			QByteArray pin;
			QDataStream stream( pin,IO_WriteOnly  );
			stream << prop.value();
			record->setValue( prop.name(), stream );
		} else {
			record->setValue( prop.name(), prop.value() );
		}
	}

	// Fill the fields for relations with other objects
	ObjectIterator oIt( object->objectsBegin() );
	ObjectIterator oEnd( object->objectsEnd() );
	Object *obj;
	for ( ; oIt != oEnd; ++oIt ) {
		obj = (*oIt);
		if ( ! cursor.contains( oIt.key() ) )
			continue;
		if ( obj )
			record->setValue( oIt.key(), obj->oid() );
		else
			record->setNull( oIt.key() );

		/*
		if ( ! obj->isNull() )
			record->setValue( tableName( obj ), obj->oid() );
		else
			record->setNull( tableName( obj ) );
		*/
	}

	for ( uint i = 0; i < record->count(); ++i ) {
		if ( record->fieldName( i ).right( 1 ) == "_" && variantToOid( record->value( i ) ) == 0 ) {
			record->setNull( i );
		}
	}

	if ( update ) {
		if (! cursor.update() ) {
			kdDebug() << "EOOOOOO -> " << cursor.lastError().text() << endl;
			kdDebug() << "EOOOOOO -> " << cursor.executedQuery() << endl;
			ERROR( "Update failed" );
		}
	} else {
		if ( ! cursor.insert() ) {
			kdDebug() << "EOOOOOO -> " << cursor.lastError().text() << endl;
			kdDebug() << "EOOOOOO -> " << cursor.executedQuery() << endl;
			ERROR( "Insert failed" );
		}
	}
	return true;
}

bool SqlDbBackend::remove( Object *object )
{
	assert( object );
	if ( object->oid() == 0 )
		ERROR( "Oid = 0" );

	QSqlQuery query;
	query.prepare( "DELETE FROM " + object->classInfo()->name().lower() + " WHERE to_number( dboid, '9999999999G0') = " + oidToString( object->oid() ) );
	query.exec();

	if ( query.numRowsAffected() > 0 ) {
		// Deixem l'objecte a null
		object->setOid( 0 );
		return true;
	} else
		return false;
}

// QString SqlDbBackend::tableName( const QString &name ) const
// {
// 	QString tmp = name;
// 	return tmp.replace( "-", "_" ) + QString( "_" );
// }
//
// QString SqlDbBackend::tableName( const Object *object ) const
// {
// 	assert( object );
// 	return tableName( object->classInfo()->name() );
// }

/*!
Generates a new Oid from the database sequence
*/
OidType SqlDbBackend::newOid()
{
	//QSqlSelectCursor cursor( "SELECT nextval('seq_dboid') AS Oid" );
	//m_db->exec( "ROLLBACK;" );
	QSqlQuery query = m_db->exec( "SELECT nextval('seq_dboid');" );
//	kdDebug() << k_funcinfo << m_db->lastError().text()  << endl;
//	if ( ! query.isValid() )
//		kdDebug() << k_funcinfo << "Query not valid" << endl;
	if ( ! query.next() )
		ERROR( "Could not read next value from the sequence" );
	kdDebug() << k_funcinfo << "OID: " << query.value( 0 ) << endl;
	return variantToOid( query.value( 0 ) );
}

SeqType SqlDbBackend::newSeq()
{
	//QSqlSelectCursor cursor( "SELECT nextval('seq_dbseq') AS Seq" );
	kdDebug() << k_funcinfo << m_db->lastError().text()  << endl;
	QSqlQuery query = m_db->exec( "SELECT nextval('seq_dbseq')" );
	kdDebug() << k_funcinfo << m_db->lastError().text()  << endl;
	if ( ! query.next() )
		ERROR( "Could not read next value from the sequence" );

	return variantToOid( query.value( 0 ) );
}

//bool SqlDbBackend::load( Collection *collection, QString criteria  )
bool SqlDbBackend::load( Collection *collection )
{
	assert( collection );
	QString criteria;

	collection->clear();

	QSqlCursor *cursor;

	cursor = new QSqlCursor( collection->collectionInfo()->name() );
	cursor->select( filterFieldName( collection->collectionInfo() ) + " = " + QString::number( filterValue( collection ) ) );

	int i = 0;
	OidType val;
	while ( cursor->next() ) {
		i++;
		if ( collection->collectionInfo()->isNToOne() ) {
			// Optimització: en cas de tractar-se de una relació N - 1 com que la taula
			// que tenim oberta és la dels objectes a carregar podem utilitzar el mateix
			// cursor i passar-lo com a paràmetre
			//load( *cursor, obj );
			val = variantToOid( cursor->value( idFieldName( collection->collectionInfo() ) ) );
			//load( val, obj );
			collection->simpleAdd( val );
		} else {
			val = variantToOid( cursor->value( idFieldName( collection->collectionInfo() ) ) );
			//load( val, obj );
			collection->simpleAdd( val );
		}
	}
	// TODO: Look if it is necessary a function like this:
	//collection->setLoaded( true );
	delete cursor;
	return true;
}

// QString SqlDbBackend::tableName( RelatedCollection *collection ) const
// {
// 	assert( collection );
//
// /*
// 	assert( collection );
// 	if ( collection->isNToOne() )
// 		return tableName( collection->childrenClassInfo()->name() );
//
// 	QStringList names ( collection->parentClassInfo()->name() );
// 	names << collection->childrenClassInfo()->name();
// 	names.sort();
// 	return "rel_" + names.join( "_" );
// */
// }

QString SqlDbBackend::filterFieldName( RelatedCollection *collection ) const
{
	assert( collection );
	return collection->parentClassInfo()->name();
}

OidType SqlDbBackend::filterValue( Collection *collection ) const
{
	assert( collection );
	if ( ! collection->parent() ) {
		kdDebug() << "The collection doesn't have a parent" << endl;
		return 0;
	}
	// TODO: Veure com podem cridar ERROR sense haver de treure el const
	// ERROR( "The collection doesn't have a parent" );

	if ( ! collection->parent()->inherits( "Object" ) ) {
		kdDebug() << "The parent doesn't inherit from Object" << endl;
		return 0;
	}

	return static_cast<Object*>(collection->parent())->oid();
}

QString SqlDbBackend::idFieldName( RelatedCollection *collection ) const
{
	assert( collection );
	if ( collection->isNToOne() )
		return "dboid";
	else
		return collection->childrenClassInfo()->name();
}

bool SqlDbBackend::add( Collection *collection, Object *object )
{
	assert( collection);
	assert( object );
	if ( object->oid() == 0 )
		ERROR( "Oid = 0" );
	if ( collection->contains( object->oid() ) )
		ERROR( "Oid alread exists in collection" );
	if ( collection->collectionInfo()->childrenClassInfo()->name() != object->className() )
		ERROR( "The object's class and the collection's object class don't match" );

	// TODO: Traspassar a ObjectManagerIface
	/*
	if ( ! collection->loaded() ) {
		if ( ! load( collection ) ) {
			kdDebug() << "Error loading collection" << endl;
		}
	}
	*/

	QSqlCursor cursor( collection->collectionInfo()->name() );

	QSqlRecord *record;
	if ( collection->collectionInfo()->isNToOne() ) {
		cursor.select( idFieldName( collection->collectionInfo() ) + " = " + oidToString( object->oid() ) );
		cursor.next();
		record = cursor.primeUpdate();
		record->setValue( filterFieldName( collection->collectionInfo() ), filterValue( collection ) );
		record->setValue( "dbseq", newSeq() );
		if ( cursor.update() <= 0 )
			return false;
	} else {
		cursor.select();
		record = cursor.primeInsert();
		record->setValue( filterFieldName( collection->collectionInfo() ), filterValue( collection ) );
		record->setValue( idFieldName( collection->collectionInfo() ), object->oid() );
		record->setValue( "dbseq", newSeq() );
		if ( cursor.insert() <= 0 )
			return false;
	}
	return true;
}

bool SqlDbBackend::remove( Collection *collection, const OidType& oid )
{
	assert( collection );
	if ( oid == 0 )
		ERROR( "Oid = 0" );
	if ( ! collection->contains( oid ) )
		ERROR( "Oid doesn't exist in collection" );

	// TODO: Traspassar a ObjectManagerIface
	/*
	if ( ! collection->loaded() ) {
		if ( ! load( collection ) ) {
			kdDebug() << "Error loading collection" << endl;
		}
	}
	*/

	// TODO: Comprovar si no és preferible fer-ho mitjançant Cursors
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
	Object *object;
	Property prop;
	uint i;

	//m_db->exec( "ROLLBACK;" );

	// This sequence is incremented every time a new object is created
	m_db->exec( "CREATE SEQUENCE seq_dboid;" );

	// This sequence is incremented every time a record is created or modified and is used in the dbseq field that will be created in each table.
	m_db->exec( "CREATE SEQUENCE seq_dbseq;" );

	// Create the tables
	ClassInfoIterator it ( Classes::begin() );
	ClassInfoIterator end ( Classes::end() );
	ClassInfo *currentClass;
	for ( ; it != end; ++it ) {
		currentClass = *it;
		// We need to create an object because it's the only way to iterate through a class' properties right now
		object = currentClass->createInstance();
		exec = "CREATE TABLE " +  currentClass->name().lower() + " ( dboid BIGINT PRIMARY KEY, dbseq BIGINT NOT NULL, ";

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
				tables << col->name().lower() + "-"  + filterFieldName( col ).lower() + "-" + idFieldName( col ).lower();
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
   		exec = "CREATE TABLE " + list[ 0 ] + " ( " + list[ 1 ] + " BIGINT NOT NULL REFERENCES " + list[ 1 ] + ", "+ list[ 2 ] + " BIGINT NOT NULL REFERENCES " + list[2] +", dbseq BIGINT NOT NULL , PRIMARY KEY( "+ list[1] +" , " + list[2] + " ) );";

		kdDebug() << exec << endl;
		m_db->exec( exec );
		kdDebug() << m_db->lastError().text() << endl;
	}

	for ( i = 0; i < constraints.count(); ++i ) {
		list = QStringList::split( QString( "-" ), constraints[ i ] );
		exec = "ALTER TABLE " + list[ 0 ] + " ADD FOREIGN KEY (" + list[ 1 ] + ") REFERENCES " + list[ 2 ] + "( dboid )";
		kdDebug() << exec << endl;
		m_db->exec( exec );
	}

	start();
	return true;
}

QString SqlDbBackend::sqlType( QVariant::Type type )
{
	// TODO: Repassar els tipus enters sense signe. El mapejat
	// de tipus és incorrecte
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

SqlDbBackend* SqlDbBackend::SqlBackend( )
{
	return this;
}

bool SqlDbBackend::hasChanged( Object * object )
{
	QSqlSelectCursor cursor( "SELECT dbseq FROM " + object->classInfo()->name() + " WHERE to_number( dboid, '9999999999G0') = " + oidToString( object->oid() ) );

	// Is it been deleted?
	if ( ! cursor.next() )
		return false;

	return variantToSeq( cursor.value( 0 ) ) != object->seq();
}

void SqlDbBackend::setRelation( const OidType& /*oid*/, const QString& /*relation*/, const OidType& /*oidRelated*/, const OidType& /*oldOid*/ )
{

}

/*!
Starts a transaction
*/
bool SqlDbBackend::start()
{
	//m_db->exec( "BEGIN;" );
	//m_db->exec( "SET CONSTRAINTS DEFERRED;" );
	return true;
}

/*!
Commits the current transaction
*/
bool SqlDbBackend::commit()
{
	kdDebug() << k_funcinfo << endl;
	//m_db->exec( "BEGIN;" );
	//m_db->
	m_db->transaction();
	m_db->exec( "SET CONSTRAINTS ALL DEFERRED;" );
	QMapIterator<OidType, Object*> it( Manager::self()->begin() );
	QMapIterator<OidType, Object*> end( Manager::self()->end() );
	Object *obj;
	for ( ; it != end; ++it ) {
		obj = (*it);
		if ( obj->isModified() ) {
			save( obj );
			obj->setModified( false );
		}
	}

	QMap<OidType, QMap<QString, QPair<OidType, bool> > > &relations = Manager::self()->relations();
	QMapIterator<OidType, QMap<QString, QPair<OidType, bool> > > mit( relations.begin() );
	QMapIterator<OidType, QMap<QString, QPair<OidType, bool> > > mend( relations.end() );
	for ( ; mit != mend; ++mit ) {
		QMapIterator<QString, QPair<OidType, bool> > mmit( (*mit).begin() );
		QMapIterator<QString, QPair<OidType, bool> > mmend( (*mit).end() );
		for ( ; mmit != mmend; ++mmit ) {
			if ( (*mmit).second ) {

			}
		}
	}

	commitCollections();

	m_db->exec( "COMMIT;" );

	return true;
}

void SqlDbBackend::commitCollections()
{
	QMap<OidType, QMap<QString, Collection*> > m_collections;

	QMapIterator<OidType, QMap<QString, Collection*> > cit( Manager::self()->collections().begin() ) ;
	QMapIterator<OidType, QMap<QString, Collection*> > cend( Manager::self()->collections().end() );
	Collection *c;
	for ( ; cit != cend; ++cit ) {
		QMapIterator<QString, Collection*> it( (*cit).begin() );
		QMapIterator<QString, Collection*> end( (*cit).end() );
		for ( ; it != end; ++it ) {
			c = *it;
			if ( c->modified() ) {
//				save( c );
			}
		}
	}
}


/*!
Aborts the current transaction
*/
bool SqlDbBackend::rollback()
{
	return true;
}
