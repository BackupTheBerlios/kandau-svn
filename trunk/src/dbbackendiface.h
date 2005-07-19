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
	/*!
	Called at the Manager constructor
	*/
	virtual void setup() = 0;

	/*!
	Called at the Manager destructor
	*/
	virtual void shutdown() = 0;

	/*!
	Loads the object from the database
	@param oid Oid of the object to load
	@param object A pointer to the object where the data has to be loaded to
	*/
	virtual bool load( const OidType& oid, Object *object ) = 0;

	/*!
	Loads a collection from the database
	@param collection Pointer to the collection where the data has to be loaded to
	*/
	virtual bool load( Collection *collection ) = 0;

	/*!
	Loads a collection from the database
	@param collection Pointer to the collection where the data has to be loaded to
	@param query Name of the class type of the objects you want to load
	*/
	virtual bool load( Collection *collection, const QString& query ) = 0;

	/*!
	Creates the database schema, such as creating tables and sequences in a SQL backend or a DTD in a XML backend.
	@return false if the schema could not be created, true otherwise.
	*/
	virtual bool createSchema() = 0;

	/*!
	Decides whether the object changed in the database since last load
	@param object Pointer to the object we want to check
	@return true if the object has changed (or has been removed), false otherwise
	*/
	virtual bool hasChanged( Object * object ) = 0;

	/*!
	Commits the current transaction
	@return true if the commit worked correctly, false otherwise
	 */
	virtual bool commit() = 0;

	/*!
	This function must provide a new unique Oid. Used for newly created
	objects.
	@return a new and unique Oid
	*/
	virtual OidType newOid() = 0;

	/*!
	This function is for testing purposes only right now. It expects the backend will restart it's oid assignment counter.
	*/
	virtual void reset() = 0;

	/* Callbacks */

	/*!
	Callback called before removing an object. Necessary for those backends that don't want all the objects to be kept in memory. This allows them to keep a list with the removed objects.
	@param object Pointer to the object to be deleted
	*/
	virtual void beforeRemove( Object *object ) = 0;

	/*!
	Callback called after doing the rollback. Necessary for those backends that don't want all the objects to be kept in memory. This allows them to clear the list of objects to be removed. Also, those that keep every thing in memory, can reload data from disk.
	*/
	virtual void afterRollback() = 0;
};

#endif
