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
#ifndef COLLECTION_H
#define COLLECTION_H

#include <qobject.h>

#include "defs.h"
#include "object.h"
#include "oidtype.h"
#include "classes.h"

class Object;

class CollectionIterator
{
public:
	CollectionIterator( QMapIterator<OidType,bool> it, const ClassInfo* classInfo, Manager* manager );
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
	QMapIterator<OidType,bool> m_it;
	const ClassInfo* m_classInfo;
	Manager* m_manager;
};


/**
@author Albert Cervera Areny
*/
class Collection
{
public:
	Collection( const QString& query, Manager* manager = 0 );
	Collection( RelatedCollection *rel, const OidType& parent, Manager* manager );
	virtual ~Collection();
	Collection& operator=( const Collection& col );

	RelatedCollection* collectionInfo() const;

	Object* addNew();
	bool add( Object *object );
	bool remove( Object *object );
	bool remove( const OidType& oid );

	bool contains( const OidType& oid );

	bool modified() const;
	void setModified( bool m );
	bool modified( const OidType& oid ) const;
	void setModified( const OidType& oid, bool m );

	CollectionIterator begin();
	CollectionIterator end();

	int count() const;

	void clear();

	int numObjects();

	Object* object( OidType oid );
	Object* parent() const;
	OidType parentOid() const;
	const ClassInfo *childrenClassInfo() const;

	bool simpleAdd( const OidType& oid );
	void simpleRemove( const OidType& oid );
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

	RelatedCollection *m_collectionInfo;
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

#endif
