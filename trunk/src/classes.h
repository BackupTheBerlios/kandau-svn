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

#include "oidtype.h"
#include "defs.h"

class QObject;
class Object;
class ClassInfo;
class Manager;

//typedef QMap<QString, Object *(*)(void)> ClassMap;
//typedef QMapIterator<QString, Object *(*)(void)> ClassIterator;
//typedef QMapConstIterator<QString, Object *(*)(void)> ClassConstIterator;

/**
@author Albert Cervera Areny
*/

class PropertyInfo
{
public:
	PropertyInfo() {}
	PropertyInfo( const QString& name, QVariant::Type type, bool readOnly );
	QVariant::Type type() const;
	const QString& name() const;
	bool readOnly() const;

private:
	QString m_name;
	QVariant::Type m_type;
	bool m_readOnly;
};


/*!
Stores the information of a relation to an object. Mainly stores the name of the relation and a pointer to a function that allows the creation of an object of the related class type.


The constructor is only called from the addObject function in ClassInfo which is called in the OBJECT macro in object.h
*/
class RelatedObject
{
public:
	/*!
	Empty constructor, created for convenience only.
	*/
	RelatedObject();

	/*!
	@param classInfo A pointer to the ClassInfo object the collection is in.
	@param name The name of the relation (not the class of the related object)
	@param function The function which creates an object of the type of the related one
	*/
	RelatedObject( const ClassInfo *classInfo, const QString& name, CreateObjectFunction function );

	/*!
	Get the name of the relation
	@return The name of the relation
	*/
	const QString& name() const;

	/*!
	Get the function pointer to create a new object
	@return The function pointer
	*/
	CreateObjectFunction createObjectFunction() const;

	/*!
	Get whether the relation is One to One or One to N.
	@return True if the relation is One to One, false if it's One to N.
	*/
	bool isOneToOne();

	/*!
	Gets the ClassInfo of the class of the objects related.
	@return The name of the class.
	*/
	const ClassInfo* relatedClassInfo();

private:
	/*!
	Calculates the isOneToOne() and relatedClassInfo() information and saves it in the cache variables.
	*/
	void cacheData();

	CreateObjectFunction m_function;
	QString m_name;
	bool m_oneToOne;

	// Pointer to the parent ClassInfo
	const ClassInfo *m_parentClassInfo;

	// Contains the cached ClassInfo of the related class
	const ClassInfo *m_relatedClassInfo;

	// If the needed data is already in the caches
	bool m_cached;
};



/*!
Stores the information of a relation to collection of objects. Mainly stores the name of the relation and a pointer to a function that allows the creation of an object of the related class type.

The constructor is only called from the addCollection function in ClassInfo which is called by COLLECTION and COLLECTIONN macros in object.h
*/
class RelatedCollection
{
public:
	/*!
	Empty constructor, created for convenience only.
	*/
	RelatedCollection();

	/*!
	@param classInfo A pointer to the ClassInfo object the collection is in.
	@param name The name of the relation (not the class of the related object)
	@param function The function which creates an object of the type of the related one
	@param nToOne Specifies if the relation is N to One, or N to N. This is a hint only and only needed if in the related class there is no declaration.
	*/
	RelatedCollection( ClassInfo *parent, const QString& name, ClassInfo *children, bool nToOne );

	/*!
	Get the name of the relation
	@return The name of the relation
	*/
	const QString& name() const;
	
	/*!
	Get the function pointer to create a new object
	@return The function pointer
	*/
	//CreateObjectFunction createObjectFunction() const;

	/*!
	Get whether the relation is N to One or N to N.
	@return True if the relation is N to One, false if it's N to N.
	*/
	bool isNToOne();

	/*!
	Gets the ClassInfo of the class of the objects related.
	@return The name of the class.
	*/
	ClassInfo* childrenClassInfo();

	/*!
	Gets the ClassInfo of the class of the parent
	*/
	ClassInfo* parentClassInfo();
	
private:
	/*!
	Calculates the isNToOne() and relatedClassInfo() information and saves it in the cache variables.
	*/
	void cacheData();

	QString m_name;
//	CreateObjectFunction m_function;
	bool m_nToOne;

	/*! 
	Pointer to the parent ClassInfo
	*/
	ClassInfo *m_parentClassInfo;

	/*!
	Contains the cached ClassInfo of the related class
	*/
	ClassInfo *m_childrenClassInfo;

	/*!
	If the needed data is already in the caches
	*/
	bool m_cached;
};


typedef QMap<QString,PropertyInfo*> PropertiesInfo;
//typedef QMapIterator<QString,PropertyInfo*> PropertiesInfoIterator;
typedef QMapConstIterator<QString,PropertyInfo*> PropertiesInfoConstIterator;

typedef QMap<QString,RelatedObject*> RelatedObjects;
typedef QMapIterator<QString,RelatedObject*> RelatedObjectsIterator;
typedef QMapConstIterator<QString,RelatedObject*> RelatedObjectsConstIterator;

typedef QMap<QString,RelatedCollection*> RelatedCollections;
typedef QMapIterator<QString,RelatedCollection*> RelatedCollectionsIterator;
typedef QMapConstIterator<QString,RelatedCollection*> RelatedCollectionsConstIterator;


/*!
Contains information regarding the structure of a class that subclasses Object.
*/
class ClassInfo
{
public:
	/*!
	@param name Name of the class
	@param function Pointer to the function that can create an instance of an object of "class" type
	*/
	ClassInfo( const QString& name, CreateObjectFunction function );

	Object* create( Manager* manager = 0 ) const;
	/*!
	Creates an instance of the class type given by name().
	@param oid Oid for the newly created/loaded object
	@param manager Optional manager that will hold the object (Manager::self() will be used if not specified)
	@param create Optional create the object instead of try to load it.
	@return The pointer to the newly created object
	*/
	Object* create( const OidType& oid, Manager* manager = 0, bool create = false ) const;

	/*!
	Creates an instance of the class type given by name() without assigning an oid nor adding the object to the manager. Equivalent to 'createObjectFunction()()'
	@return The pointer to the newly created object
	*/
	Object* createInstance() const;

	/*!
	Returns a pointer to the function that can create new objects of this type.
	*/
	CreateObjectFunction createObjectFunction() const;

	/*!
	Adds a relation to the class. Indicates that the class has a 1-1 relation. Called only inside the OBJECT macro in object.h
	@param className The name of the related class
	@param relationName The name of the relation. If QString::null, the name will be calculated by concatenating both class names in alphabetical order.
	@param function The pointer to the function that can create objects.
	*/
	void addObject( const QString& className, const QString& relationName, CreateObjectFunction function );
	
	/*!
	Adds a relation to the class. Indicates that the class has a N-1 or N-N relation. Called only inside the COLLECTION and COLLECTIONN macros in object.h
	@param className The name of the related class
	@param relationName The name of the relation. If QString::null, the name will be calculated by concatenating both class names in alphabetical order.
	@param function The pointer to the function that can create objects.
	@param nToOne Specifies if the relation is N to One, or N to N. This is a hint only and only needed if in the related class there is no declaration.
	@param definitiveName true if the name of the relation shouldn't be calculated by the function. If false, the function will, create a relation name using the name of both classes (concatenated in alphabetical order).
	*/
	void addCollection( const QString& className, const QString& relationName, bool nToOne = true);

	void addProperty( const QString& name, QVariant::Type type, bool readOnly = false );

	/*!
	Gets the name of the class.
	@return The name of the class
	*/
	const QString& name() const;

	/*!
	Gets the beggining of the list of related objects. This is the const version.
	@return An iterator pointing to the first position of the list of related objects.
	*/
	RelatedObjectsConstIterator objectsBegin() const;

	/*!
	Gets the last item of the list of related objects. This is the const version.
	@return An iterator pointing to the last position of the list of related objects.
	*/
	RelatedObjectsConstIterator objectsEnd() const;

	/*!
	Gets the beggining of the list of related objects.
	@return An iterator pointing to the first position of the list of related objects.
	*/
	RelatedObjectsIterator objectsBegin();

	/*!
	Gets the beggining of the list of related objects.
	@return An iterator pointing to the first position of the list of related objects.
	*/
	RelatedObjectsIterator objectsEnd();

	/*!
	Searches if the object contains a 1-to-1 relation with the given name.
	@param name Name of the relation to look for.
	@return true, if the class contains such a relation. false otherwise.
	*/
	bool containsObject( const QString& name ) const;

	/*!
	Returns the RelatedObject for a given 1-to-1 relation.
	@param name Name of the relation to look for.
	@return The RelatedObject.
	*/
	RelatedObject* object( const QString& name ) const;

	/*!
	Gets the number of 1-to-1 relations
	@return The number of 1-to-1 relations.
	*/
	int objectsCount() const;
	
	/*!
	Gets the number of 1-to-1 relations
	@return The number of 1-to-1 relations.
	*/
	int numObjects() const;
	
	/*!
	Gets the beggining of the list of related collections. This is the const version.
	@return An iterator pointing to the first position of the list of related collections.
	*/
	RelatedCollectionsConstIterator collectionsBegin() const;
	/*!
	Gets the last entry of the list of related objects. This is the const version.
	@return An iterator pointing to the first position of the list of related collections.
	*/
	RelatedCollectionsConstIterator collectionsEnd() const;
	
	/*!
	Gets the beggining of the list of related collections.
	@return An iterator pointing to the first position of the list of related collections.
	*/
	RelatedCollectionsIterator collectionsBegin();
	
	/*!
	Gets the last entry of the list of related objects.
	@return An iterator pointing to the first position of the list of related collections.
	*/
	RelatedCollectionsIterator collectionsEnd();

	/*!
	Searches if the class contains a N-to-1 or N-to-N relation with the given name.
	@param name Name of the relation to look for.
	@return true, if the class contains such a relation. false otherwise.
	*/
	bool containsCollection( const QString& name ) const;
	
	/*!
	Returns the RelatedCollection for a given 1-to-1 relation.
	@param name Name of the relation to look for.
	@return The RelatedCollection.
	*/
	RelatedCollection* collection( const QString& name ) const;

	/*!
	Gets the number of N-to-1 or N-to-N relations
	@return The number of N-to-1 or N-to-N relations.
	*/
	int collectionsCount() const;
	
	/*!
	Gets the number of N-to-1 or N-to-N relations
	@return The number of N-to-1 or N-to-N relations.
	*/
	int numCollections() const;

	static QString relationName( const QString& relation, const QString& className );

	PropertiesInfoConstIterator propertiesBegin() const;
	PropertiesInfoConstIterator propertiesEnd() const;
	const PropertyInfo* property( const QString& name ) const;
	bool containsProperty( const QString& name ) const;
	uint numProperties() const;

	void addMetaInfo( const QString& name, QObject *object );
	QObject* metaInfo( const QString& name ) const;

private:
	QString m_name;
	CreateObjectFunction m_function;

	RelatedObjects m_objects;
	RelatedCollections m_collections;
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


class Classes
{
public:
	/*!
	Ends all the configuration required for browsing throug class relations. Should be used at the very begginning of each application.
	*/
	static void setup();

	/*!
	Registers a new class to the list of known classes. This function is used by DeclareClass.
	@param name The name of the class.
	@param createInstance The pointer to the function that allows the creation of objects of the type class given by name.
	@param createRelations The pointer to the function that creates the relations of the class.
	*/
	static void addClass( const QString& name, CreateObjectFunction createInstance, CreateRelationsFunction createRelations );

	/*!
	Returns an iterator pointing to the first class
	*/
	static ClassInfoIterator begin();

	/*!
	Returns an iterator pointing to the last class
	*/
	static ClassInfoIterator end();

	/*!
	Checks if the given class exists.
	@param name The name of the class to search for.
	@return true if the class exists, false otherwise.
	*/
	static bool contains( const QString& name );

	/*!
	Gets the information of a given class.
	@param name The name of the class to search for.
	@return A pointer to the ClassInfo structure of the given class.
	*/
	static ClassInfo* classInfo( const QString& name );

	/*!
	Creates an instance of the given class, optionally specifing its oid.
	@param name The name of the class type for the new object.
	@param oid Optional. The oid for the newly created object.
	@return The pointer to the newly created object. 0 if it could not be created.	
	*/
//	static Object* create( const QString& name, const OidType& oid = 0 );

	/*!
	Gets the current class which is being created. This is a convenience class used by the OBJECT, COLLECTION and COLLECTIONN macros.
	@return The pointer to the class which is being created.
	*/
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
