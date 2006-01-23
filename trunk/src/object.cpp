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
#include <qvaluelist.h>

#include <kdebug.h>

#include "object.h"


// Property

Property::Property( Object *obj, const QString& name )
{
	m_object = obj;
	m_constObject = obj;
	m_name = name;
}

Property::Property( const Object *obj, const QString& name )
{
	m_object = 0;
	m_constObject = obj;
	m_name = name;
}

QVariant::Type Property::type() const
{
	return m_constObject->classInfo()->property( m_name )->type();
}

QVariant Property::value() const
{
	return m_constObject->propertyValue( m_name.ascii() );
}

void Property::setValue( const QVariant& value )
{
	if ( m_object->setProperty( m_name, value ) )
		m_object->setModified( true );
}

const QString& Property::name() const
{
	return m_name;
}

bool Property::readOnly() const
{
	return m_constObject->classInfo()->property( m_name )->readOnly();
}

// PropertiesIterator

PropertiesIterator::PropertiesIterator( Object *object, PropertiesInfoConstIterator it )
{
	m_object = object;
	m_it = it;
}

Property PropertiesIterator::data()
{
	return Property( m_object, (*m_it)->name() );
}
const Property PropertiesIterator::data() const
{
	return Property( m_object, (*m_it)->name() );
}
PropertiesIterator& PropertiesIterator::operator++()
{
	m_it++;
	return *this;
}
PropertiesIterator& PropertiesIterator::operator--()
{
	m_it--;
	return *this;
}
PropertiesIterator PropertiesIterator::operator++(int)
{
	PropertiesIterator tmp = *this;
	m_it++;
	return tmp;
}
PropertiesIterator PropertiesIterator::operator--(int)
{
	PropertiesIterator tmp = *this;
	m_it--;
	return tmp;
}
bool PropertiesIterator::operator==( const PropertiesIterator& it ) const
{
	return m_it == it.m_it && m_object == it.m_object;
}
bool PropertiesIterator::operator!=( const PropertiesIterator& it ) const
{
	return m_it != it.m_it || m_object != it.m_object;
}
Property PropertiesIterator::operator*()
{
	return Property( m_object, (*m_it)->name() );
}
const Property PropertiesIterator::operator*() const
{
	return Property( m_object, (*m_it)->name() );
}

PropertiesIterator& PropertiesIterator::operator=(const PropertiesIterator& it)
{
	m_object = it.m_object;
	m_it = it.m_it;
	return *this;
}


// PropertiesConstIterator

PropertiesConstIterator::PropertiesConstIterator( const Object *object, PropertiesInfoConstIterator it ) 
{
	m_object = object;
	m_it = it;
}
	
const Property PropertiesConstIterator::data() const
{
	return Property( m_object, (*m_it)->name() );
}

PropertiesConstIterator& PropertiesConstIterator::operator++()
{
	m_it++;
	return *this;
}
PropertiesConstIterator& PropertiesConstIterator::operator--()
{
	m_it--;
	return *this;
}
PropertiesConstIterator PropertiesConstIterator::operator++(int)
{
	PropertiesConstIterator tmp = *this;
	m_it++;
	return tmp;
}
PropertiesConstIterator PropertiesConstIterator::operator--(int)
{
	PropertiesConstIterator tmp = *this;
	m_it--;
	return tmp;
}
bool PropertiesConstIterator::operator==( const PropertiesConstIterator& it ) const
{
	return m_it == it.m_it && m_object == it.m_object;
}
bool PropertiesConstIterator::operator!=( const PropertiesConstIterator& it ) const
{
	return m_it != it.m_it || m_object != it.m_object;
}
const Property PropertiesConstIterator::operator*() const
{
	return Property( m_object, (*m_it)->name() );
}
PropertiesConstIterator& PropertiesConstIterator::operator=(const PropertiesConstIterator& it)
{
	m_object = it.m_object;
	m_it = it.m_it;
	return *this;
}

// ObjectsIterator

ObjectsIterator::ObjectsIterator( const OidType& oid, RelationInfosConstIterator it, Manager* manager )
{
	m_oid = oid;
	m_it = it;
	m_manager = manager;
}

Object* ObjectsIterator::data()
{
	return m_manager->load( m_manager->relation( m_oid, *m_it ), (*m_it)->relatedClassInfo() );
}

const Object* ObjectsIterator::data() const
{
	return m_manager->load( m_manager->relation( m_oid, *m_it ), (*m_it)->relatedClassInfo() );
}

QString ObjectsIterator::key()
{
	return m_it.key();
}

const QString& ObjectsIterator::key() const
{
	return m_it.key();
}

const RelationInfo* ObjectsIterator::relatedObject() const
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
	return m_manager->load( m_manager->relation( m_oid, *m_it ), (*m_it)->relatedClassInfo() );
}

const Object* ObjectsIterator::operator*() const
{
	return m_manager->load( m_manager->relation( m_oid, *m_it ), (*m_it)->relatedClassInfo() );
}

ObjectsIterator& ObjectsIterator::operator=(const ObjectsIterator& it)
{
	m_it = it.m_it;
	m_oid = it.m_oid;
	return *this;
}

// CollectionsIterator

CollectionsIterator::CollectionsIterator( const OidType& oid, CollectionInfosConstIterator it, Manager* manager )
{
	m_oid = oid;
	m_it = it;
	m_manager = manager;
}

Collection* CollectionsIterator::data()
{
	return m_manager->collection( m_oid, *m_it );
}

const Collection* CollectionsIterator::data() const
{
	return m_manager->collection( m_oid, *m_it );
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
	PropertiesIterator it( propertiesBegin() );
	PropertiesIterator end( propertiesEnd() );
	for ( ; it != end; ++it )
		if ( (*it).readOnly() == false )
			(*it).setValue( obj.property( (*it).name() ).value() );
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

/*!
With this function you can force the ClassInfo of the object. It has been introduced for dynamic objects as they don't know which object they are emulating at a given moment
*/
void Object::setClassInfo( const ClassInfo* info )
{
	m_classInfo = info;
}
/*
const ClassInfo* Object::classInfo()
{
	if ( m_classInfo )
		return m_classInfo;
	m_classInfo = Classes::classInfo( className() );
	return m_classInfo;
}
*/
const ClassInfo* Object::classInfo() const
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

void Object::reset()
{
	PropertiesIterator it( propertiesBegin() );
	PropertiesIterator end( propertiesEnd() );
	Property p;
	for ( ; it != end; ++it ) {
		p = it.data();
		switch ( p.type() ) {
			case QVariant::Int:
			case QVariant::UInt:
			case QVariant::LongLong:
			case QVariant::ULongLong:
				p.setValue( 0 );
				break;
			case QVariant::String:
				p.setValue( QString::null );
			default:
				break;
		}
	}
}

/*
Functions for managing the properties
*/
PropertiesIterator Object::propertiesBegin()
{
	return PropertiesIterator( this, classInfo()->propertiesBegin() );
}

PropertiesConstIterator Object::propertiesBegin() const
{
	return PropertiesConstIterator( this, classInfo()->propertiesBegin() );
}

PropertiesConstIterator Object::propertiesConstBegin() const
{
	return PropertiesConstIterator( this, classInfo()->propertiesBegin() );
}

PropertiesIterator Object::propertiesEnd()
{
	return PropertiesIterator( this, classInfo()->propertiesEnd() );
}

PropertiesConstIterator Object::propertiesEnd() const
{
	return PropertiesConstIterator( this, classInfo()->propertiesEnd() );
}

PropertiesConstIterator Object::propertiesConstEnd() const
{
	return PropertiesConstIterator( this, classInfo()->propertiesEnd() );
}

int Object::numProperties() const
{
	return classInfo()->numProperties();
}

Property Object::property( const QString& name )
{
	return Property( this, name );
}

const Property Object::property( const QString& name ) const
{
	return Property( this, name );
}

bool Object::containsProperty( const QString& name ) const
{
	return classInfo()->containsProperty( name );
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
	return ObjectsIterator( m_oid, classInfo()->relationsBegin(), m_manager );
}

ObjectsIterator Object::objectsEnd()
{
	return ObjectsIterator( m_oid, classInfo()->relationsEnd(), m_manager );
}

int Object::numObjects() const
{
	return classInfo()->numObjects();
}

Object* Object::object( const QString& name ) const
{
	assert( containsObject( name ) );
	return m_manager->load( m_manager->relation( m_oid, classInfo()->object( name ) ), classInfo()->object( name )->relatedClassInfo() );
}

bool Object::containsObject( const QString& name ) const
{
	return classInfo()->containsObject( name );
}

void Object::setObject( const QString& name, Object* object )
{
	if ( object )
		setObject( name, object->oid() );
	else
		setObject( name, static_cast<OidType>( 0 ) );
}

void Object::setObject( const QString& name, const OidType& oid )
{
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
//	kdDebug() << " name = " << name << ", classInfo = " << classInfo()->name() << ", RELATION: " << relation << endl;
	assert( classInfo()->collection( relation ) );
	return m_manager->collection( m_oid, classInfo()->collection( relation ) );
}

#include "object.moc"
