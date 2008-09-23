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
#ifndef COLLECTION_H
#define COLLECTION_H

#include <qobject.h>

#include "defs.h"
#include "object.h"
#include "oidtype.h"
#include "classes.h"
#include "manager.h"

/*!
\brief This class contains information about a class' collection.

Stores the information of a relation to collection of objects. Mainly stores the
name of the relation and a pointer to a function that allows the creation of
an object of the related class type.

The constructor is only called from the addCollection function in ClassInfo which is called by COLLECTION and COLLECTIONN macros in object.h

\see ClassInfo
*/
class CollectionInfo
{
public:
    CollectionInfo( const ClassInfo *parent, const QString& name, const ClassInfo *children, bool nToOne );
    const QString& name() const;
    bool isNToOne() const;
    const ClassInfo* childrenClassInfo() const;
    const ClassInfo* parentClassInfo() const;
    bool browsable() const;
    bool inherited() const;

private:
    // This allows Classes to call setBrowsable & setNToOne
    friend class Classes;

    void setNToOne( bool nToOne );
    void setBrowsable( bool browsable );
    void setInherited( bool inherited );

    /*!
    Name of the relation
    */
    QString m_name;

    /*!
    Pointer to the parent ClassInfo
    */
    const ClassInfo *m_parentClassInfo;

    /*!
    Contains the cached ClassInfo of the related class
    */
    const ClassInfo *m_childrenClassInfo;

    /*!
    Contains whether the relation is N-1 or N-M.
    */
    bool m_nToOne;
    /*!
    Contains whether the collection is browsable or not
    */
    bool m_browsable;
    bool m_inherited;
};

class CollectionIterator
{
public:
	CollectionIterator();
	//CollectionIterator( QMapIterator<OidType,bool> it, const ClassInfo* classInfo, Manager* manager ); // DEPRECATED Qt3 -> Qt4 by Percy
    CollectionIterator( QMap<OidType,bool>::iterator it, const ClassInfo* classInfo, Manager* manager );
	Object* data();
	const Object* data() const;
	OidType key();
	const OidType& key() const;
	CollectionIterator& operator++();
	CollectionIterator& operator--();
	CollectionIterator operator++(int);
	CollectionIterator operator--(int);
	bool operator==( const CollectionIterator& it ) const;
	bool operator!=( const CollectionIterator& it ) const;
	Object* operator*();
	const Object* operator*() const;
	CollectionIterator& operator=(const CollectionIterator& it);

private:
	// QMapIterator<OidType,bool> m_it; // DEPRECATED Qt3 -> Qt4 by Percy
    QMap<OidType,bool>::iterator m_it;
	const ClassInfo* m_classInfo; 
	Manager* m_manager;
};

class CollectionConstIterator
{
public:
	CollectionConstIterator();
	//CollectionConstIterator( QMutableMapIterator<OidType,bool> it, const ClassInfo* classInfo, Manager* manager ); // DEPRECATED Qt3 -> Qt4 by Percy
    CollectionConstIterator( QMap<OidType,bool>::const_iterator it, const ClassInfo* classInfo, Manager* manager );
	Object* data();
	const Object* data() const;
	OidType key();
	const OidType& key() const;
	CollectionConstIterator& operator++();
	CollectionConstIterator& operator--();
	CollectionConstIterator operator++(int);
	CollectionConstIterator operator--(int);
	bool operator==( const CollectionConstIterator& it ) const;
	bool operator!=( const CollectionConstIterator& it ) const;
	Object* operator*();
	const Object* operator*() const;
	CollectionConstIterator& operator=(const CollectionConstIterator& it);

private:
	//QMutableMapIterator<OidType,bool> m_it; // DEPRECATED Qt3 -> Qt4 by Percy
    QMap<OidType,bool>::const_iterator m_it;
	const ClassInfo* m_classInfo;
	Manager* m_manager;
};

/**
@author Albert Cervera Areny
*/
class Collection
{
public:
	Collection();
	Collection( const QString& query, Manager* manager = 0 );
	Collection( const CollectionInfo *rel, const OidType& parent, Manager* manager );
	virtual ~Collection();
	Collection& operator=( const Collection& col );

	const CollectionInfo* collectionInfo() const;

	void setQuery( const QString& query, Manager* manager = 0 );

	Object* addNew();
	bool add( Object *object );
	bool remove( Object *object );
	bool remove( const OidType& oid );

	bool contains( const OidType& oid );

	Object* find( const QString& property, const QVariant& value );

	bool modified() const;
	void setModified( bool m );
	bool modified( const OidType& oid ) const;
	void setModified( const OidType& oid, bool m );

	CollectionIterator begin();
	CollectionIterator end();

	CollectionConstIterator begin() const;
	CollectionConstIterator end() const;
	CollectionConstIterator constBegin() const;
	CollectionConstIterator constEnd() const;

	int count() const;

	void clear();

	int numObjects();

	Object* object( OidType oid );
	Object* parent() const;
	OidType parentOid() const;
	const ClassInfo *childrenClassInfo() const;

	Manager* manager() const;

	bool simpleAdd( const OidType& oid );
	void simpleRemove( const OidType& oid );
	void simpleClear();

protected:
//	CreateObjectFunction createObjectFunction();

private:
	// This variable contains the name of the class of the objects the collection
	// contains. It is used like a cache, so it is filled the first time the objectsClassName()
	// function is called.
	QString m_objectsClassName;

	// Map containing all oids and a boolean that indicates whether the relation has been modified or not
	QMap<OidType,bool> m_collection;
	//CreateObjectFunction m_createObjectFunction;

	const CollectionInfo *m_collectionInfo;
	OidType m_parent;

	// If the nToOne variable is set
	bool m_nToOneSet;

	// Whether the collection has been loaded or not
	bool m_loaded;

	// Whether the collection has been modified or not
	bool m_modified;

//	CreateObjectFunction m_createObjectFunction;
	const ClassInfo *m_classInfo;

	Manager* m_manager;
};

template <class T>
class CollectionRef
{
public:
	CollectionRef()
	{
		m_oid = 0;
		m_manager = 0;
		m_collectionInfo = 0;
	}

	CollectionRef( T* collection )
	{
		if ( collection ) {
			Collection* col = static_cast<Collection*>( collection );
			m_oid = col->parentOid();
			m_manager = col->manager();
			if ( m_oid ) {
				m_collectionInfo = col->collectionInfo();
				m_collection = 0;
			} else {
				m_collectionInfo = 0;
				m_collection = collection;
			}
		} else {
			m_oid = 0;
			m_manager = 0;
			m_collectionInfo = 0;
			m_collection = 0;
		}
	}

	CollectionRef<T>& operator=( T* collection )
	{
		if ( collection ) {
			Collection* col = static_cast<Collection*>( collection );
			m_oid = col->parentOid();
			m_manager = col->manager();
			if ( m_oid ) {
				m_collectionInfo = col->collectionInfo();
				m_collection = 0;
			} else {
				m_collectionInfo = 0;
				m_collection = collection;
			}
		} else {
			m_oid = 0;
			m_manager = 0;
			m_collectionInfo = 0;
			m_collection = 0;
		}
		return *this;
	}

	CollectionRef<T>& operator=( CollectionRef<T> ref )
	{
		m_oid = ref.m_oid;
		m_manager = ref.m_manager;
		m_collectionInfo = ref.m_collectionInfo;
		m_collection = ref.m_collection;
		return *this;
	}

	T* operator->() const
	{
		if ( m_oid )
			return static_cast<T*>( m_manager->collection( m_oid, m_collectionInfo ) );
		else
			return m_collection;
	}

	T& operator*()
	{
		if ( m_oid )
			return &(static_cast<T*>( m_manager->collection( m_oid, m_collectionInfo ) ));
		else
			return *m_collection;
	}

	operator T*() const
	{
		if ( m_oid )
			return static_cast<T*>( m_manager->collection( m_oid, m_collectionInfo ) );
		else
			return m_collection;
	}

	OidType oid() const
	{
		return m_oid;
	}

	bool isNull() const
	{
		return m_oid == 0;
	}

	void setNull()
	{
		m_oid = 0;
	}

private:
	OidType m_oid;
	Manager* m_manager;
	const CollectionInfo *m_collectionInfo;
	Collection *m_collection;
};

#endif
