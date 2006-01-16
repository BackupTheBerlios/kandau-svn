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
#include <kdebug.h>

#include "classes.h"
#include "object.h"

ClassInfoMap *Classes::m_classes = 0;
ClassInfo* Classes::m_currentClass = 0;
TmpClassMap* Classes::m_tmpClasses = 0;

/* PropertyInfo */

PropertyInfo::PropertyInfo( const QString& name, QVariant::Type type, bool readOnly )
{
	m_name = name;
	m_type = type;
	m_readOnly = readOnly;
}

QVariant::Type PropertyInfo::type() const
{
	return m_type;
}

const QString& PropertyInfo::name() const
{
	return m_name;
}

bool PropertyInfo::readOnly() const
{
	return m_readOnly;
}

/* RelatedObject */

/*
RelatedObject::RelatedObject()
{
}
*/

RelatedObject::RelatedObject( const ClassInfo *classInfo, const QString& name, CreateObjectFunction function )
{
	assert( classInfo );
	assert( function );
	m_parentClassInfo = classInfo;
	m_name = name;
	m_function = function;
	
	Object *obj = m_function();
	assert( obj );
	m_relatedClassInfo = obj->classInfo();
	// TODO: We don't check if the type of the collection is of the same type of our class. Maybe this check could be added when compiled with the DEBUG flag.
	if ( m_relatedClassInfo->containsObject( m_name ) ) {
		m_oneToOne = true;
	} else {
		m_oneToOne = false;
	}
	delete obj;
}

const QString& RelatedObject::name() const
{
	return m_name;
}

CreateObjectFunction RelatedObject::createObjectFunction() const
{
	return m_function;
}

bool RelatedObject::isOneToOne() const
{
	return m_oneToOne;
}

const ClassInfo* RelatedObject::relatedClassInfo() const
{
	return m_relatedClassInfo;
}

const ClassInfo* RelatedObject::parentClassInfo() const
{
	return m_parentClassInfo;
}

/* RelatedCollection */

RelatedCollection::RelatedCollection()
{
}

RelatedCollection::RelatedCollection( const ClassInfo* parent, const QString& name, const ClassInfo* children, bool nToOne )
{
	m_parentClassInfo = parent;
	m_name = name;
	//m_function = function;
	m_childrenClassInfo = children;
}

const QString& RelatedCollection::name() const
{
	return m_name;
}

bool RelatedCollection::isNToOne() const
{
	return m_childrenClassInfo->containsObject( m_name );
}

const ClassInfo* RelatedCollection::childrenClassInfo() const
{
	return m_childrenClassInfo;
}

const ClassInfo* RelatedCollection::parentClassInfo() const
{
	return m_parentClassInfo;
}

/* ClassInfo */

ClassInfo::ClassInfo( const QString& name, CreateObjectFunction function )
{
	m_name = name;
	m_function = function;
}

Object* ClassInfo::create( Manager* manager ) const
{
	Object *object = m_function();
	assert( object );
	// This has been introduced for DynamicObject to let them know which 
	// ClassInfo they should use.
	object->setClassInfo( this );
	// ^^^
	object->setModified( true );
	object->setManager( manager );
	object->manager()->add( object );
	return object;
}

Object* ClassInfo::create( const OidType& oid, Manager* manager, bool create ) const
{
	if ( create ) {
		Object *object = m_function();
		assert( object );
		// This has been introduced for DynamicObject to let them know which 
		// ClassInfo they should use.
		object->setClassInfo( this );
		// ^^^
		object->setOid( oid );
		object->setModified( true );
		object->setManager( manager );
		object->manager()->add( object );
		return object;
	} else {
		if ( manager )
			return manager->load( oid, m_function );
		else
			return Manager::self()->load( oid, m_function );
	}
}

Object* ClassInfo::createInstance() const
{
	return m_function();
}

CreateObjectFunction ClassInfo::createObjectFunction() const
{
	return m_function;
}

const QString& ClassInfo::name() const
{
	return m_name;
}

void ClassInfo::addObject( const QString& className, const QString& relationName, CreateObjectFunction function )
{
	QString name;

	if ( relationName.isNull() )
		name = ClassInfo::relationName( className, m_name );
	else
		name = relationName;

	assert( ! m_objects.contains( name ) );
	assert( function );
	m_objects.insert( name, new RelatedObject( this, name, function ) );
}

void ClassInfo::addCollection( const QString& className, const QString& relationName, bool nToOne )
{
	QString name;

	if ( relationName.isNull() )
		name = ClassInfo::relationName( className, m_name );
	else
		name = relationName;

	assert( ! m_collections.contains( name ) );
	m_collections.insert( name, new RelatedCollection( this, name, Classes::classInfo( className ), nToOne ) );
}

void ClassInfo::addProperty( const QString& name, QVariant::Type type, bool readOnly )
{
	m_properties.insert( name, new PropertyInfo( name, type, readOnly ) );
}

void ClassInfo::createProperties()
{
	// Fill in the Properties Map
	Object* obj = createInstance();
	for ( int i = 0; i < obj->metaObject()->numProperties(); ++i ) {
		const QMetaProperty *meta = obj->metaObject()->property( i );
		PropertyInfo *p = new PropertyInfo( meta->name(),  QVariant::nameToType( meta->type() ), ! meta->writable() );
		m_properties.insert( meta->name(), p );
	}
}

RelatedObjectsConstIterator ClassInfo::objectsBegin() const
{
	return m_objects.begin();
}

RelatedObjectsConstIterator ClassInfo::objectsEnd() const
{
	return m_objects.end();
}

RelatedObjectsIterator ClassInfo::objectsBegin()
{
	return m_objects.begin();
}

RelatedObjectsIterator ClassInfo::objectsEnd()
{
	return m_objects.end();
}

bool ClassInfo::containsObject( const QString& name ) const
{
	return m_objects.contains( name );
}

RelatedObject* ClassInfo::object( const QString& name ) const
{
	return m_objects[ name ];
}

int ClassInfo::objectsCount() const
{
	return m_objects.count();
}

int ClassInfo::numObjects() const
{
	return m_objects.count();
}

RelatedCollectionsConstIterator ClassInfo::collectionsBegin() const
{
	return m_collections.begin();
}

RelatedCollectionsConstIterator ClassInfo::collectionsEnd() const
{
	return m_collections.end();
}

RelatedCollectionsIterator ClassInfo::collectionsBegin()
{
	return m_collections.begin();
}

RelatedCollectionsIterator ClassInfo::collectionsEnd()
{
	return m_collections.end();
}

bool ClassInfo::containsCollection( const QString& name ) const
{
	return m_collections.contains( name );
}

RelatedCollection* ClassInfo::collection( const QString& name ) const
{
	return m_collections[ name ];
}

int ClassInfo::collectionsCount() const
{
	return m_collections.count();
}

int ClassInfo::numCollections() const
{
	return m_collections.count();
}

QString ClassInfo::relationName( const QString& relation, const QString& className )
{
	if ( ! Classes::contains( relation ) )
		return relation;

	QStringList list;
	list << relation;
	list << className;
	list.sort();
	return list.join( "_" );
}


PropertiesInfoConstIterator ClassInfo::propertiesBegin() const
{
	return m_properties.begin();
}

PropertiesInfoConstIterator ClassInfo::propertiesEnd() const
{
	return m_properties.end();
}

const PropertyInfo* ClassInfo::property( const QString& name ) const
{
	return m_properties[name];
}

bool ClassInfo::containsProperty( const QString& name ) const
{
	return m_properties.contains( name );
}

uint ClassInfo::numProperties() const
{
	return m_properties.count();
}

void ClassInfo::addMetaInfo( const QString& name, QObject *object )
{
	m_metaInfo.insert( name, object );
}

QObject* ClassInfo::metaInfo( const QString& name ) const
{
	if ( m_metaInfo.contains( name ) ) 
		return m_metaInfo[ name ];
	else
		return 0;
}

/* TmpClass */

TmpClass::TmpClass( const QString &name, CreateRelationsFunction createRelations )
{
	m_name = name;
	m_createRelations = createRelations;
}

const QString& TmpClass::name() const
{
	return m_name;
}

CreateRelationsFunction TmpClass::createRelations() const
{
	return m_createRelations;
}

/* Classes */

void Classes::setup()
{
	// If m_tmpClasses has not been initialized probably setup() has
	// already been called.
	if ( m_tmpClasses == 0 )
		return;

	TmpClassConstIterator it( m_tmpClasses->constBegin() );
	TmpClassConstIterator end( m_tmpClasses->constEnd() );
	TmpClass *tmp;
	for ( ; it != end; ++it ) {
		tmp = (*it);
		m_currentClass = Classes::classInfo( tmp->name() );
		m_currentClass->createProperties();
		tmp->createRelations()();
		delete tmp;
		tmp = 0;
	}
	delete m_tmpClasses;
	m_tmpClasses = 0;
}

void Classes::addClass( const QString &name, CreateObjectFunction createInstance, CreateRelationsFunction createRelations )
{
	if ( ! m_classes ) {
		m_classes = new ClassInfoMap();
		m_tmpClasses = new TmpClassMap();
	}
	m_classes->insert( name, new ClassInfo( name, createInstance ) );
	if ( m_tmpClasses )
		m_tmpClasses->insert( name, new TmpClass( name, createRelations ) );
}

ClassInfoIterator Classes::begin()
{
	if ( ! m_classes )
		m_classes = new ClassInfoMap();

	return m_classes->begin();
}

ClassInfoIterator Classes::end()
{
	 if ( ! m_classes )
		m_classes = new ClassInfoMap();

	return m_classes->end();
}

bool Classes::contains( const QString& name )
{
	return m_classes->contains( name );
}

ClassInfo* Classes::classInfo( const QString& name )
{
	return (*m_classes)[ name ];
}

ClassInfo* Classes::currentClass()
{
	return m_currentClass;
}
