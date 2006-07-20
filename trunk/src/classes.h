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
#include <qvaluevector.h>

#include "oidtype.h"
#include "defs.h"

class QObject;
class Object;
class ClassInfo;
class Manager;
class Classes;


/**
@author Albert Cervera Areny
*/

/*!
\brief This class contains information about a class' property.

The most obvious meta-information about a class' property are name and type. But it
also contains wheather it's read-only or read-write, and inherited or not.

\see ClassInfo
*/
class PropertyInfo
{
public:
	PropertyInfo();
	PropertyInfo( const QString& name, QVariant::Type type, bool readOnly, bool inherited );
	QVariant::Type type() const;
	const QString& name() const;
	bool readOnly() const;
	bool inherited() const;
	bool isSetType() const;
	bool isEnumType() const;
	QStringList enumKeys() const;

	void setType( QVariant::Type type );
	void setName( const QString& name );
	void setReadOnly( bool value );
	void setInherited( bool value );
	void setSetType( bool value );
	void setEnumType( bool value );
	void addKeyAndValue( const QString& key, int value );
	int keyToValue ( const QString& key ) const;
	const QString& valueToKey ( int value ) const;
	int keysToValue ( const QStringList& keys ) const;
	QStringList valueToKeys ( int value ) const;

private:
	class KeyAndValue 
	{
	public:
		KeyAndValue() : m_value( 0 ) {}
		KeyAndValue( const QString& key, int value ) :
			m_key( key ), 
			m_value( value ) 
			{}
		QString m_key;
		int m_value;
	};

	QString m_name;
	QVariant::Type m_type;
	bool m_readOnly;
	bool m_inherited;
	bool m_enumType;
	bool m_setType;
	QValueVector<KeyAndValue> m_enums;
};


/*!
\brief This class contains information about a class' relation.

Stores the information of a relation to a class. Mainly stores the name 
of the relation and a pointer to a function that allows the creation of 
an object of the related class type.

The constructor is only called from the addObject function in ClassInfo which is called in the OBJECT macro in object.h

\see ClassInfo
*/
class RelationInfo
{
public:
	RelationInfo( const ClassInfo *classInfo, const QString& name, CreateObjectFunction function );
	const QString& name() const;
	bool isOneToOne() const;
	const ClassInfo* relatedClassInfo() const;
	const ClassInfo* parentClassInfo() const;
	bool browsable() const;
	bool inherited() const;

private:
	// This allows Classes to call setBrowsable & setOneToOne
	friend class Classes;

	void setOneToOne( bool oneToOne );
	void setBrowsable( bool browsable );
	void setInherited( bool inherited );

	QString m_name;
	bool m_oneToOne;
	bool m_browsable;
	bool m_inherited;

	// Pointer to the parent ClassInfo
	const ClassInfo *m_parentClassInfo;

	// Contains the cached ClassInfo of the related class
	const ClassInfo *m_relatedClassInfo;
};



/*!
\brief This class contains information about a class' collection.

Stores the information of a relation to collection of objects. Mainly stores the 
name of the relation and a pointer to a function that allows the creation of 
an object of the related class type.

The constructor is only called from the addCollection function in ClassInfo which is called by COLLECTION and COLLECTIONN macros in object.h

\see ClassInfo
*/
class CollectionInfo
{
public:
	CollectionInfo( const ClassInfo *parent, const QString& name, const ClassInfo *children, bool nToOne );
	const QString& name() const;
	bool isNToOne() const;
	const ClassInfo* childrenClassInfo() const;
	const ClassInfo* parentClassInfo() const;
	bool browsable() const;
	bool inherited() const;

private:
	// This allows Classes to call setBrowsable & setNToOne
	friend class Classes;

	void setNToOne( bool nToOne );
	void setBrowsable( bool browsable );
	void setInherited( bool inherited );

	/*!
	Name of the relation
	*/
	QString m_name;

	/*!
	Pointer to the parent ClassInfo
	*/
	const ClassInfo *m_parentClassInfo;

	/*!
	Contains the cached ClassInfo of the related class
	*/
	const ClassInfo *m_childrenClassInfo;

	/*!
	Contains whether the relation is N-1 or N-M.
	*/
	bool m_nToOne;
	/*!
	Contains whether the collection is browsable or not
	*/
	bool m_browsable;
	bool m_inherited;
};


typedef QMap<QString,PropertyInfo*> PropertiesInfo;
typedef QMapIterator<QString,PropertyInfo*> PropertiesInfoIterator;
typedef QMapConstIterator<QString,PropertyInfo*> PropertiesInfoConstIterator;

typedef QMap<QString,RelationInfo*> RelationInfos;
typedef QMapIterator<QString,RelationInfo*> RelationInfosIterator;
typedef QMapConstIterator<QString,RelationInfo*> RelationInfosConstIterator;

typedef QMap<QString,CollectionInfo*> CollectionInfos;
typedef QMapIterator<QString,CollectionInfo*> CollectionInfosIterator;
typedef QMapConstIterator<QString,CollectionInfo*> CollectionInfosConstIterator;

typedef QValueList<const ClassInfo*> ChildrenInfo;
typedef QValueListConstIterator<const ClassInfo*> ChildrenInfoConstIterator;


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

typedef QMap<QString, ClassInfo*> ClassInfoMap;
typedef QMapIterator<QString, ClassInfo*> ClassInfoIterator;
typedef QMapConstIterator<QString, ClassInfo*> ClassInfoConstIterator;


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

typedef QMap<QString, TmpClass*> TmpClassMap;
typedef QMapIterator<QString, TmpClass*> TmpClassIterator;
typedef QMapConstIterator<QString, TmpClass*> TmpClassConstIterator;


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
