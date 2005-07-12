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
#ifndef SQLDBBACKEND_H
#define SQLDBBACKEND_H

#include "dbbackendiface.h"
#include "oidtype.h"
#include "seqtype.h"

class QSqlCursor;
class QSqlDatabase;
class RelatedCollection;

/**
@author Albert Cervera Areny
*/

#define ERROR( msg ) { kdDebug() << msg << endl; return false; }

/*!
	This class implements a backend for accessing/storing objects in a SQL database. It uses the database access classes provided by Qt.
*/
class SqlDbBackend : public DbBackendIface
{
public:
	SqlDbBackend( QSqlDatabase *db );
	virtual ~SqlDbBackend();

	QSqlDatabase *database();

	void setup();
	void shutdown();

	/*!
	This function loads an object from the database from the oid of the
	object and a pointer to an Object.
	*/
	virtual bool load( const OidType& oid, Object *object );

//	virtual bool load( Collection *collection , QString criteria = "");

	virtual bool load( Collection *collection );

	/*!
	This function saves an object to the database using a pointer to an Object.
	*/
	virtual bool save( Object *object );

	virtual bool save( Collection *collection );


	/*!
	This function deletes an object from the database from a pointer to the Object
	*/
	virtual bool remove( Object *object );

	/*!
	This function inserts an object to a collection
	(Heredada de SQLObjectManagerIface)
	*/
	virtual bool add( Collection *collection, Object *object );

	/*!
	This function removes an object form a collection
	(Heredada de SQLObjectManagerIface)
	*/
	virtual bool remove( Collection *collection, const OidType& oid );

	/*!
	Creates and executes all CREATE TABLE / SEQUENCE.. necessari for creating
	the SQL Database.
	(Heredada de SQLObjectManagerIface)
	*/
	virtual bool createSchema();

	void setRelation( const OidType& oid, const QString& relation, const OidType& oidRelated, const OidType& oldOid );

	/*! Decides whether the object changed in the database since last load */
	virtual bool hasChanged( Object * object );

	/*!
	Starts a transaction
	 */
	bool start();

	/*!
	Commits the current transaction
	 */
	bool commit();

	/*!
	Aborts the current transaction
	 */
	bool rollback();

	virtual OidType newOid();

	virtual SqlDbBackend* SqlBackend( );

	virtual void reset() {};
protected:
	virtual bool load( const QSqlCursor &cursor, Object *object );

	virtual void commitCollections();

	virtual SeqType newSeq();

	virtual QString filterFieldName( RelatedCollection *collection ) const;
	virtual OidType filterValue( Collection *collection ) const;
	virtual QString idFieldName( RelatedCollection *collection ) const;
	virtual QString sqlType( QVariant::Type type );
	QSqlDatabase *m_db;

	// The first OidType (key) stores the Object that references the second OidType
	QMap<OidType, QMap<QString,OidType> > m_removedRelations;
};


#endif
