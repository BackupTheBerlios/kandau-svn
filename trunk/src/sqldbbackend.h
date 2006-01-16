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
class RelatedCollection;

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
	SqlDbBackend( QSqlDatabase *db );
	virtual ~SqlDbBackend();

	QSqlDatabase *database();

	void setup( Manager* m_manager );
	void shutdown();
	bool load( const OidType& oid, Object *object );
	bool load( Collection *collection );
	bool load( Collection *collection, const QString& query );
	bool load( OidType* relatedOid, const OidType& oid, const RelatedObject* related );
	bool hasChanged( Object * object );
	bool hasChanged( Collection *collection );
	bool hasChanged( const OidType& oid, const RelatedObject* related );

	bool createSchema();
	bool commit();
	OidType newOid();
	void reset() {};

	// Callbacks
	void afterRollback();
	void beforeRemove( Object* object );

protected:
	bool remove( Object *object );
	bool remove( Collection *collection, const OidType& oid );
	bool save( Object *object );
	bool save( Collection *collection );
	bool load( const QSqlCursor &cursor, Object *object );
	void commitCollections();
	QString expandDotsString( const QString& string );

	SeqType newSeq();

	QString filterFieldName( const RelatedCollection *collection ) const;
	OidType filterValue( const Collection *collection ) const;
	QString idFieldName( const RelatedCollection *collection ) const;
	QString sqlType( QVariant::Type type );
	QSqlDatabase *m_db;

	// Mantains the list of removed objects (classname,oid) since last commit/rollback
	QValueVector<QPair<QString,OidType> > m_removedObjects;
	// The first OidType (key) stores the Object that references the second OidType
	QMap<OidType, QMap<QString,OidType> > m_removedRelations;
	
	Manager *m_manager;
};

#endif
