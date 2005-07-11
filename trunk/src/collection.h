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

/*
TOTHINK :)
	- It'd be nice to add some cardinality information in the constructor
	such as the minimum and maximum number of elements that can be contained.
	However, although the maximum is easily assured, the minimum would require
	some thinking and envolving the manager and transactions. Too early to
	think about it right now.
*/

class ObjectIterator;

/**
@author Albert Cervera Areny
*/
class Collection
{
public:
	Collection( RelatedCollection *rel, const OidType& parent );
	virtual ~Collection();

	RelatedCollection* collectionInfo() const;

	Object* addNew();
	bool add( Object *object );
	bool remove( Object *object );
	bool remove( const OidType& oid );

	bool contains( const OidType& oid );

	bool load();

	bool modified() const;
	void setModified( bool m );

	ObjectIterator begin();
	ObjectIterator end();

	int count() const;

	void clear();

	int numObjects();

	Object* object( OidType oid );
	Object* parent() const;

	bool simpleAdd( const OidType& oid );
	void simpleRemove( const OidType& oid );

private:
	// This variable contains the name of the class of the objects the collection
	// contains. It is used like a cache, so it is filled the first time the objectsClassName()
	// function is called.
	QString m_objectsClassName;
	//Object *m_parent;
	QMap<OidType,int> m_collection;
	//CreateObjectFunction m_createObjectFunction;

	RelatedCollection *m_collectionInfo;
	OidType m_parent;

	// If the nToOne variable is set
	bool m_nToOneSet;

	// Whether the collection has been loaded or not
	bool m_loaded;

	// Whether the collection has been modified or not
	bool m_modified;
};

#endif
