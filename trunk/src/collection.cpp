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
#include <assert.h>

#include <kdebug.h>

#include "collection.h"
#include "defs.h"
#include "object.h"
#include "manager.h"
#include "tokenizer.h"

// CollectionIterator
CollectionIterator::CollectionIterator()
{
	m_classInfo = 0;
	m_manager = 0;
}

CollectionIterator::CollectionIterator( QMapIterator<OidType,bool> it, const ClassInfo *classInfo, Manager* manager )
{
	assert( classInfo );
	m_it = it;
	m_classInfo = classInfo;
	m_manager = manager;
}

Object* CollectionIterator::data()
{
	assert( m_classInfo );
	return m_manager->load( m_it.key(), m_classInfo );
}

const Object* CollectionIterator::data() const
{
	assert( m_classInfo );
	return m_manager->load( m_it.key(), m_classInfo );
}

OidType CollectionIterator::key()
{
	return m_it.key();
}

const OidType& CollectionIterator::key() const
{
	return m_it.key();
}

CollectionIterator& CollectionIterator::operator++()
{
	m_it++;
	return *this;
}

CollectionIterator& CollectionIterator::operator--()
{
	m_it--;
	return *this;
}

CollectionIterator CollectionIterator::operator++(int)
{
	CollectionIterator tmp = *this;
	m_it++;
	return tmp;
}

CollectionIterator CollectionIterator::operator--(int)
{
	CollectionIterator tmp = *this;
	m_it--;
	return tmp;
}

bool CollectionIterator::operator==( const CollectionIterator& it ) const
{
	return m_it == it.m_it;
}

bool CollectionIterator::operator!=( const CollectionIterator& it ) const
{
	return m_it != it.m_it;
}

Object* CollectionIterator::operator*()
{
	assert( m_classInfo );
	return m_manager->load( m_it.key(), m_classInfo );
}

const Object* CollectionIterator::operator*() const
{
	assert( m_classInfo );
	return m_manager->load( m_it.key(), m_classInfo );
}

CollectionIterator& CollectionIterator::operator=(const CollectionIterator& it)
{
	m_it = it.m_it;
	m_classInfo = it.m_classInfo;
	m_manager = it.m_manager;
	return *this;
}

// CollectionIterator

CollectionConstIterator::CollectionConstIterator()
{
	m_classInfo = 0;
	m_manager = 0;
}

CollectionConstIterator::CollectionConstIterator( QMapConstIterator<OidType,bool> it, const ClassInfo *classInfo, Manager* manager )
{
	assert( classInfo );
	m_it = it;
	m_classInfo = classInfo;
	m_manager = manager;
}

Object* CollectionConstIterator::data()
{
	assert( m_classInfo );
	return m_manager->load( m_it.key(), m_classInfo );
}

const Object* CollectionConstIterator::data() const
{
	assert( m_classInfo );
	return m_manager->load( m_it.key(), m_classInfo );
}

OidType CollectionConstIterator::key()
{
	return m_it.key();
}

const OidType& CollectionConstIterator::key() const
{
	return m_it.key();
}

CollectionConstIterator& CollectionConstIterator::operator++()
{
	m_it++;
	return *this;
}

CollectionConstIterator& CollectionConstIterator::operator--()
{
	m_it--;
	return *this;
}

CollectionConstIterator CollectionConstIterator::operator++(int)
{
	CollectionConstIterator tmp = *this;
	m_it++;
	return tmp;
}

CollectionConstIterator CollectionConstIterator::operator--(int)
{
	CollectionConstIterator tmp = *this;
	m_it--;
	return tmp;
}

bool CollectionConstIterator::operator==( const CollectionConstIterator& it ) const
{
	return m_it == it.m_it;
}

bool CollectionConstIterator::operator!=( const CollectionConstIterator& it ) const
{
	return m_it != it.m_it;
}

Object* CollectionConstIterator::operator*()
{
	assert( m_classInfo );
	return m_manager->load( m_it.key(), m_classInfo );
}

const Object* CollectionConstIterator::operator*() const
{
	assert( m_classInfo );
	return m_manager->load( m_it.key(), m_classInfo );
}

CollectionConstIterator& CollectionConstIterator::operator=(const CollectionConstIterator& it)
{
	m_it = it.m_it;
	m_classInfo = it.m_classInfo;
	m_manager = it.m_manager;
	return *this;
}

// Collection

Collection::Collection()
{
	m_collectionInfo = 0;
	m_parent = 0;
	m_modified = false;
	m_classInfo = 0;
	m_manager = 0;
}

Collection::Collection( const QString& query, Manager* manager )
{
	m_collectionInfo = 0;
	m_parent = 0;
	m_modified = false;
	m_classInfo = 0;
	m_manager = 0;
	setQuery( query, manager );
}

Collection::Collection( const RelatedCollection *rel, const OidType& parent, Manager* manager )
{
	assert( rel );
	m_collectionInfo = rel;
	m_parent = parent;
	m_modified = false;
	m_classInfo = m_collectionInfo->childrenClassInfo();
	m_manager = manager;
	m_manager->load( this );
	assert( m_collectionInfo );
}

Collection::~Collection()
{
}

void Collection::setQuery( const QString& query, Manager* manager )
{
	m_collection.clear();
	if ( manager )
		m_manager = manager;
	else
		m_manager = Manager::self();
	m_manager->load( this, query );
	if ( Classes::contains( query ) )
		m_classInfo = Classes::classInfo( query );
	else {
		Tokenizer tokenizer( query, " " );
		tokenizer.nextToken(); // First item should be SELECT
		QString c = tokenizer.nextToken();
		c = c.left( c.find( '.' ) );
		if ( ! Classes::contains( c ) ) {
			kdDebug() << k_funcinfo << "Could not find class '" << c << "'" << endl;
			m_classInfo = 0;
			return;
		}
		m_classInfo = Classes::classInfo( c );
	}
	m_collectionInfo = 0;
}

Collection& Collection::operator=( const Collection& col )
{
	m_objectsClassName = col.m_objectsClassName;
	m_collection = col.m_collection;	
	m_collectionInfo = col.m_collectionInfo;
	m_parent = col.m_parent;
	m_nToOneSet = col.m_nToOneSet;
	m_loaded = col.m_loaded;
	m_modified = col.m_modified;
	m_classInfo = col.m_classInfo;
	return *this;
}

Object* Collection::addNew()
{
	Object *obj = m_collectionInfo->childrenClassInfo()->create();
	assert( obj );
	m_manager->add( obj );
	add( obj );
	return obj;
}

bool Collection::add( Object *object )
{
	assert( object );
	if ( object->oid() ) {
		m_manager->addRelation( m_parent, m_collectionInfo, object->oid(), true );
		m_collection.insert( object->oid(), 0 );
		m_modified = true;
		return true;
	}
	return false;
}

/*!
Adds the oid to the list. Plain. Called by Manager. If Manager called add() it would create an infinite loop or a crash (I think).
*/
bool Collection::simpleAdd( const OidType& oid )
{
	assert( oid != 0 );
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
	m_manager->removeRelation( m_parent, m_collectionInfo, oid, true );
	return true;
}

/*!
Returns whether the collection contains an object with oid oid.
*/
bool Collection::contains( const OidType& oid )
{
	return m_collection.contains( oid );
}

/*!
Returns a pointer to the first object in the collection whose property "property" 
equals "value".
*/
Object* Collection::find( const QString& property, const QVariant& value )
{
	CollectionConstIterator it( constBegin() );
	CollectionConstIterator end( constEnd() );
	for ( ; it != end; ++it ) {
		if ( it.data()->property( property ).value() == value )
			return it.data();
	}
	return 0;
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

bool Collection::modified( const OidType& oid ) const
{
	return m_collection[ oid ];
}

void Collection::setModified( const OidType& oid, bool m )
{
	if ( m_collection.contains( oid ) ) {
		m_collection[ oid ] = m;
		m_modified &= m;
	}
}

/*!
Returns a pointer to the object with the given oid. If the Oid isn't
in the collection a null pointer is returned
*/
Object* Collection::object( OidType oid )
{
	if ( m_collection.contains( oid ) )
		return m_manager->load( oid, childrenClassInfo() );
	return 0;
}

/*!
Returns an iterator pointing the first object of the collection
*/
CollectionIterator Collection::begin()
{
	return CollectionIterator( m_collection.begin(), m_classInfo, m_manager );
}

/*!
Returns an iterator pointing past the last object of the collection.
*/
CollectionIterator Collection::end()
{
	return CollectionIterator( m_collection.end(), m_classInfo, m_manager );
}

/*!
Returns an iterator pointing the first object of the collection
*/
CollectionConstIterator Collection::begin() const
{
	return CollectionConstIterator( m_collection.begin(), m_classInfo, m_manager );
}

/*!
Returns an iterator pointing past the last object of the collection.
*/
CollectionConstIterator Collection::end() const
{
	return CollectionConstIterator( m_collection.end(), m_classInfo, m_manager );
}

/*!
Returns an iterator pointing the first object of the collection
*/
CollectionConstIterator Collection::constBegin() const
{
	return CollectionConstIterator( m_collection.begin(), m_classInfo, m_manager );
}

/*!
Returns an iterator pointing past the last object of the collection.
*/
CollectionConstIterator Collection::constEnd() const
{
	return CollectionConstIterator( m_collection.end(), m_classInfo, m_manager );
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
	return m_manager->load( m_parent, m_collectionInfo->parentClassInfo() );
}

/*!
Returns the Oid of the object the collection is in.
The function is provided for convenience. Mostly useful in the database backends.
*/
OidType Collection::parentOid() const
{
	return m_parent;
}

/*!
Returns a pointer to a RelatedCollection which contains the information for the current collection.
*/
const RelatedCollection* Collection::collectionInfo() const
{
	return m_collectionInfo;
}

const ClassInfo* Collection::childrenClassInfo() const
{
	return m_classInfo;
}

/*!
Returns a pointer to the manager that handles te collection
*/
Manager* Collection::manager() const
{
	return m_manager;
}
