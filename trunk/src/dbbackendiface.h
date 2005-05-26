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
#ifndef DBBACKENDIFACE_H
#define DBBACKENDIFACE_H

#include "oidtype.h"

class QString;
class Object;
class Collection;

/**
@author Albert Cervera Areny
*/

/*
TODO: Add a function to enable reading the last error. [ lastError() ]
Maybe needs to return a class or just a string...
*/
class DbBackendIface
{
public:
	/* Called at the Manager constructor */
	virtual void setup() = 0;
	/* Called at the Manager destructor */
	virtual void shutdown() = 0;

	/* Object management related functions */
	virtual bool load( const OidType& oid, Object *object ) = 0;
	virtual bool save( Object *object ) = 0;
	virtual bool remove( Object *object ) = 0;

	/* Collection management related functions */
	virtual bool load( Collection *collection ) = 0;
	virtual bool add( Collection* collection, Object* object ) = 0;
	virtual bool remove( Collection* collection, const OidType& oid ) = 0;

	/* Database Schema related functions */
	virtual bool createSchema() = 0;

	/* Object relations functions */
	virtual void setRelation( const OidType& oid, const QString& relation, const OidType& oidRelated, const OidType& oldOid ) = 0;
	
	/*! Decides whether the object changed in the database since last load */
	virtual bool hasChanged( Object * object ) = 0;

	/*!
	Starts a transaction
	 */
	virtual bool start() = 0;

	/*!
	Commits the current transaction
	 */
	virtual bool commit() = 0;

	/*!
	Aborts the current transaction
	 */
	virtual bool rollback() = 0;

	/*!
	This function must provide a new unique Oid. Used for newly created
	objects.
	*/
	virtual OidType newOid() = 0;

	/*!
	This function is for testing purposes only right now. It expects the backend will restart it's oid assignment counter.
	*/
	virtual void reset() = 0;
};

#endif
