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
#ifndef INMEMORYSQLDBBACKEND_H
#define INMEMORYSQLDBBACKEND_H

#include <dbbackendiface.h>
#include "oidtype.h"
#include "seqtype.h"
#include "object.h"

class QSqlDatabase;
class QSqlCursor;

/**
@author Albert Cervera Areny
*/
class InMemorySqlDbBackend : public DbBackendIface
{
public:
	InMemorySqlDbBackend( QSqlDatabase* db );
	virtual ~InMemorySqlDbBackend();

	void loadObject( const QSqlCursor& cursor, ObjectRef<Object> object );
	void saveObject( ObjectRef<Object> object );

	QString idFieldName( RelatedCollection *collection ) const;

	/* Called at the Manager constructor */
	virtual void setup();
	virtual void shutdown();

	/* Object management related functions */
	virtual bool load( const OidType& oid, Object *object );
	virtual bool save( Object *object );
	virtual bool remove( Object *object );

	/* Collection management related functions */
	virtual bool load( Collection *collection );
	virtual bool add( Collection* collection, Object* object );
	virtual bool remove( Collection* collection, const OidType& oid );

	/* Database Schema related functions */
	virtual bool createSchema();

	void setRelation( const OidType& oid, const QString& relation, const OidType& oidRelated, const OidType& oldOid );

	/*! Decides whether the object changed in the database since last load */
	virtual bool hasChanged( Object * object );

	/*!
	This function must provide a new unique Oid. Used for newly created
	objects.
	*/
	virtual OidType newOid();

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

	void reset() {};

	QString sqlType( QVariant::Type type );
private:
	QSqlDatabase *m_db;
	OidType m_currentOid;
	QStringList m_savedCollections;
};

#endif
