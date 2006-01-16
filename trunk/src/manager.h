/***************************************************************************
 *   Copyright (C) 2005 by Albert Cervera Areny   *
 *   albertca@hotpop.com   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
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

#define LOCK_ME m_manager->lockObject( this );
#define UNLOCK_ME m_manager->unlockObject( this );

class Object;
class Collection;
class DbBackendIface;
class RelatedCollection;
class RelatedObject;
class ClassInfo;
class NotificationHandler;

class ObjectHandler
{
public:
	ObjectHandler();
	ObjectHandler( Object* object );
	
	void setValid( bool valid );
	bool isValid() const;
	
	void setObject( Object* object );
	Object* object() const;
private:
	bool m_valid;
	Object* m_object;
};

class RelationHandler
{
public:
	RelationHandler();
	RelationHandler( const OidType& oid, bool modified );
	
	void setValid( bool valid );
	bool isValid() const;
	
	void setOid( const OidType& oid );
	const OidType& oid() const;
	
	void setModified( bool modified );
	bool isModified() const;
private:
	OidType m_oid;
	bool m_modified;
	bool m_valid;
};

class CollectionHandler
{
public:
	CollectionHandler();
	CollectionHandler( Collection *collection );
	
	void setValid( bool valid );
	bool isValid() const;
	
	void setCollection( Collection* collection );
	Collection* collection() const;
private:
	Collection *m_collection;
	bool m_valid;
};

class Reference
{
public:
	Reference();
	Reference( const Reference& reference );
	Reference( const OidType& oid, const QString& name );
	
	bool operator==( const Reference& reference ) const;
	bool operator<( const Reference& reference ) const;

	void setOid( const OidType& oid );
	const OidType& oid() const;
	
	void setName( const QString& name );
	const QString& name() const;
	
private:
	OidType m_oid;
	QString m_name;
};

typedef QMap<OidType, ObjectHandler> ManagerObjectMap;
typedef QMapIterator<OidType, ObjectHandler> ManagerObjectIterator;
typedef QMapConstIterator<OidType, ObjectHandler> ManagerObjectConstIterator;

typedef QMap<Reference, RelationHandler> ManagerRelatedObjectMap;
typedef QMapIterator<Reference, RelationHandler> ManagerRelatedObjectIterator;
typedef QMapConstIterator<Reference, RelationHandler> ManagerRelatedObjectConstIterator;

typedef QMap<Reference, CollectionHandler> ManagerRelatedCollectionMap;
typedef QMapIterator<Reference, CollectionHandler> ManagerRelatedCollectionIterator;
typedef QMapConstIterator<Reference, CollectionHandler> ManagerRelatedCollectionConstIterator;

/*
typedef QMap<OidType, QMap<QString, RelationHandler> > ManagerRelatedObjectMap;
typedef QMapIterator<OidType, QMap<QString, RelationHandler> > ManagerRelatedObjectIterator;
typedef QMapConstIterator<OidType, QMap<QString, RelationHandler> > ManagerRelatedObjectConstIterator;

typedef QMap<OidType, QMap<QString, CollectionHandler> > ManagerRelatedCollectionMap;
typedef QMapIterator<OidType, QMap<QString, CollectionHandler> > ManagerRelatedCollectionIterator;
typedef QMapConstIterator<OidType, QMap<QString, CollectionHandler> > ManagerRelatedCollectionConstIterator;
*/

typedef QMap<Object*,bool> LockedObjectsMap;

#define MaxObjects 100

class Manager
{
public:
	enum CachePolicy {
		FreeMaxOnLoad = 1, ///< Default. Frees invalid cache until MaxObjects are kept in memory. Called every time a new object is loaded in memory.
		FreeMaxOnTransaction, ///< Frees invalid cache until MaxObjects are kept in memory. Called only on commit() and rollback()
		FreeAllOnLoad, ///< Frees the whole invalid cache every time a new object is loaded in memory.
		FreeAllOnTransaction ///< Frees the whole invalid cache on commit() and rollback() only.
	};

	Manager( DbBackendIface *backend, NotificationHandler *handler = 0 );
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

	/*!
	Returns a pointer to the requested object.
	@param oid The oid of the object
	@param f The pointer to the function that creates an object of the type of the expected object
	*/
	Object* load( OidType oid, CreateObjectFunction f );
	
	Object* load( OidType oid, const ClassInfo* info );

	/* Functions related to collection management */
	bool load( Collection* collection );

	// This function has to disappear some day. When Collection becomes an appropiate
	// class hierarchy
	bool load( Collection* collection, const QString& query );

	ManagerObjectIterator begin();
	ManagerObjectIterator end();

	/* Relation management functions */
	void setRelation( const OidType& oid, const ClassInfo* classInfo, const QString& relation, const OidType& oidRelated, bool recursive = true );
	void addRelation( const OidType& oid, const RelatedCollection* collection, const OidType& oidRelated, bool recursive = true );
	void removeRelation( const OidType& oid, const RelatedCollection* collection, const OidType& oidRelated, bool recursive = true );
	
/*
	void setModifiedRelation( const OidType& oid, const ClassInfo* classInfo, const QString& relationName, bool modified, bool recursive = true );
	void setModifiedRelation( const OidType& oid, RelatedCollection* collection, const OidType& oidRelated, bool modified, bool recursive = true );
*/

//	OidType relation( const OidType& oid, const QString& relation );
//	OidType relation( const Object* object, const QString& relation );
	OidType relation( const OidType& oid, const RelatedObject* related );
	// Needed by the GETOBJECT macro in Object derived classes
	OidType relation( const Object* object, const QString& related );
	
//	Collection* collection( const Object* object, const QString& relation );
	Collection* collection( const OidType& oid, const RelatedCollection* related );

	bool notifyPropertyModified( const Object* object, const QString& function, const QVariant& value = QVariant() );
	void setNotificationHandler( NotificationHandler* handler );
	NotificationHandler* notificationHandler() const;
	

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

public:
	ManagerRelatedObjectMap& relations();
	ManagerRelatedCollectionMap& collections();
	void checkObjects();

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
	
	/*!
	The object that is called when an event occurrs
	*/
	NotificationHandler *m_notificationHandler;
};


#endif
