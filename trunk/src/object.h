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

#ifndef _OBJECT_H_
#define _OBJECT_H_

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <assert.h>

#include <qvariant.h>

#include <kmainwindow.h>
#include <kdebug.h>

#include "defs.h"
#include "oidtype.h"
#include "seqtype.h"
#include "collection.h"
#include "manager.h"
#include "classes.h"

/**
 * @short Application Main Window
 * @author Albert Cervera Areny <albertca@hotpop.com>
 * @version 0.1
 */

class Collection;
class Object;


#define OBJECT( Object ) Classes::currentClass()->addObject( #Object, QString::null, &Object::createInstance )
#define COLLECTION( Object ) Classes::currentClass()->addCollection( #Object, QString::null )
#define COLLECTIONN( Object ) Classes::currentClass()->addCollection( #Object, QString::null, false )

#define OBJECTR( Object, name ) Classes::currentClass()->addObject( #Object, name, &Object::createInstance )
#define COLLECTIONR( Object, name ) Classes::currentClass()->addCollection( #Object, name )
#define COLLECTIONNR( Object, name ) Classes::currentClass()->addCollection( #Object, name, false )

/*
#define OBJECT( Object ) Classes::currentClass()->addObject( #Object, QString::null, &Object::createInstance )
#define COLLECTION( Object ) Classes::currentClass()->addCollection( #Object, QString::null, &Object::createInstance )
#define COLLECTIONN( Object ) Classes::currentClass()->addCollection( #Object, QString::null, &Object::createInstance, false )

#define OBJECTR( Object, name ) Classes::currentClass()->addObject( #Object, name, &Object::createInstance )
#define COLLECTIONR( Object, name ) Classes::currentClass()->addCollection( #Object, name, &Object::createInstance )
#define COLLECTIONNR( Object, name ) Classes::currentClass()->addCollection( #Object, name, &Object::createInstance, false )
*/


#define DCLASS( obj ) static obj* create(); static obj* create( OidType oid ); static Object* createInstance(); obj* createObjectInstance(); static void createLabels(); static void createRelations();

#define ICLASS( class ) \
	class* class::create() \
	{\
		class *object = new class();\
		assert( object );\
		object->m_modified = true;\
		Manager::self()->add( object );\
		return object;\
	}\
	class* class::create( OidType oid )\
	{\
		return static_cast<class*>( Manager::self()->load( oid, &createInstance ) );\
	}\
	Object* class::createInstance()\
	{\
		Object* obj = new class();\
		assert( obj );\
		return obj;\
	}\
	class* class::createObjectInstance()\
	{\
		class* obj = new class();\
		assert( obj );\
		return obj;\
	}\
	DeclareClass declareClass##class( #class, &class::createInstance, &class::createLabels, &class::createRelations );

#define MODIFIED m_modified = true;


//TODO: Make versions of these macros for debug mode.
//	For example, use dynamic_cast, and check the pointer

//#define SETOBJECT( Class, Object ) Manager::self()->setRelation( this, #Class, Object )
#define SETOBJECT( Class, Object ) Manager::self()->setRelation( oid(), classInfo(), #Class, Object ? Object->oid() : 0 )
#define GETOBJECT( Class ) static_cast<Class*>(Manager::self()->load( Manager::self()->relation( this, #Class ), &Class::createInstance ) )

#define GETCOLLECTION( Class ) collection( #Class )

/*
#ifdef DEBUG
#define SETOBJECT( Class, Object ) Class testClassInstance; setObject( #Class, Object );
const QString& classTypeTestFunction( const QString&, Object* );
#define GETOBJECT( Class ) static_cast<Class*>( object( classTypeTestFunction( #Class, new Class() ) ) )
#endif

#ifndef DEBUG
#define SETOBJECT( Class, Object ) setObject( #Class, Object )
#define GETOBJECT( Class ) static_cast<Class*>(object( #Class ))
#endif
*/

class Property
{
public:
	Property() {}
	Property( Object *obj, const QString& name );
	QVariant::Type type();
	QVariant value();
	void setValue( const QVariant& value );
	QString name();

private:
	Object *m_object;
	QString m_name;
};


class PropertyIterator
{
public:
	PropertyIterator( Object *object, int pos );
	Property data();
	const Property data() const;
	PropertyIterator& operator++();
	PropertyIterator& operator--();
	PropertyIterator operator++(int);
	PropertyIterator operator--(int);
	bool operator==( const PropertyIterator& it ) const;
	bool operator!=( const PropertyIterator& it ) const;
	Property operator*();
	const Property operator*() const;
	PropertyIterator& operator=(const PropertyIterator& it);

private:
	Object *m_object;
	int m_pos;
};

// TODO: Add a method to SET the object like in setObject function of Object class.
class ObjectIterator
{
public:
//	ObjectIterator( QMapIterator<QString,QPair<OidType,CreateObjectFunction> > it );
	ObjectIterator( const OidType& oid, RelatedObjectsIterator it );
	ObjectIterator( QMapIterator<OidType,bool> it, CreateObjectFunction function );
	Object* data();
	const Object* data() const;
	QString key();
	const QString& key() const;
	const RelatedObject* relatedObject() const;
	ObjectIterator& operator++();
	ObjectIterator& operator--();
	ObjectIterator operator++(int);
	ObjectIterator operator--(int);
	bool operator==( const ObjectIterator& it ) const;
	bool operator!=( const ObjectIterator& it ) const;
	Object* operator*();
	const Object* operator*() const;
	ObjectIterator& operator=(const ObjectIterator& it);

private:
	//QMapIterator<QString,QPair<OidType,CreateObjectFunction> > m_it;
	RelatedObjectsIterator m_it;
	QMapIterator<OidType,bool> m_colit;
	CreateObjectFunction m_createObjectFunction;
	bool m_collection;
	OidType m_oid;

protected:
	int attribute_1;
};

class CollectionIterator
{
public:
	CollectionIterator( const OidType& oid, RelatedCollectionsIterator it );
	//CollectionIterator( QMapIterator<QString,Collection*> it );
	Collection* data();
	const Collection* data() const;
	CollectionIterator& operator++();
	CollectionIterator& operator--();
	CollectionIterator operator++(int);
	CollectionIterator operator--(int);
	bool operator==( const CollectionIterator& it ) const;
	bool operator!=( const CollectionIterator& it ) const;
	Collection* operator*();
	const Collection* operator*() const;
	CollectionIterator& operator=(const CollectionIterator& it);
private:
	RelatedCollectionsIterator m_it;
	OidType m_oid;
};


/*
	We should provide an easy way to check if any class that inherits Object,
	has the MODIFIED macro to all set operations and doesn't have it on GET
	operations. Maybe an optional compilation flag could add a function, or
	simply a test case should be provided.
*/

/*!
Any object that is to be persistent must inherit from this class, declare de class with DCLASS in the header and implement it with ICLASS (see example below)

Data which has to be saved needs to be a Qt property (see http://doc.trolltech.com/3.3/properties.html) and relations of objects

Related objects (that is 1-1 relations), have to be declared with the OBJECT() macro in the void OurNewPersistentClass::createRelations() function.

Related collections (that is N-N or N-1 relations) have to be declared COLLECTION() macro in the createRelations() function, as well.

Example:
<code>
// Header: example.h

class Example : public Object
{
	Q_OBJECT
	Q_PROPERTY( QString exampleProperty READ exampleProperty WRITE setExampleProperty )

public:
	DCLASS( Example );

	const QString& exampleProperty() const;
	void setExampleProperty( const QString& prop );

	AnotherExample* anotherExample();
	void setAnotherExample( AnotherExample* otherExample );

	Collection* moreExamples();

private:
	QString m_exampleProperty;
};

// Source: example.cpp
#include <klocale.h>
#include <labels.h>

#include "example.h"
#include "anotherexample.h"

ICLASS( Example );

void Example::createRelations()
{
	OBJECT( AnotherExample );
	COLLECTION( AnotherExample);
}

void Example::createLabels()
{
	LABEL( "exampleProperty", i18n( "Property of the example" ) );
}

const QString& Example::exampleProperty() const
{
	return m_exampleProperty;
}

void Example::setExampleProperty( const QString& prop )
{
	MODIFIED;
	m_exampleProperty = prop;
}

AnotherExample* Example::anotherExample()
{
	return GETOBJECT( AnotherExample );
}

void Example::setAnotherExample( AnotherExample* otherExample )
{
	SETOBJECT( AnotherExample, otherExample );
}

Collection* Example::moreExamples()
{
	return GETCOLLECTION( AnotherExample );
}

#include "example.moc"
</code>
*/

class Object : public QObject
{
	Q_OBJECT
public:
	Object();
	virtual ~Object();

	static Object* create();
	static Object* create( OidType oid );
	static Object* createInstance();
	Object* createObjectInstance() const;

	ClassInfo* classInfo();
	ClassInfo* classInfo() const;

	OidType oid() const;
	void setOid( const OidType& oid );
	SeqType seq() const;
	void setSeq( const SeqType& seq );

	bool isNull() const;

	bool isModified() const;
	void setModified( bool value );

	/*
	Functions for managing the properties
	*/
	PropertyIterator propertiesBegin();
	PropertyIterator propertiesEnd();
	int numProperties() const;
	Property property( int pos );
	Property property( const QString& name );
	bool containsProperty( const QString& name );

	// This property is kind of special, it is provided to allow
	// consistency. If we create the property() function that returns
	// a Property then we need some kind of function to allow access
	// to the inherited property( const char*) function, to let the
	// Property class do its job.
	QVariant propertyValue( const char* name ) const;

	/*
	Functions for managing the other objects related
	*/
	ObjectIterator objectsBegin();
	ObjectIterator objectsEnd();
	bool containsObject( const QString& name ) const;
	int numObjects() const;
	Object* object( const QString& name ) const;
	void setObject( const QString& name, Object* object );
	void setObject( const QString& name, const OidType& oid );

	/*
	Functions for managing the collections of objects related
	*/
	CollectionIterator collectionsBegin();
	CollectionIterator collectionsEnd();
	bool containsCollection( const QString& name ) const;
	int numCollections() const;
	Collection* collection( const QString& name ) const;

protected:
	// m_modified is protected because the MODIFIED macro needs it,
	// is it a good solution?
	bool m_modified;

private:
	bool m_removed, m_loaded;
	OidType m_oid;
	SeqType m_seq;
	// Used as a cache. It is calculated the first time the classInfo() (non-const) function is called and used from there on
	ClassInfo *m_classInfo;
};


template <class T>
class ObjectRef
{
public:
	ObjectRef()
	{
		m_oid = 0;
	}

	ObjectRef( T* object )
	{
		m_oid = static_cast<Object*>( object )->oid();
	}

	ObjectRef( const OidType& oid )
	{
		m_oid = oid;
	}

	ObjectRef<T>& operator=( T* object )
	{
		m_oid = static_cast<Object*>( object )->oid();
		return *this;
	}

	ObjectRef<T>& operator=( ObjectRef<T> ref )
	{
		m_oid = ref.m_oid;
		return *this;
	}

	T* operator->()
	{
		return T::create( m_oid );
	}

	T& operator*()
	{
		return *T::create( m_oid );
	}

	operator T*() const
	{
		return T::create( m_oid );
	}

	OidType oid() const
	{
		return m_oid;
	}

	bool isNull() const
	{
		return m_oid == 0;
	}

private:
	OidType m_oid;
};

#endif // _OBJECT_H_
