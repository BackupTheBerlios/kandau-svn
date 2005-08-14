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
#include <kdebug.h>

#include "classes.h"
#include "object.h"
#include "labels.h"

ClassInfoMap *Classes::m_classes = 0;
ClassInfo* Classes::m_currentClass = 0;
TmpClassMap* Classes::m_tmpClasses = 0;

/* RelatedObject */

RelatedObject::RelatedObject()
{
	m_cached = false;
}

RelatedObject::RelatedObject( ClassInfo *classInfo, const QString& name, CreateObjectFunction function )
{
	m_parentClassInfo = classInfo;
	m_name = name;
	m_function = function;
	m_cached = false;
}

const QString& RelatedObject::name() const
{
	return m_name;
}

CreateObjectFunction RelatedObject::createObjectFunction() const
{
	return m_function;
}

bool RelatedObject::isOneToOne()
{
	if ( ! m_cached )
		cacheData();
	return m_oneToOne;
}

ClassInfo* RelatedObject::relatedClassInfo()
{
	if ( ! m_cached )
		cacheData();
	return m_relatedClassInfo;
}

void RelatedObject::cacheData()
{
	assert( m_function );
	Object *obj = m_function();
	assert( obj );
	m_relatedClassInfo = obj->classInfo();
	// TODO: We don't check if the type of the collection is of the same type of our class. Maybe this check could be added when compiled with the DEBUG flag.

	if ( m_relatedClassInfo->containsObject( m_name ) ) {
		m_oneToOne = true;
	} else {
		m_oneToOne = false;
	}

	m_cached = true;
	delete obj;
}


/* RelatedCollection */

RelatedCollection::RelatedCollection()
{
	m_cached = false;
	m_nToOne = true;
}

RelatedCollection::RelatedCollection( ClassInfo* parent, const QString& name, ClassInfo* children, bool nToOne )
{
	m_parentClassInfo = parent;
	m_name = name;
	//m_function = function;
	m_childrenClassInfo = children;
	m_nToOne = nToOne;
	m_cached = false;
}

const QString& RelatedCollection::name() const
{
	return m_name;
}

bool RelatedCollection::isNToOne()
{
	if ( ! m_cached )
		cacheData();
	return m_nToOne;
}

ClassInfo* RelatedCollection::childrenClassInfo()
{
	//if ( ! m_cached )
	//	cacheData();
	//return m_relatedClassInfo;
	return m_childrenClassInfo;
}

ClassInfo* RelatedCollection::parentClassInfo()
{
	return m_parentClassInfo;
}

void RelatedCollection::cacheData()
{
	//assert( m_function );
	//Object *obj = m_function();

	//m_relatedClassInfo = obj->classInfo();
	// TODO: We don't check if the type of the collection is of the same type of our class. Maybe this check could be added when compiled with the DEBUG flag.

	// Only if we haven't been explicitly told that it is N-to-N we will
	// search in the related class
	if ( m_nToOne ) {
		if ( m_childrenClassInfo->containsObject( m_name ) ) {
			m_nToOne = true;
		} else {
			m_nToOne = false;
		}
	}
	m_cached = true;
//	delete obj;
}


/* ClassInfo */

ClassInfo::ClassInfo( const QString& name, CreateObjectFunction function )
{
	m_name = name;
	m_function = function;
}

Object* ClassInfo::create( const OidType& oid, Manager* manager ) const
{
	Object *object = m_function();
	assert( object );
	object->setOid( oid );
	object->setModified( true );
	object->setManager( manager );
	object->manager()->add( object );
	return object;
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

/* TmpClass */

TmpClass::TmpClass( const QString &name, CreateRelationsFunction createRelations, CreateLabelsFunction createLabels )
{
	m_name = name;
	m_createRelations = createRelations;
	m_createLabels = createLabels;
}

const QString& TmpClass::name() const
{
	return m_name;
}

CreateRelationsFunction TmpClass::createRelations() const
{
	return m_createRelations;
}

CreateLabelsFunction TmpClass::createLabels() const
{
	return m_createLabels;
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
		tmp->createRelations()();
		Labels::setDefaultClass( tmp->name() );
		tmp->createLabels()();
		delete tmp;
		tmp = 0;
	}
	delete m_tmpClasses;
	m_tmpClasses = 0;
}

void Classes::addClass( const QString &name, CreateObjectFunction createInstance, CreateRelationsFunction createRelations, CreateLabelsFunction createLabels )
{
	if ( ! m_classes ) {
		m_classes = new ClassInfoMap();
		m_tmpClasses = new TmpClassMap();
	}
	m_classes->insert( name, new ClassInfo( name, createInstance ) );
	m_tmpClasses->insert( name, new TmpClass( name, createRelations, createLabels ) );
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
