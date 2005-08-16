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
#ifndef MANAGER_H
#define MANAGER_H

/**
@author Albert Cervera Areny
*/

#include <qmap.h>
#include "defs.h"
#include "oidtype.h"


class Object;
class Collection;
class DbBackendIface;
class RelatedCollection;
class ClassInfo;

typedef QMap<OidType, Object*> ManagerObjectMap;
typedef QMapIterator<OidType, Object*> ManagerObjectIterator;
typedef QMapConstIterator<OidType, Object*> ManagerObjectConstIterator;

typedef QMap<OidType, QMap<QString, QPair<OidType, bool> > > ManagerRelatedObjectMap;
typedef QMapIterator<OidType, QMap<QString, QPair<OidType, bool> > > ManagerRelatedObjectIterator;
typedef QMapConstIterator<OidType, QMap<QString, QPair<OidType, bool> > > ManagerRelatedObjectConstIterator;

typedef QMap<OidType, QMap<QString, Collection*> > ManagerRelatedCollectionMap;
typedef QMapIterator<OidType, QMap<QString, Collection*> > ManagerRelatedCollectionIterator;
typedef QMapConstIterator<OidType, QMap<QString, Collection*> > ManagerRelatedCollectionConstIterator;

#define MaxObjects 100

class Manager
{
public:
	Manager( DbBackendIface *backend );
	~Manager();

	static Manager* self();

	/* Functions related to object cache management */
	void setMaxObjects( Q_ULLONG max );
	Q_ULLONG maxObjects() const;
	uint count() const;
	uint countObjectRelations() const;
	uint countCollectionRelations() const;

	/*!
	This function prints with kdDebug() some information about the current status of the Manager. It is used for debugging purposes.
	Shown information includes:
		* Number of objects kept in memory 
		* Number of references to objects kept in memory
		* Number of references to collections kept in memory
	*/
	void status() const;

	/* Functions related to object management */
	bool add( Object* object );
	bool remove( Object* object );
	bool contains( OidType oid );
	Object* object( OidType oid );

	/*!
	Returns a pointer to the requested object.
	@param oid The oid of the object
	@param f The pointer to the function that creates an object of the type of the expected object
	*/
	Object* load( OidType oid, CreateObjectFunction f );

	/* Functions related to collection management */
	bool load( Collection* collection );

	// This function has to disappear some day. When Collection becomes an appropiate
	// class hierarchy
	bool load( Collection* collection, const QString& query );

	ManagerObjectIterator begin();
	ManagerObjectIterator end();

	/* Relation management functions */
	void setRelation( const OidType& oid, ClassInfo* classInfo, const QString& relation, const OidType& oidRelated, bool recursive = true );
	void addRelation( const OidType& oid, RelatedCollection* collection, const OidType& oidRelated, bool recursive = true );
	void removeRelation( const OidType& oid, RelatedCollection* collection, const OidType& oidRelated, bool recursive = true );
	
	void setModifiedRelation( const OidType& oid, ClassInfo* classInfo, const QString& relationName, bool modified, bool recursive = true );
	void setModifiedRelation( const OidType& oid, RelatedCollection* collection, const OidType& oidRelated, bool modified, bool recursive = true );


	OidType relation( const OidType& oid, const QString& relation );
	OidType relation( const Object* object, const QString& relation );

	Collection* collection( const Object* object, const QString& relation );
	Collection* collection( const OidType& oid, RelatedCollection* collection );

	/*!
	Commits the current transaction
	*/
	bool commit();

	/*!
	Aborts the current transaction
	*/
	bool rollback();

	/*!
	*/
	bool createSchema();

	/*!
	For testing purposes only. Used by the tests to ensure no objects remain from the previous test.
	*/
	void reset();

	/*!
	Copies the contents (objects & relations) to another manager. Only objects and relations that are currently in memory are copied currently. This means it only will copy the whole data in those src managers that have MaxObjects Unlimited. So it is possible to copy the whole Xml database to an Sql Database but not viceversa.
	@param manager The manager to which the contents of the current manager will be copied.
	*/
	void copy( Manager* manager );
	
	static const Q_ULLONG Unlimited = ULONG_MAX;
protected:
	
	/*!
	Ensures the total number of objects stays under maxObjects() as long as there
	are unmodified objects. Right now the parameter isn't used anywhere as we call this
	function before adding the object to the map. We'll see if it's necessary somewhere
	in the future, we could remove the parameter if isn't useful.
	@param object if provided, ensures that object won't be freed (ie. it has just been loaded)
	*/
	void ensureUnderMaxObjects( Object *object = 0 );
	void ensureUnderMaxRelations();
	void ensureUnderMaxCollections();

	/*!
	Sets objects and relations as unmodified. Used by commit once the backend has returned true from its commit function.
	*/
	void setEverythingUnmodified();

	enum Filter {
		Modified,
		Unmodified,
	};
	
	/*!
	This function is called inside ensureUnderMaxObjects, whenever an object is decided that is no longer necessary in memory. Then, all references the object has and which have not been modified are freed. It is also used by rollback.
	@param oid the iterator of the object which references are to be freed
	@param filter the filter to apply and thus the references that will be removed
	*/
	void Manager::removeObjectReferences( QMap<QString, QPair<OidType, bool> > map, Filter filter );

	/*!
	This function is called inside ensureUnderMaxObjects, whenever an object is decided that is no longer necessary in memory. Then, all references the object has and which have not been modified are freed. It is also used by rollback.
	@param oid the iterator of the object which references are to be freed
	@param filter the filter to apply and thus the references that will be removed
	*/
	void Manager::removeCollectionReferences( QMap<QString, Collection*> map, Filter filter );
	
	/*!
	This function acts as the previous function but also removes the oid from the map if it is empty when the appropiate references are removed
	@param oid the oid of the object which references are to be freed
	@param filter the filter to apply and thus the references that will be removed
	*/
	void removeObjectReferences( const OidType& oid, Filter filter );

public:
	ManagerRelatedObjectMap& relations();
	ManagerRelatedCollectionMap& collections();

private:
	/*!
	This QMap contains all existing objects and thus all have a valid Oid
	assigned
	*/
	ManagerObjectMap m_objects;

	/*!
	This is the pointer to the appropiate backend that will do the real persistance
	*/
	DbBackendIface *m_dbBackend;

	/*!
	A pointer to the current manager object
	*/
	static Manager* m_self;

	/*!
	Contains the maximum number of objects that should be loaded.
	Take into account that it includes modified and unmodified objects.
	*/
	uint m_maxObjects;

	/*!
	Mantains the relation between objects
	*/
	ManagerRelatedObjectMap m_relations;
	
	/*!
	Mantains the collections of objects
	*/
	ManagerRelatedCollectionMap m_collections;
};


#endif
