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
#include <qvaluevector.h>
#include "defs.h"
#include "oidtype.h"

class Object;
class Collection;
class DbBackendIface;
class CollectionInfo;
class RelationInfo;
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

//	void setRemoved( bool removed );
//	bool isRemoved() const;

private:
	bool m_valid;
	Object* m_object;
	bool m_removed;
};

class RelationHandler
{
public:
	RelationHandler();
	RelationHandler( const OidType& oid, const RelationInfo* relation, bool modified );

	void setValid( bool valid );
	bool isValid() const;

	void setOid( const OidType& oid );
	const OidType& oid() const;

	void setModified( bool modified );
	bool isModified() const;

	void setRelationInfo( const RelationInfo* relation );
	const RelationInfo* relationInfo();

private:
	OidType m_oid;
	bool m_modified;
	bool m_valid;
	bool m_removed;
	const RelationInfo* m_relationInfo;
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

typedef QMap<Reference, RelationHandler> ManagerRelationMap;
typedef QMapIterator<Reference, RelationHandler> ManagerRelationIterator;
typedef QMapConstIterator<Reference, RelationHandler> ManagerRelationConstIterator;

typedef QMap<Reference, CollectionHandler> ManagerCollectionMap;
typedef QMapIterator<Reference, CollectionHandler> ManagerCollectionIterator;
typedef QMapConstIterator<Reference, CollectionHandler> ManagerCollectionConstIterator;

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

	void setCachePolicy( CachePolicy policy );
	CachePolicy cachePolicy() const;

	/* Functions related to object cache management */
	void setMaxObjects( Q_ULLONG max );
	Q_ULLONG maxObjects() const;

	void status() const;

	/* Functions related to object management */
	bool add( Object* object );
	bool remove( Object* object );

	Object* load( OidType oid, CreateObjectFunction f );
	Object* load( OidType oid, const ClassInfo* info );

	bool load( Collection* collection );
	bool load( Collection* collection, const QString& query );

	ManagerObjectMap& objects();
	ManagerRelationMap& relations();
	ManagerCollectionMap& collections();
	
	/* Relation management functions */
	void setRelation( const OidType& oid, const ClassInfo* classInfo, const QString& relation, const OidType& oidRelated, bool recursive = true );
	void addRelation( const OidType& oid, const CollectionInfo* collection, const OidType& oidRelated, bool recursive = true );
	void removeRelation( const OidType& oid, const CollectionInfo* collection, const OidType& oidRelated, bool recursive = true );
	
	OidType relation( const OidType& oid, const RelationInfo* related );
	OidType relation( const Object* object, const QString& related );
	Collection* collection( const OidType& oid, const CollectionInfo* related );

	bool notifyPropertyModified( const Object* object, const QString& function, const QVariant& value = QVariant() );
	void setNotificationHandler( NotificationHandler* handler );
	NotificationHandler* notificationHandler() const;
	

	bool commit();
	bool rollback();

	bool createSchema();

	void reset();

	void copyTo( Manager* manager );
	
	static const Q_ULLONG Unlimited = ULONG_MAX;
protected:
	void ensureUnderMaxObjects();
	void ensureUnderMaxRelations();
	void ensureUnderMaxCollections();

	void setEverythingUnmodified();

	enum Filter {
		Modified,
		Unmodified,
	};

public:
	void checkObjects();

private:
	/*!
	This QMap contains all existing objects and thus all have a valid Oid
	assigned
	*/
	ManagerObjectMap m_objects;

	/*!
	Mantains the relation between objects
	*/
	ManagerRelationMap m_relations;
	
	/*!
	Mantains the collections of objects
	*/
	ManagerCollectionMap m_collections;

	/*!
	Contains a list with the objects removed in the current transaction
	*/
	QValueList<OidType> m_removedOids;

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
	The object that is called when an event occurrs
	*/
	NotificationHandler *m_notificationHandler;
	
	/*!
	The current cache policy
	*/
	CachePolicy m_cachePolicy;
};


#endif
