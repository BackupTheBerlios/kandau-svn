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
#ifndef SQLDBBACKEND_H
#define SQLDBBACKEND_H

#include <qvaluevector.h>

#include "dbbackendiface.h"
#include "oidtype.h"
#include "seqtype.h"

class QSqlCursor;
class QSqlDatabase;
class CollectionInfo;
class ClassInfo;
class PropertyInfo;

/**
@author Albert Cervera Areny
*/

#define ERROR( msg ) { kdDebug() << k_funcinfo << ": " << msg << endl; return false; }

/*!
	This class implements a backend for accessing/storing objects in a SQL database. It uses the database access classes provided by Qt.
*/
class SqlDbBackend : public DbBackendIface
{
public:
	/*!
	All four isolation levels considered in SQL Standard. Though PostgreSQL, implements only ReadCommited and Serialiable.
	*/
	enum IsolationLevel {
		ReadUncommited,
		ReadCommited,
		RepeatableRead,
		Serializable
	};

	SqlDbBackend( QSqlDatabase *db, IsolationLevel isolationLevel = ReadCommited );
	virtual ~SqlDbBackend();

	QSqlDatabase *database();

	void setIsolationLevel( IsolationLevel isolationLevel );
	IsolationLevel isolationLevel() const;

	void setup( Manager* m_manager );
	void shutdown();
	bool load( const OidType& oid, Object *object );
	bool load( Collection *collection );
	bool load( Collection *collection, const QString& query );
	bool load( OidType* relatedOid, const OidType& oid, const RelationInfo* related );
	bool hasChanged( Object * object );
	bool hasChanged( Collection *collection );
	bool hasChanged( const OidType& oid, const RelationInfo* related );

	bool createSchema();
	bool commit();
	OidType newOid();
	void reset() {};

	void setOidFieldName( const QString& name );
	const QString& oidFieldName() const;

	void setSequenceFieldName( const QString& name );
	const QString& sequenceFieldName() const;

	void setPreloadCollections( bool preload );
	bool preloadCollections() const;

	// Callbacks
	void afterRollback();
	void beforeRemove( Object* object );

protected:
	virtual const ClassInfo* findObject( const OidType& oid, const ClassInfo *classInfo, QSqlCursor& cursor );
	virtual bool remove( Object *object );
	virtual bool remove( Collection *collection, const OidType& oid );
	virtual bool save( Object *object );
	virtual bool save( const OidType& oid, const RelationInfo* relationInfo, const OidType& relatedOid );
	virtual bool save( Collection *collection );
	virtual bool load( const QSqlCursor &cursor, Object *object );

	void commitObjects();
	void commitRelations();
	void commitCollections();

	QString expandDotsString( const QString& string );

	SeqType newSeq();

	QString filterFieldName( const CollectionInfo *collection ) const;
	OidType filterValue( const Collection *collection ) const;
	QString idFieldName( const CollectionInfo *collection ) const;
	QString sqlType( const PropertyInfo *info );
	QSqlDatabase *m_db;

	// Mantains the list of removed objects (classname,oid) since last commit/rollback
	QValueVector<QPair<QString,OidType> > m_removedObjects;
	// The first OidType (key) stores the Object that references the second OidType
	QMap<OidType, QMap<QString,OidType> > m_removedRelations;

private:
	QString m_oidFieldName;
	QString m_sequenceFieldName;
	bool m_preloadCollections;

	Manager *m_manager;

	IsolationLevel m_isolationLevel;

};

#endif
