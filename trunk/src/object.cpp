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
#include <qvaluelist.h>

#include <kdebug.h>

#include "object.h"

/*
#ifdef DEBUG
const QString& classTypeTestFunction( const QString& o, Object* tmp )
{
	delete tmp;
	return o;
}
#endif
*/

// Property

Property::Property( Object *obj, const QString& name )
{
	m_object = obj;
	m_name = name;
}

QVariant::Type Property::type()
{
	return m_object->propertyValue( m_name.ascii() ).type();
}

QVariant Property::value()
{
	return m_object->propertyValue( m_name.ascii() );
}

void Property::setValue( const QVariant& value )
{
	m_object->setProperty( m_name, value );
}

QString Property::name()
{
	return m_name;
}


// PropertyIterator

PropertyIterator::PropertyIterator( Object *object, int pos ) : m_object( object ), m_pos( pos )
{
}
Property PropertyIterator::data()
{
	return m_object->property( m_pos );
}
const Property PropertyIterator::data() const
{
	return m_object->property( m_pos );
}
PropertyIterator& PropertyIterator::operator++()
{
	m_pos++;
	return *this;
}
PropertyIterator& PropertyIterator::operator--()
{
	m_pos--;
	return *this;
}
PropertyIterator PropertyIterator::operator++(int)
{
	PropertyIterator tmp = *this;
	m_pos++;
	return tmp;
}
PropertyIterator PropertyIterator::operator--(int)
{
	PropertyIterator tmp = *this;
	m_pos--;
	return tmp;
}
bool PropertyIterator::operator==( const PropertyIterator& it ) const
{
	return m_pos == it.m_pos && m_object == it.m_object;
}
bool PropertyIterator::operator!=( const PropertyIterator& it ) const
{
	return m_pos != it.m_pos || m_object != it.m_object;
}
Property PropertyIterator::operator*()
{
	return m_object->property( m_pos );
}
const Property PropertyIterator::operator*() const
{
	return m_object->property( m_pos );
}

PropertyIterator& PropertyIterator::operator=(const PropertyIterator& it)
{
	m_object = it.m_object;
	m_pos = it.m_pos;
	return *this;
}


// ObjectIterator

ObjectIterator::ObjectIterator( const OidType& oid, RelatedObjectsIterator it )
{
	m_oid = oid;
	m_it = it;
	m_collection = false;
}

ObjectIterator::ObjectIterator( QMapIterator<OidType,int> it, CreateObjectFunction function )
{
	assert( function );
	m_colit = it;
	m_createObjectFunction = function;
	m_collection = true;
}

Object* ObjectIterator::data()
{
	if ( m_collection ) {
		assert( m_createObjectFunction );
		return Manager::self()->load( m_colit.key(), m_createObjectFunction );
	} else {
		return Manager::self()->load( Manager::self()->relation( m_oid, (*m_it)->name() ), (*m_it)->createObjectFunction() );
	}
}


const Object* ObjectIterator::data() const
{
	if ( m_collection ) {
		assert( m_createObjectFunction );
		return Manager::self()->load( m_colit.key(), m_createObjectFunction );
	} else {
	//	return Manager::self()->load( m_it.data().first, m_it.data().second );
		return Manager::self()->load( Manager::self()->relation( m_oid, (*m_it)->name() ), (*m_it)->createObjectFunction() );

	}
}

QString ObjectIterator::key()
{
	assert( ! m_collection );
	return m_it.key();
}

const QString& ObjectIterator::key() const
{
	assert( ! m_collection );
	return m_it.key();
}

ObjectIterator& ObjectIterator::operator++()
{
	if ( m_collection )
		m_colit++;
	else
		m_it++;
	return *this;
}

ObjectIterator& ObjectIterator::operator--()
{
	if ( m_collection )
		m_colit--;
	else
		m_it--;
	return *this;
}

ObjectIterator ObjectIterator::operator++(int)
{
	ObjectIterator tmp = *this;
	if ( m_collection )
		m_colit++;
	else
		m_it++;
	return tmp;
}

ObjectIterator ObjectIterator::operator--(int)
{
	ObjectIterator tmp = *this;
	if ( m_collection )
		m_colit--;
	else
		m_it--;
	return tmp;
}

bool ObjectIterator::operator==( const ObjectIterator& it ) const
{
	if ( m_collection )
		return m_colit == it.m_colit;
	else
		return m_it == it.m_it;
}

bool ObjectIterator::operator!=( const ObjectIterator& it ) const
{
	if ( m_collection )
		return m_colit != it.m_colit;
	else
		return m_it != it.m_it;
}

Object* ObjectIterator::operator*()
{
	if ( m_collection ) {
		assert( m_createObjectFunction );
		return Manager::self()->load( m_colit.key(), m_createObjectFunction );
	} else {
		return Manager::self()->load( Manager::self()->relation( m_oid, (*m_it)->name() ), (*m_it)->createObjectFunction() );
	}
	//	return Manager::self()->load( m_it.data().first, m_it.data().second );
}

const Object* ObjectIterator::operator*() const
{
	if ( m_collection ) {
		assert( m_createObjectFunction );
		return Manager::self()->load( m_colit.key(), m_createObjectFunction );
	} else {
		return Manager::self()->load( Manager::self()->relation( m_oid, (*m_it)->name() ), (*m_it)->createObjectFunction() );
	}
		//	return Manager::self()->load( m_it.data().first, m_it.data().second );
}

ObjectIterator& ObjectIterator::operator=(const ObjectIterator& it)
{
	m_collection = it.m_collection;
	if ( m_collection ) {
		m_colit = it.m_colit;
		m_createObjectFunction = it.m_createObjectFunction;
	} else
		m_it = it.m_it;
	return *this;
}

// CollectionIterator

CollectionIterator::CollectionIterator( const OidType& oid, RelatedCollectionsIterator it )
{
	m_oid = oid;
	m_it = it;
}

Collection* CollectionIterator::data()
{
//	return Manager::self()->collection( m_oid, (*m_it).name() );
	//(*m_it).
	return Manager::self()->collection( m_oid, (*m_it) );
}

const Collection* CollectionIterator::data() const
{
	return Manager::self()->collection( m_oid, (*m_it) );
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

Collection* CollectionIterator::operator*()
{
	return Manager::self()->collection( m_oid, (*m_it) );
}

const Collection* CollectionIterator::operator*() const
{
	return Manager::self()->collection( m_oid, (*m_it) );
}

CollectionIterator& CollectionIterator::operator=(const CollectionIterator& it)
{
	m_it = it.m_it;
	return *this;
}


// Object

Object::Object()
{
	m_oid = 0;
	m_modified = false;
	//m_removed = false;
	m_loaded = false;
	m_classInfo = 0;
}

Object::~Object()
{
/*
  	QMapIterator<QString,Collection*> it( m_collections.begin() );
	QMapIterator<QString,Collection*> end( m_collections.end() );
	for ( ; it != end; ++it )
		delete (*it);
*/
}

Object* Object::create()
{
	Object *o = new Object();
	o->m_modified = true;
	assert( o );
	Manager::self()->add( o );
	return o;
}

Object* Object::create( OidType oid )
{
	return Manager::self()->load( oid, &createInstance );
}

Object* Object::createInstance()
{
	return new Object();
}

Object* Object::createObjectInstance() const
{
	return new Object();
}

ClassInfo* Object::classInfo()
{
  if ( m_classInfo )
    return m_classInfo;
  m_classInfo = Classes::classInfo( className() );
  return m_classInfo;
}

ClassInfo* Object::classInfo() const
{
  if ( m_classInfo )
    return m_classInfo;
  return Classes::classInfo( className() );
}


OidType Object::oid() const
{
	return m_oid;
}

void Object::setOid( const OidType& oid )
{
	m_oid = oid;
}

SeqType Object::seq() const
{
	return m_seq;
}

void Object::setSeq( const SeqType& seq )
{
	m_seq = seq;
}

bool Object::isNull() const
{
	return m_oid == 0;
}

bool Object::isModified() const
{
	return m_modified;
}

void Object::setModified( bool value )
{
	m_modified = value;
}

/*
bool Object::remove()
{
	m_removed = true;
	return Manager::self()->remove( this );
}
*/

/*
Functions for managing the properties
*/
PropertyIterator Object::propertiesBegin()
{
	return PropertyIterator( this, 0 );
}

PropertyIterator Object::propertiesEnd()
{
	return PropertyIterator( this, numProperties() );
}

int Object::numProperties() const
{
	return metaObject()->numProperties();
}

Property Object::property( int pos )
{
	return Property( this, metaObject()->propertyNames().at( pos ) );
}

Property Object::property( const QString& name )
{
	return Property( this, name );
}

bool Object::containsProperty( const QString& name )
{
	return metaObject()->findProperty( name.ascii() ) == -1 ? false: true;
}

QVariant Object::propertyValue( const char* name ) const
{
	return QObject::property( name );
}

/*
Functions for managing the other objects related
*/

ObjectIterator Object::objectsBegin()
{
	return ObjectIterator( m_oid, classInfo()->objectsBegin() );
}

ObjectIterator Object::objectsEnd()
{
  return ObjectIterator( m_oid, classInfo()->objectsEnd() );
}

int Object::numObjects() const
{
  return classInfo()->numObjects();
}

Object* Object::object( const QString& name ) const
{
	assert( containsObject( name ) );
	return Manager::self()->load( Manager::self()->relation( m_oid, name ), classInfo()->object( name )->createObjectFunction() );
}

bool Object::containsObject( const QString& name ) const
{
	return classInfo()->containsObject( name );
}

void Object::setObject( const QString& name, Object* object )
{
	assert( object );
	setObject( name, object->oid() );
}

void Object::setObject( const QString& name, const OidType& oid )
{
	MODIFIED;
	Manager::self()->setRelation( m_oid, classInfo(), name, oid );
}

/*
Functions for managing the collections of objects related
*/

CollectionIterator Object::collectionsBegin()
{
  return CollectionIterator( m_oid, classInfo()->collectionsBegin() );
}

CollectionIterator Object::collectionsEnd()
{
  return CollectionIterator( m_oid, classInfo()->collectionsEnd() );
}

bool Object::containsCollection( const QString& name ) const
{
  return classInfo()->containsCollection( name );
}

int Object::numCollections() const
{
  return classInfo()->numCollections();
}

Collection* Object::collection( const QString& name ) const
{
	QString relation = ClassInfo::relationName( name, classInfo()->name() );
	kdDebug() << k_funcinfo << ": name = " << name << ", classInfo()->name = " << classInfo()->name() << ", relation = " << relation << endl;
	return Manager::self()->collection( this, relation );
}


/*
Routines to ease the creation of relations between objects
*/
/*
void Object::createSubobject( const QString& className, CreateObjectFunction function )
{
	assert( ! m_objects.contains( className ) );
	assert( function );
	//m_objects.insert( className, QPair<OidType,CreateObjectFunction>(0, function) );
	Classes::currentClass()->addSubobject( className, function );
}

void Object::createSubcollection( const QString& className, CreateObjectFunction function, bool nToOne )
{
	assert( function );
	//m_collections.insert( className, new Collection( function, this, nToOne ) );
	Classes::currentClass()->addSubcollection( className, function, nToOne );
}
*/

#include "object.moc"
