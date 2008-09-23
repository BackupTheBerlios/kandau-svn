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
#ifndef CLASSES_H
#define CLASSES_H

#include <qmap.h>
#include <QVector>
#include <QList>

#include "oidtype.h"
#include "defs.h"

/**
@author Albert Cervera Areny
*/

class RelationInfo;

class PropertyInfo;

class CollectionInfo;

/*

typedef QMap<QString,RelationInfo*> RelationInfos;
typedef QMapIterator<QString,RelationInfo*> RelationInfosIterator;
typedef QMutableMapIterator<QString,RelationInfo*> RelationInfosConstIterator;

// WARNING duplicate code see property.h
typedef QMap<QString,PropertyInfo*> PropertiesInfo;
typedef QMapIterator<QString,PropertyInfo*> PropertiesInfoIterator;
typedef QMutableMapIterator<QString,PropertyInfo*> PropertiesInfoConstIterator;

typedef QMap<QString,CollectionInfo*> CollectionInfos;
typedef QMapIterator<QString,CollectionInfo*> CollectionInfosIterator;
typedef QMutableMapIterator<QString,CollectionInfo*> CollectionInfosConstIterator;

typedef QList<const ClassInfo*> ChildrenInfo;
typedef QListIterator<const ClassInfo*> ChildrenInfoIterator;
typedef QMutableListIterator<const ClassInfo*> ChildrenInfoConstIterator;
*/

class QObject;
class Object;
class ClassInfo;
class Manager;
class Classes;

class CollectionInfo;

typedef QMap<QString,PropertyInfo*> PropertiesInfo;
typedef QMap<QString,PropertyInfo*>::iterator PropertiesInfoIterator;
typedef QMap<QString,PropertyInfo*>::const_iterator PropertiesInfoConstIterator;

typedef QMap<QString,RelationInfo*> RelationInfos;
typedef QMap<QString,RelationInfo*>::iterator RelationInfosIterator;
typedef QMap<QString,RelationInfo*>::const_iterator RelationInfosConstIterator;

typedef QMap<QString,CollectionInfo*> CollectionInfos;
typedef QMap<QString,CollectionInfo*>::iterator CollectionInfosIterator;
typedef QMap<QString,CollectionInfo*>::const_iterator CollectionInfosConstIterator;

typedef QList<const ClassInfo*> ChildrenInfo;
typedef QList<const ClassInfo*>::iterator ChildrenInfoIterator;
typedef QList<const ClassInfo*>::const_iterator ChildrenInfoConstIterator;

/*!
\brief This class contains information regarding the structure of a class, its properties and relations.

ClassInfo provides functions to iterate through a class' properties and relations
without a need to instantiate an object of the desired type.

To create an instance of a class you may choose between let the Manager held the 
the reference, and thus be managed by the persistency system, or manage the reference
yourself.

To create a persistent object you could: 

\code
ClassInfo *info = Classes::classInfo( "MyObject" );
if ( info ) {
	ObjectRef<Object> obj = info->create();
	...
	Manager::self()->commit();
}
\endcode

If you want a non-persistent object, you'll have to manage the pointer yourself:
\code
ClassInfo *info = Classes::classInfo( "MyObject" );
if ( info ) {
	Object* obj = info->createInstance();
	...
	delete obj;
}
\endcode

If you wish to create new classes dynamically without a need for a physical C++ 
object, refer to DynamicObject class documentation or the Kandau manual.

\see Classes
*/
class ClassInfo
{
public:
	ClassInfo( const QString& name, CreateObjectFunction function );

	Object* create( Manager* manager = 0 ) const;
	Object* create( const OidType& oid, Manager* manager = 0, bool create = false ) const;

	Object* createInstance() const;

	CreateObjectFunction createObjectFunction() const;

	void addObject( const QString& className, const QString& relationName, CreateObjectFunction function );

	void addCollection( const QString& className, const QString& relationName, bool nToOne = true);
	//void addProperty( const QString& name, QVariant::Type type, bool readOnly = false, bool inherited = false );
	void addProperty( PropertyInfo* property );

	void createProperties();

	const QString& name() const;

	RelationInfosConstIterator relationsBegin() const;
	RelationInfosConstIterator relationsEnd() const;
	RelationInfosIterator relationsBegin();
	RelationInfosIterator relationsEnd();

	bool containsObject( const QString& name ) const;
	RelationInfo* object( const QString& name ) const;

	int objectsCount() const;

	int numObjects() const;

	CollectionInfosConstIterator collectionsBegin() const;
	CollectionInfosConstIterator collectionsEnd() const;

	CollectionInfosIterator collectionsBegin();
	CollectionInfosIterator collectionsEnd();

	bool containsCollection( const QString& name ) const;

	CollectionInfo* collection( const QString& name ) const;

	int collectionsCount() const;

	int numCollections() const;

	const ClassInfo* parent() const;
	void setParent( const ClassInfo* parent );
	const ChildrenInfo& children() const;
	void addChild( const ClassInfo* child );
	bool inherits( const QString& className ) const;
	bool inherits( const ClassInfo* classInfo ) const;
	QStringList ancestors() const;

	static QString relationName( const QString& relation, const QString& className );

	PropertiesInfoConstIterator propertiesBegin() const;
	PropertiesInfoConstIterator propertiesEnd() const;
	const PropertyInfo* property( const QString& name ) const;
	bool containsProperty( const QString& name ) const;
	uint numProperties() const;

	void addMetaInfo( const QString& name, QObject *object );
	QObject* metaInfo( const QString& name ) const;

private:
	const ClassInfo *m_parent;
	ChildrenInfo m_children;
	QString m_name;
	CreateObjectFunction m_function;

	RelationInfos m_objects;
	CollectionInfos m_collections;
	PropertiesInfo m_properties;
	QMap<QString,QObject*> m_metaInfo;
};

/*
typedef QMap<QString, ClassInfo*> ClassInfoMap;
typedef QMapIterator<QString, ClassInfo*> ClassInfoIterator;
typedef QMutableMapIterator<QString, ClassInfo*> ClassInfoConstIterator;
*/

typedef QMap<QString, ClassInfo*> ClassInfoMap;
typedef QMap<QString, ClassInfo*>::iterator ClassInfoIterator;
typedef QMap<QString, ClassInfo*>::const_iterator ClassInfoConstIterator;

class TmpClass
{
public:
	TmpClass( const QString &name, CreateRelationsFunction createRelations );
	const QString& name() const;
	CreateRelationsFunction createRelations() const;

private:
	QString m_name;
	CreateRelationsFunction m_createRelations;
};

/*
typedef QMap<QString, TmpClass*> TmpClassMap;
typedef QMapIterator<QString, TmpClass*> TmpClassIterator;
typedef QMutableMapIterator<QString, TmpClass*> TmpClassConstIterator;
*/

typedef QMap<QString, TmpClass*> TmpClassMap;
typedef QMap<QString, TmpClass*>::iterator TmpClassIterator;
typedef QMap<QString, TmpClass*>::const_iterator TmpClassConstIterator;

/*!
@brief This class provides the starting point for class introspection within Kandau.

Kandau introspection mechanism allows the programmer to access properties and 
relations associated to a class. The class has to have been properly defined by
inheriting Object, and needs to have the DCLASS( ClassName ) and ICLASS( ClassName )
declarations.

Before starting to use the introspection facilities, Classes::setup() has to be called,
and thus should be one of the first function calls in an application.

After that, one can iterate through the list of classes or check their availability.

\see ClassInfo
*/
class Classes
{
public:

	static void setup();
	static void setupRelations();
	static void setupHierarchy();
	static void setupRelationsHierarchy();
	static QStringList parentsFirst();

	static void addClass( const QString& name, CreateObjectFunction createInstance, CreateRelationsFunction createRelations );

	static ClassInfoIterator begin();
	static ClassInfoIterator end();

	static ClassInfoConstIterator constBegin();
	static ClassInfoConstIterator constEnd();

	static bool contains( const QString& name );

	static ClassInfo* classInfo( const QString& name );

	static ClassInfo* currentClass();

private:
	/*
	This map contains the names of the classes and the pointers to the functions
	that can create an instance.
	*/
	static ClassInfoMap *m_classes;

	/*
	This var points to the class that is being created at the moment
	*/
	static ClassInfo* m_currentClass;

	/*
	This map contains the temporary information to create relations and labels
	*/
	static TmpClassMap *m_tmpClasses;
};

/*!
This class is a hack to declare a given class in order to be able to refer
to it later by its name (string).
The normal way of calling this class is using the DCLASS macro from object.h
*/
class DeclareClass
{
public:
	/*!
	@param name The name of the class.
	@param createInstance The pointer to the function that allows the creation of objects of the type class given by name.
	@param createRelations The pointer to the function that creates the relations of the class.
	*/
	DeclareClass( const QString& name, CreateObjectFunction createInstance, CreateRelationsFunction createRelations )
	{
		Classes::addClass( name, createInstance, createRelations );
	}
};

#endif
