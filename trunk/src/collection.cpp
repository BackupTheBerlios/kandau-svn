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
#include <assert.h>

#include <kdebug.h>

#include "collection.h"
#include "defs.h"
#include "object.h"
#include "manager.h"

/*
Collection::Collection( CreateObjectFunction function, Object *parent, bool nToOne )
{
	assert( function );
	m_createObjectFunction = function;
	m_parent = parent;

	m_nToOne = nToOne;
	m_nToOneSet = ! nToOne;
	m_loaded = false;

	if ( ! parent )
		load();
}
*/

Collection::Collection( RelatedCollection *rel, const OidType& parent )
{
	assert( rel );
	m_collectionInfo = rel;
	m_parent = parent;
	m_modified = false;
}

Collection::~Collection()
{
}

/*
CreateObjectFunction Collection::createObjectFunction()
{
	return m_createObjectFunction;
}
*/

Object* Collection::addNew()
{
//	Object *obj = m_createObjectFunction();
	Object *obj = m_collectionInfo->childrenClassInfo()->create();
	assert( obj );
	Manager::self()->add( obj );
	add( obj );
	return obj;
}

bool Collection::add( Object *object )
{
	assert( object );
	if ( object->oid() ) {
		Manager::self()->addRelation( m_parent, m_collectionInfo, object->oid(), true );
		m_collection.insert( object->oid(), 0 );
		m_modified = true;
		return true;
		/*if ( Manager::self()->add( this, object ) ) {
			m_collection.insert( object->oid(), 0 );
			return true;
		} else {
			kdDebug() << "Error adding an object to a collection" << endl;
			return false;
		}*/
	}
	return false;
}

/*!
Adds the oid to the list. Plain. Called by Manager. If Manager called add() it would create an infinite loop or a crash (I think).
*/
bool Collection::simpleAdd( const OidType& oid )
{
	m_collection.insert( oid, 0 );
	m_modified = true;
	return true;
}

bool Collection::remove( Object *object )
{
	assert( object );
	return remove( object->oid() );
}

/*!
Removes the oid from the list. Plain. Called by Manager. If Manager called remove() it would create an infinite loop or a crash.
*/
void Collection::simpleRemove( const OidType& oid )
{
	m_modified = true;
	m_collection.remove( oid );
}

/*!
Removes the object with oid oid from the collection.
*/
bool Collection::remove( const OidType& oid )
{
	load();
	Manager::self()->removeRelation( m_parent, m_collectionInfo, oid, true );
	return true;
}

/*!
Returns whether the collection contains an object with oid oid.
*/
bool Collection::contains( const OidType& oid )
{
	load();
	return m_collection.contains( oid );
}

/*!
Loads the collection.
*/
bool Collection::load()
{
/*
	if ( m_loaded )
		return true;
	m_loaded = Manager::self()->load( this );
	return m_loaded;
*/
	return true;
}

/*!
@return the modified flag of the collection. A collection has been modified if an object has been added or removed.
*/
bool Collection::modified() const
{
	return m_modified;
}

/*!
Sets the modified flag of the object to m.
*/
void Collection::setModified( bool m )
{
	m_modified = m;
}

/*!
Returns a pointer to the object with the given oid. If the Oid isn't
in the collection a null pointer is returned
*/
Object* Collection::object( OidType oid )
{
	if ( m_collection.contains( oid ) )
		return Manager::self()->object( oid );
	return 0;
}

/*!
Returns an iterator pointing the first object of the collection
*/
ObjectIterator Collection::begin()
{
	load();
	//return ObjectIterator( m_collection.begin(), m_createObjectFunction );
	return ObjectIterator( m_collection.begin(), m_collectionInfo->childrenClassInfo()->createObjectFunction() );
}

/*!
Returns an iterator pointing past the last object of the collection.
*/
ObjectIterator Collection::end()
{
	//assert( m_createObjectFunction );
	load();
	//return ObjectIterator( m_collection.end(), m_createObjectFunction );
	return ObjectIterator( m_collection.end(), m_collectionInfo->childrenClassInfo()->createObjectFunction() );
}

/*!
Returns the number of objects in the collection (same as numObjects())
*/
int Collection::count() const
{
	return m_collection.count();
}

/*!
Returns the number of objects in the collection (same as count())
*/
int Collection::numObjects()
{
	load();
	return m_collection.count();
}

/*!
Removes all items from the collection. Right now only removes the items from the
map, doesn't call the remove function to remove the objects from the database.
TODO: I should think about that...
*/
void Collection::clear()
{
	m_collection.clear();
}

/*!
Returns a pointer to the object the collection is in.
*/
Object* Collection::parent() const
{
	return Manager::self()->load( m_parent, m_collectionInfo->parentClassInfo()->createObjectFunction() );
}

/*!
Returns a pointer to a RelatedCollection which contains the information for the current collection.
*/
RelatedCollection* Collection::collectionInfo() const
{
	return m_collectionInfo;
}
