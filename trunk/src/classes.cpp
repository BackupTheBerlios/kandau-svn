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

/* RelationInfo */

/*!
RelationInfo constructor. Note that the relation is browsable by default.
@param classInfo A pointer to the ClassInfo object the collection is in.
@param name The name of the relation (not the class of the related object)
@param function The function which creates an object of the type of the related one
*/
RelationInfo::RelationInfo( const ClassInfo *classInfo, const QString& name, CreateObjectFunction function )
{
	assert( classInfo );
	assert( function );
	m_parentClassInfo = classInfo;
	m_name = name;
	m_browsable = true;
	Object *obj = function();
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

/*!
Get the name of the relation
@return The name of the relation
*/
const QString& RelationInfo::name() const
{
	return m_name;
}

/*!
Get whether the relation is One to One or One to N.
@return True if the relation is One to One, false if it's One to N.
*/
bool RelationInfo::isOneToOne() const
{
	return m_oneToOne;
}

/*!
Gets the ClassInfo of the class of the objects related.
@return The ClassInfo pointer of the class.
*/
const ClassInfo* RelationInfo::relatedClassInfo() const
{
	return m_relatedClassInfo;
}

/*!
Gets the ClassInfo of the parent class
@return The ClassInfo pointer of the parent class
*/
const ClassInfo* RelationInfo::parentClassInfo() const
{
	return m_parentClassInfo;
}

/*!
Obtains whether the relation was designed to be browsable. That is, there is a
declaration in the the definition of the class parentClassInfo() that points to
relatedClassInfo(). Otherwise, the relation exists only in the declaration from
relatedClassInfo().
@return True if it was designed to be browsable, false otherwise.
*/
bool RelationInfo::browsable() const
{
	return m_browsable;
}

/*!
Establishes whether the relation is 1-1 or 1-N.
@param oneToOne True if the relation is 1-1, false if it's 1-N
*/
void RelationInfo::setOneToOne( bool oneToOne )
{
	m_oneToOne = oneToOne;
}

/*!
Establishes whether the relation is browsable or not.
@param browsable True if the relation is browsable, false otherwise.
*/
void RelationInfo::setBrowsable( bool browsable )
{
	m_browsable = browsable;
}

/* CollectionInfo */

/*!
CollectionInfo constructor. Note that it's browsable by default.
@param classInfo A pointer to the ClassInfo object the collection is in.
@param name The name of the relation (not the class of the related object)
@param children Pointer to the ClassInfo object which holds the type of the related objects.
@param nToOne Specifies if the relation is N to One, or N to N. This is a hint only and only needed if in the related class there is no declaration.
*/
CollectionInfo::CollectionInfo( const ClassInfo* parent, const QString& name, const ClassInfo* children, bool nToOne )
{
	m_parentClassInfo = parent;
	m_name = name;
	m_browsable = true;
	m_childrenClassInfo = children;
}

/*!
Get the name of the relation
@return The name of the relation
*/
const QString& CollectionInfo::name() const
{
	return m_name;
}

/*!
Get whether the relation is N to One or N to N.
@return True if the relation is N to One, false if it's N to N.
*/
bool CollectionInfo::isNToOne() const
{
	return m_childrenClassInfo->containsObject( m_name );
}

/*!
Gets the ClassInfo of the class of the objects related.
@return The name of the class.
*/
const ClassInfo* CollectionInfo::childrenClassInfo() const
{
	return m_childrenClassInfo;
}

/*!
Obtains the ClassInfo of the class of the parent
@return ClassInfo pointer of the class of the parent
*/
const ClassInfo* CollectionInfo::parentClassInfo() const
{
	return m_parentClassInfo;
}

/*!
Obtains whether the collection is browsable or not
@return True if the collection is browsable, false otherwise.
*/
bool CollectionInfo::browsable() const
{
	return m_browsable;
}

/*!
Establishes whether the relation is N-1 or N-M
@param nToOne True if the relation is N-1, false if it's N-M.
*/
void CollectionInfo::setNToOne( bool nToOne )
{
	m_nToOne = nToOne;
}

/*!
Establishes whether the relation is browsable or not.
@param browsable True if the relation is browsable, false otherwise.
*/
void CollectionInfo::setBrowsable( bool browsable )
{
	m_browsable = browsable;
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
	m_objects.insert( name, new RelationInfo( this, name, function ) );
}

void ClassInfo::addCollection( const QString& className, const QString& relationName, bool nToOne )
{
	QString name;

	if ( relationName.isNull() )
		name = ClassInfo::relationName( className, m_name );
	else
		name = relationName;

	assert( ! m_collections.contains( name ) );
	m_collections.insert( name, new CollectionInfo( this, name, Classes::classInfo( className ), nToOne ) );
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
		// TODO: Improve enums support.
		PropertyInfo *p;
		if ( meta->isEnumType() )
			p = new PropertyInfo( meta->name(),  QVariant::ULongLong, ! meta->writable() );
		else
			p = new PropertyInfo( meta->name(),  QVariant::nameToType( meta->type() ), ! meta->writable() );
		m_properties.insert( meta->name(), p );
	}
}

RelationInfosConstIterator ClassInfo::relationsBegin() const
{
	return m_objects.begin();
}

RelationInfosConstIterator ClassInfo::relationsEnd() const
{
	return m_objects.end();
}

RelationInfosIterator ClassInfo::relationsBegin()
{
	return m_objects.begin();
}

RelationInfosIterator ClassInfo::relationsEnd()
{
	return m_objects.end();
}

bool ClassInfo::containsObject( const QString& name ) const
{
	return m_objects.contains( name );
}

RelationInfo* ClassInfo::object( const QString& name ) const
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

CollectionInfosConstIterator ClassInfo::collectionsBegin() const
{
	return m_collections.begin();
}

CollectionInfosConstIterator ClassInfo::collectionsEnd() const
{
	return m_collections.end();
}

CollectionInfosIterator ClassInfo::collectionsBegin()
{
	return m_collections.begin();
}

CollectionInfosIterator ClassInfo::collectionsEnd()
{
	return m_collections.end();
}

bool ClassInfo::containsCollection( const QString& name ) const
{
	return m_collections.contains( name );
}

CollectionInfo* ClassInfo::collection( const QString& name ) const
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
/*!
Ends all the configuration required for browsing through class relations.
Should be used at the very begining of each application.
*/
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

	setupRelations();
}

/*!
Called by Classes::setup(). You should make sure you call it after creating new classes
(using DynamicObjects).
*/
void Classes::setupRelations()
{
	// Browse all relations and collections.
	// And add new non-browsable collections where needed.
	QValueVector<QStringList> list;

	ClassInfoConstIterator it2( Classes::begin() );
	ClassInfoConstIterator end2( Classes::end() );
	ClassInfo *classInfo;
	RelationInfo *relInfo;
	for ( ; it2 != end2; ++it2 ) {
		classInfo = it2.data();
		RelationInfosConstIterator rit( classInfo->relationsBegin() );
		RelationInfosConstIterator rend( classInfo->relationsEnd() );
		for ( ; rit != rend; ++rit ) {
			relInfo = rit.data();
			if ( relInfo->relatedClassInfo()->containsObject( relInfo->name() ) ) {
				relInfo->setOneToOne( true );
			} else {
				relInfo->setOneToOne( false );
				if ( ! relInfo->relatedClassInfo()->containsCollection( relInfo->name() ) ) {
					QStringList t;
					t << relInfo->relatedClassInfo()->name();
					t << relInfo->parentClassInfo()->name();
					t << relInfo->name();
					t << "true";
					list.append( t );
				}
			}
		}
		CollectionInfosConstIterator cit( classInfo->collectionsBegin() );
		CollectionInfosConstIterator cend( classInfo->collectionsEnd() );
		CollectionInfo *colInfo;
		for ( ; cit != cend; ++cit ) {
			colInfo = cit.data();
			if ( colInfo->childrenClassInfo()->containsObject( colInfo->name() ) ) {
				colInfo->setNToOne( true );
			} else {
				colInfo->setNToOne( false );
				if ( ! colInfo->childrenClassInfo()->containsCollection( colInfo->name() ) ) {
					QStringList t;
					t << colInfo->childrenClassInfo()->name();
					t << colInfo->parentClassInfo()->name();
					t << colInfo->name();
					t << "false";
					list.append( t );
				}
			}
		}
	}
	for ( uint i = 0; i < list.count(); ++i ) {
		QStringList str = list[ i ];
		ClassInfo *t = Classes::classInfo( str[0] );
		t->addCollection( str[1], str[2] );
		t->collection( str[2] )->setBrowsable( false );
		t->collection( str[2] )->setNToOne( str[3] == "true" );
	}
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
