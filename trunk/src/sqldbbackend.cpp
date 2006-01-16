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

void SqlDbBackend::setup( Manager* manager )
{
	m_manager = manager;
}

void SqlDbBackend::shutdown()
{
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

bool SqlDbBackend::load( OidType* relatedOid, const OidType& oid, const RelatedObject* related )
{
	if ( oid == 0 ) {
		*relatedOid = 0;
		return false;
	}
	QSqlCursor cursor( related->parentClassInfo()->name() );
	cursor.select( "to_number( dboid, '9999999999G0') = " + oidToString( oid ) );
	if ( ! cursor.next() ) {
		*relatedOid = 0;
		return false;
	}
	if ( ! cursor.contains( related->name() ) ) {
		*relatedOid = 0;
		return false;
	}
	*relatedOid = variantToOid( cursor.value( related->name() ) );
	return true;
}

bool SqlDbBackend::load( Collection *collection )
{
	assert( collection );

	collection->clear();

	QSqlCursor cursor( collection->collectionInfo()->name() );
	cursor.select( filterFieldName( collection->collectionInfo() ) + " = " + QString::number( filterValue( collection ) ) );

	OidType val;
	while ( cursor.next() ) {
		if ( collection->collectionInfo()->isNToOne() ) {
			// Optimització: en cas de tractar-se d'una relació N - 1 com que la taula
			// que tenim oberta és la dels objectes a carregar podem utilitzar el mateix
			// cursor i passar-lo com a paràmetre
			//load( *cursor, obj );
			val = variantToOid( cursor.value( idFieldName( collection->collectionInfo() ) ) );
			//load( val, obj );
			collection->simpleAdd( val );
		} else {
			val = variantToOid( cursor.value( idFieldName( collection->collectionInfo() ) ) );
			//load( val, obj );
			collection->simpleAdd( val );
		}
	}
	// TODO: Look if it is necessary a function like this:
	//collection->setLoaded( true );
	collection->setModified( false );
	return true;
}

bool SqlDbBackend::save( Collection *collection )
{
	assert( collection );
	m_db->exec( "DELETE FROM " + collection->collectionInfo()->name() + " WHERE " + collection->collectionInfo()->parentClassInfo()->name() + " = " + oidToString( collection->parentOid() ) );

	QSqlCursor cursor( collection->collectionInfo()->name() );
	QSqlRecord *record;

	CollectionIterator it( collection->begin() );
	CollectionIterator end( collection->end() );
	for ( ; it != end; ++it ) {
		record = cursor.primeInsert();
		record->setValue( collection->collectionInfo()->parentClassInfo()->name(), collection->parentOid() );
		record->setValue( collection->collectionInfo()->childrenClassInfo()->name(), (*it)->oid() );
		record->setValue( "dbseq", newSeq() );
		cursor.insert();
	}
	return true;
}

bool SqlDbBackend::save( Object *object )
{
	assert( object );
	bool update;
	QSqlRecord *record;
	QSqlCursor cursor( object->classInfo()->name() );

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
	}

	ObjectsIterator oIt( object->objectsBegin() );
	ObjectsIterator oEnd( object->objectsEnd() );
	Object *obj;
	for ( ; oIt != oEnd; ++oIt ) {
		obj = (*oIt);
		if ( ! cursor.contains( oIt.key() ) )
			continue;
		if ( obj )
			record->setValue( oIt.key(), obj->oid() );
		else
			record->setNull( oIt.key() );
	}
	
	for ( uint i = 0; i < record->count(); ++i ) {
		if ( record->fieldName( i ).right( 1 ) == "_" && variantToOid( record->value( i ) ) == 0 ) {
			record->setNull( i );
		}
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

QString SqlDbBackend::filterFieldName( const RelatedCollection *collection ) const
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

QString SqlDbBackend::idFieldName( const RelatedCollection *collection ) const
{
	assert( collection );
	if ( collection->isNToOne() )
		return "dboid";
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

	// Create the tables
	ClassInfoIterator it ( Classes::begin() );
	ClassInfoIterator end ( Classes::end() );
	ClassInfo *currentClass;
	for ( ; it != end; ++it ) {
		currentClass = *it;

		exec = "CREATE TABLE " +  currentClass->name().lower() + " ( dboid BIGINT PRIMARY KEY, dbseq BIGINT NOT NULL, ";

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
				tables << col->name().lower() + "-"  + filterFieldName( col ).lower() + "-" + idFieldName( col ).lower();
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
   		exec = "CREATE TABLE " + list[ 0 ] + " ( " + list[ 1 ] + " BIGINT NOT NULL REFERENCES " + list[ 1 ] + " DEFERRABLE INITIALLY DEFERRED, "+ list[ 2 ] + " BIGINT NOT NULL REFERENCES " + list[2] + " DEFERRABLE INITIALLY DEFERRED, dbseq BIGINT NOT NULL , PRIMARY KEY( "+ list[1] +" , " + list[2] + " ) );";

		m_db->exec( exec );
		if ( m_db->lastError().type() != QSqlError::None ) {
			kdDebug() << k_funcinfo << " -> " << exec << endl;
			kdDebug() << k_funcinfo << " -> " << m_db->lastError().text() << endl;
		}
	}

	for ( i = 0; i < constraints.count(); ++i ) {
		list = QStringList::split( QString( "-" ), constraints[ i ] );
		exec = "ALTER TABLE " + list[ 0 ] + " ADD FOREIGN KEY (" + list[ 1 ] + ") REFERENCES " + list[ 2 ] + "( dboid ) DEFERRABLE INITIALLY DEFERRED";
		
		m_db->exec( exec );
		if ( m_db->lastError().type() != QSqlError::None ) {
			kdDebug() << k_funcinfo << " -> " << exec << endl;
			kdDebug() << k_funcinfo << " -> " << m_db->lastError().text() << endl;
		}
	}
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
			kdDebug() << "Unclassified Type: " << QVariant::typeToName( type ) << endl;
			assert( false );
			return "INTEGER";
	}
}

bool SqlDbBackend::hasChanged( Object * object )
{
	QSqlSelectCursor cursor( "SELECT dbseq FROM " + object->classInfo()->name() + " WHERE to_number( dboid, '9999999999G0') = " + oidToString( object->oid() ) );

	// Has it been deleted?
	if ( ! cursor.next() )
		return false;

	return variantToSeq( cursor.value( 0 ) ) != object->seq();
}

bool SqlDbBackend::hasChanged( Collection *collection )
{
	/* Right now we always consider we need to reload the collection */
	return false;
}

bool SqlDbBackend::hasChanged( const OidType& oid, const RelatedObject* related )
{
	/* Right now we always consider we need to reload the oid */
	return false;
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
		m_db->exec( "DELETE FROM " + (*rit).first + " WHERE dboid=" + oidToString( (*rit).second ) );
	}

	commitCollections();
	
	ManagerObjectIterator it( m_manager->begin() );
	ManagerObjectIterator end( m_manager->end() );
	Object *obj;
	for ( ; it != end; ++it ) {
		obj = it.data().object();
		if ( obj->isModified() ) {
			save( obj );
		}
	}

/*
	QMap<OidType, QMap<QString, QPair<OidType, bool> > > &relations = m_manager->relations();
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
*/

	if ( m_db->commit() ) {
		m_removedObjects.clear();
		return true;
	} else {
		return false;
	}
}

void SqlDbBackend::commitCollections()
{
	QMap<OidType, QMap<QString, Collection*> > m_collections;

	ManagerRelatedCollectionIterator cit( m_manager->collections().begin() ) ;
	ManagerRelatedCollectionIterator cend( m_manager->collections().end() );
	Collection *c;
	for ( ; cit != cend; ++cit ) {
		c = cit.data().collection();
		if ( c->modified() ) {
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

		curClass + "." + element + " = " + relatedClassName + ".dboid AND " + relatedClassName;
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
	
	Tokenizer tokenizer( string, "." );
	className = tokenizer.nextToken();
	relationName = tokenizer.nextToken();
	do {
		if ( ! Classes::contains( className ) ) {
			kdDebug() << k_funcinfo << "Class '" << className << "' not found." << endl;
			return string;
		}
		ClassInfo *classInfo = Classes::classInfo( className );
		if ( classInfo->containsObject( relationName ) ) {
			RelatedObject *rel = classInfo->object( relationName );
			if ( rel->isOneToOne() ) {
				expanded += className + "." + relationName + "=";
				expanded += rel->relatedClassInfo()->name() + ".dboid AND ";
				className=rel->relatedClassInfo()->name();
			} else {
				expanded += className + "." + relationName + "=";
				expanded += rel->relatedClassInfo()->name() + ".dboid AND ";
				className=rel->relatedClassInfo()->name();
			}
		} else if ( classInfo->containsCollection( relationName ) ) {
			RelatedCollection *rel = classInfo->collection( relationName );
			if ( rel->isNToOne() ) {
				expanded += className + ".dboid=";
				expanded += rel->childrenClassInfo()->name() + "." + relationName + " AND ";
				className=rel->childrenClassInfo()->name();
			} else {
				expanded += className + ".dboid=";
				expanded += relationName + "." + className + " AND ";
				expanded += relationName + "." + rel->childrenClassInfo()->name() + "=";
				expanded += rel->childrenClassInfo()->name() + ".dboid AND ";
				className = rel->childrenClassInfo()->name();
			}
		} else {
			kdDebug() << k_funcinfo << "Class '" << className << "' doesn't contain any relation named '" << relationName << "'" << endl;
			expanded += className + "." + tokenizer.tail();
			break;
		}
		relationName = tokenizer.nextToken();	
	} while ( ! relationName.isNull() );
	expanded += className + "." + tokenizer.tail() + " ";
	//expanded += "." + tokenizer.tail();
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
		Tokenizer tokenizer( query, seps );
		QString token;
		int lastIndex = 0;	
		while ( ( token = tokenizer.nextToken() ) != QString::null ) {
			if ( token.contains( "." ) && ! token.contains( "*" ) ) {
				newQuery += expandDotsString( token );
			} else {
				newQuery += query.mid( lastIndex, tokenizer.currentIndex() - lastIndex );
			}
			lastIndex = tokenizer.currentIndex();
		}
		newQuery += tokenizer.tail();
	}
	kdDebug() << k_funcinfo << "FINAL QUERY: " << newQuery << endl;
	collection->clear();
	QSqlSelectCursor cursor( newQuery );
	cursor.select();
	while ( cursor.next() ) {
		collection->simpleAdd( variantToOid( cursor.value( "dboid" ) ) );
	}
	return true;
}
