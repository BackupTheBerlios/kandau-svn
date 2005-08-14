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


// ConstProperty

ConstProperty::ConstProperty( const Object *obj, const QString& name )
{
	m_object = obj;
	m_name = name;
}

QVariant::Type ConstProperty::type() const
{
	return m_object->propertyValue( m_name.ascii() ).type();
}

QVariant ConstProperty::value() const
{
	return m_object->propertyValue( m_name.ascii() );
}

const QString& ConstProperty::name() const
{
	return m_name;
}


// Property

Property::Property( Object *obj, const QString& name )
{
	m_object = obj;
	m_name = name;
}

QVariant::Type Property::type() const
{
	return m_object->propertyValue( m_name.ascii() ).type();
}

QVariant Property::value() const
{
	return m_object->propertyValue( m_name.ascii() );
}

void Property::setValue( const QVariant& value )
{
	m_object->setProperty( m_name, value );
}

const QString& Property::name() const
{
	return m_name;
}


// PropertiesIterator

PropertiesIterator::PropertiesIterator( Object *object, int pos ) : m_object( object ), m_pos( pos )
{
}
Property PropertiesIterator::data()
{
	return m_object->property( m_pos );
}
const Property PropertiesIterator::data() const
{
	return m_object->property( m_pos );
}
PropertiesIterator& PropertiesIterator::operator++()
{
	m_pos++;
	return *this;
}
PropertiesIterator& PropertiesIterator::operator--()
{
	m_pos--;
	return *this;
}
PropertiesIterator PropertiesIterator::operator++(int)
{
	PropertiesIterator tmp = *this;
	m_pos++;
	return tmp;
}
PropertiesIterator PropertiesIterator::operator--(int)
{
	PropertiesIterator tmp = *this;
	m_pos--;
	return tmp;
}
bool PropertiesIterator::operator==( const PropertiesIterator& it ) const
{
	return m_pos == it.m_pos && m_object == it.m_object;
}
bool PropertiesIterator::operator!=( const PropertiesIterator& it ) const
{
	return m_pos != it.m_pos || m_object != it.m_object;
}
Property PropertiesIterator::operator*()
{
	return m_object->property( m_pos );
}
const Property PropertiesIterator::operator*() const
{
	return m_object->property( m_pos );
}

PropertiesIterator& PropertiesIterator::operator=(const PropertiesIterator& it)
{
	m_object = it.m_object;
	m_pos = it.m_pos;
	return *this;
}


// ConstPropertiesIterator

ConstPropertiesIterator::ConstPropertiesIterator( const Object *object, int pos ) : m_object( object ), m_pos( pos )
{
}
ConstProperty ConstPropertiesIterator::data()
{
	return m_object->property( m_pos );
}
const ConstProperty ConstPropertiesIterator::data() const
{
	return m_object->property( m_pos );
}
ConstPropertiesIterator& ConstPropertiesIterator::operator++()
{
	m_pos++;
	return *this;
}
ConstPropertiesIterator& ConstPropertiesIterator::operator--()
{
	m_pos--;
	return *this;
}
ConstPropertiesIterator ConstPropertiesIterator::operator++(int)
{
	ConstPropertiesIterator tmp = *this;
	m_pos++;
	return tmp;
}
ConstPropertiesIterator ConstPropertiesIterator::operator--(int)
{
	ConstPropertiesIterator tmp = *this;
	m_pos--;
	return tmp;
}
bool ConstPropertiesIterator::operator==( const ConstPropertiesIterator& it ) const
{
	return m_pos == it.m_pos && m_object == it.m_object;
}
bool ConstPropertiesIterator::operator!=( const ConstPropertiesIterator& it ) const
{
	return m_pos != it.m_pos || m_object != it.m_object;
}
ConstProperty ConstPropertiesIterator::operator*()
{
	return m_object->property( m_pos );
}
const ConstProperty ConstPropertiesIterator::operator*() const
{
	return m_object->property( m_pos );
}

ConstPropertiesIterator& ConstPropertiesIterator::operator=(const ConstPropertiesIterator& it)
{
	m_object = it.m_object;
	m_pos = it.m_pos;
	return *this;
}

// ObjectsIterator

ObjectsIterator::ObjectsIterator( const OidType& oid, RelatedObjectsIterator it, Manager* manager )
{
	m_oid = oid;
	m_it = it;
	m_manager = manager;
}

Object* ObjectsIterator::data()
{
	return m_manager->load( m_manager->relation( m_oid, (*m_it)->name() ), (*m_it)->createObjectFunction() );
}

const Object* ObjectsIterator::data() const
{
	return m_manager->load( m_manager->relation( m_oid, (*m_it)->name() ), (*m_it)->createObjectFunction() );
}

QString ObjectsIterator::key()
{
	return m_it.key();
}

const QString& ObjectsIterator::key() const
{
	return m_it.key();
}

const RelatedObject* ObjectsIterator::relatedObject() const
{
	return (*m_it);
}

ObjectsIterator& ObjectsIterator::operator++()
{
	m_it++;
	return *this;
}

ObjectsIterator& ObjectsIterator::operator--()
{
	m_it--;
	return *this;
}

ObjectsIterator ObjectsIterator::operator++(int)
{
	ObjectsIterator tmp = *this;
	m_it++;
	return tmp;
}

ObjectsIterator ObjectsIterator::operator--(int)
{
	ObjectsIterator tmp = *this;
	m_it--;
	return tmp;
}

bool ObjectsIterator::operator==( const ObjectsIterator& it ) const
{
	return m_it == it.m_it;
}

bool ObjectsIterator::operator!=( const ObjectsIterator& it ) const
{
	return m_it != it.m_it;
}

Object* ObjectsIterator::operator*()
{
	return m_manager->load( m_manager->relation( m_oid, (*m_it)->name() ), (*m_it)->createObjectFunction() );
}

const Object* ObjectsIterator::operator*() const
{
	return m_manager->load( m_manager->relation( m_oid, (*m_it)->name() ), (*m_it)->createObjectFunction() );
}

ObjectsIterator& ObjectsIterator::operator=(const ObjectsIterator& it)
{
	m_it = it.m_it;
	m_oid = it.m_oid;
	return *this;
}

// CollectionsIterator

CollectionsIterator::CollectionsIterator( const OidType& oid, RelatedCollectionsIterator it, Manager* manager )
{
	m_oid = oid;
	m_it = it;
	m_manager = manager;
}

Collection* CollectionsIterator::data()
{
	return m_manager->collection( m_oid, (*m_it) );
}

const Collection* CollectionsIterator::data() const
{
	return m_manager->collection( m_oid, (*m_it) );
}

CollectionsIterator& CollectionsIterator::operator++()
{
	m_it++;
	return *this;
}

CollectionsIterator& CollectionsIterator::operator--()
{
	m_it--;
	return *this;
}

CollectionsIterator CollectionsIterator::operator++(int)
{
	CollectionsIterator tmp = *this;
	m_it++;
	return tmp;
}

CollectionsIterator CollectionsIterator::operator--(int)
{
	CollectionsIterator tmp = *this;
	m_it--;
	return tmp;
}

bool CollectionsIterator::operator==( const CollectionsIterator& it ) const
{
	return m_it == it.m_it;
}

bool CollectionsIterator::operator!=( const CollectionsIterator& it ) const
{
	return m_it != it.m_it;
}

Collection* CollectionsIterator::operator*()
{
	return m_manager->collection( m_oid, (*m_it) );
}

const Collection* CollectionsIterator::operator*() const
{
	return m_manager->collection( m_oid, (*m_it) );
}

CollectionsIterator& CollectionsIterator::operator=(const CollectionsIterator& it)
{
	m_it = it.m_it;
	return *this;
}


// Object

Object::Object()
{
	m_manager = 0;
	m_oid = 0;
	m_seq = 0;
	m_modified = false;
	m_classInfo = 0;
}

Object::~Object()
{
}

Object& Object::operator=(const Object& obj)
{
	m_classInfo = obj.m_classInfo;
	m_oid = obj.m_oid;
	m_seq = obj.m_seq;
	m_modified = obj.m_modified;
	for ( int i = 0; i < numProperties(); ++i )
		property(i).setValue( obj.property(i).value() );
	return *this;
}

Object* Object::create( Manager* manager )
{
	Object *o = new Object();
	assert( o );
	o->m_modified = true;
	o->setManager( manager );
	o->m_manager->add( o );
	return o;
}

Object* Object::create( OidType oid, Manager* manager )
{
	if ( manager )
		return manager->load( oid, &createInstance );
	else
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

Manager* Object::manager() const
{
	return m_manager;
}

void Object::setManager( Manager* m )
{
	if ( m )
		m_manager = m;
	else
		m_manager = Manager::self();
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
Functions for managing the properties
*/
PropertiesIterator Object::propertiesBegin()
{
	return PropertiesIterator( this, 0 );
}

ConstPropertiesIterator Object::constPropertiesBegin() const
{
	return ConstPropertiesIterator( this, 0 );
}

PropertiesIterator Object::propertiesEnd()
{
	return PropertiesIterator( this, numProperties() );
}

ConstPropertiesIterator Object::constPropertiesEnd() const
{
	return ConstPropertiesIterator( this, numProperties() );
}

int Object::numProperties() const
{
	return metaObject()->numProperties();
}

Property Object::property( int pos )
{
	return Property( this, metaObject()->propertyNames().at( pos ) );
}

ConstProperty Object::property( int pos ) const
{
	return ConstProperty( this, metaObject()->propertyNames().at( pos ) );
}

Property Object::property( const QString& name )
{
	return Property( this, name );
}

ConstProperty Object::property( const QString& name ) const
{
	return ConstProperty( this, name );
}

bool Object::containsProperty( const QString& name ) const
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

ObjectsIterator Object::objectsBegin()
{
	return ObjectsIterator( m_oid, classInfo()->objectsBegin(), m_manager );
}

ObjectsIterator Object::objectsEnd()
{
	return ObjectsIterator( m_oid, classInfo()->objectsEnd(), m_manager );
}

int Object::numObjects() const
{
	return classInfo()->numObjects();
}

Object* Object::object( const QString& name ) const
{
	assert( containsObject( name ) );
	return m_manager->load( m_manager->relation( m_oid, name ), classInfo()->object( name )->createObjectFunction() );
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
	m_manager->setRelation( m_oid, classInfo(), name, oid );
}

/*
Functions for managing the collections of objects related
*/

CollectionsIterator Object::collectionsBegin()
{
	return CollectionsIterator( m_oid, classInfo()->collectionsBegin(), m_manager );
}

CollectionsIterator Object::collectionsEnd()
{
	return CollectionsIterator( m_oid, classInfo()->collectionsEnd(), m_manager );
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
	return m_manager->collection( this, relation );
}

#include "object.moc"
