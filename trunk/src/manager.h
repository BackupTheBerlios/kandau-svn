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

	/* Functions related to object management */
	bool add( Object* object );
	bool remove( Object* object );
	bool contains( OidType oid );
	Object* object( OidType oid );

	bool save( Object* object );
	/*!
	Returns a pointer to the requested object.
	@param oid The oid of the object
	@param f The pointer to the function that creates an object of the type of the expected object
	*/
	Object* load( OidType oid, CreateObjectFunction f );


	/* Functions related to collection management */
//	bool add( Collection* collection, Object* object );
//	bool remove( Collection* collection, const OidType& oid );
	bool load( Collection* collection );

	ManagerObjectIterator begin();
	ManagerObjectIterator end();

	/* Relation management functions */
/*
	void setRelation( const Object* object, const QString& relation, const Object* objRelated, bool recursive = true );
	void setRelation( const Object* object, const QString& relation, const OidType& objRelated, bool recursive = true );
	void addRelation( const Object *object, const QString& relation, const Object* objRelated, bool recursive );
	void removeRelation( const Object* object, const QString& relation, const Object* objRelated, bool recursive );
*/

	void setRelation( const OidType& oid, ClassInfo* classInfo, const QString& relation, const OidType& oidRelated, bool recursive = true );
	void addRelation( const OidType& oid, RelatedCollection* collection, const OidType& oidRelated, bool recursive = true );
	void removeRelation( const OidType& oid, RelatedCollection* collection, const OidType& oidRelated, bool recursive = true );



	OidType relation( const OidType& oid, const QString& relation );
	OidType relation( const Object* object, const QString& relation );

	Collection* collection( const Object* object, const QString& relation );
	Collection* collection( const OidType& oid, RelatedCollection* collection );

	/* Transaction management functions */

	/*!
	Starts a transaction
	*/
//	bool start();

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

	//const Q_ULLONG Unlimited = 9223372036854775807;
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


	enum Filter {
		Modified,
		Unmodified,
	};
	/*!
	This function is called inside ensureUnderMaxObjects, whenever an object is decided that is no longer necessary in memory. Then, all references the object has and which have not been modified are freed. It is also used by rollback.
	@param oid the iterator of the object which references are to be freed
	@param filter the filter to apply and thus the references that will be removed
	*/
	void removeObjectReferences( QMapIterator<OidType, QMap<QString, QPair<OidType, bool> > > oid, Filter filter );

	/*!
	This function is called inside ensureUnderMaxObjects, whenever an object is decided that is no longer necessary in memory. Then, all references the object has and which have not been modified are freed. It is also used by rollback.
	@param oid the iterator of the object which references are to be freed
	@param filter the filter to apply and thus the references that will be removed
	*/
	void removeCollectionReferences( QMapIterator<OidType, QMap<QString, Collection*> > oid, Filter filter );

	/*!
	This function acts as the previous function but also removes the oid from the map if it is empty when the appropiate references are removed
	@param oid the oid of the object which references are to be freed
	@param filter the filter to apply and thus the references that will be removed
	*/
	void removeObjectReferences( const OidType& oid, Filter filter );

public:
	QMap<OidType, QMap<QString, QPair<OidType, bool> > >& relations();
	QMap<OidType, QMap<QString, Collection*> >& collections();

private:
	/*
	This QMap contains all existing objects and thus all have a valid Oid
	assigned
	*/
	QMap<OidType, Object*> m_objects;

	/*
	This is the pointer to the appropiate backend that will do the real persistance
	*/
	DbBackendIface *m_dbBackend;

	/*
	A pointer to the current manager object
	*/
	static Manager* m_self;

	/*
	Contains the maximum number of objects that should be loaded.
	Take into account that it includes modified and unmodified objects.
	*/
	uint m_maxObjects;

	/*
	Mantains the relation between objects
	*/
	QMap<OidType, QMap<QString, QPair<OidType, bool> > > m_relations;

	/*
	Mantains the collections of objects
	*/
	QMap<OidType, QMap<QString, Collection*> > m_collections;
};


#endif
