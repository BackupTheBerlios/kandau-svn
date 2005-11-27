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

#define ADDMETAINFO( name, object ) Classes::currentClass()->addMetaInfo( name, object )

/*
#define OBJECT( Object ) Classes::currentClass()->addObject( #Object, QString::null, &Object::createInstance )
#define COLLECTION( Object ) Classes::currentClass()->addCollection( #Object, QString::null, &Object::createInstance )
#define COLLECTIONN( Object ) Classes::currentClass()->addCollection( #Object, QString::null, &Object::createInstance, false )

#define OBJECTR( Object, name ) Classes::currentClass()->addObject( #Object, name, &Object::createInstance )
#define COLLECTIONR( Object, name ) Classes::currentClass()->addCollection( #Object, name, &Object::createInstance )
#define COLLECTIONNR( Object, name ) Classes::currentClass()->addCollection( #Object, name, &Object::createInstance, false )
*/


#define DCLASS( obj ) static obj* create( Manager* manager = 0); static obj* create( OidType oid, Manager* manager = 0 ); static Object* createInstance(); obj* createObjectInstance(); static void createRelations();

#define ICLASS( class ) \
	class* class::create( Manager* manager ) \
	{\
		class *object = new class();\
		assert( object );\
		object->m_modified = true;\
		object->setManager( manager );\
		object->manager()->add( object );\
		return object;\
	}\
	class* class::create( OidType oid, Manager* manager )\
	{\
		if ( manager )\
			return static_cast<class*>( manager->load( oid, &createInstance ) );\
		else\
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
	DeclareClass declareClass##class( #class, &class::createInstance, &class::createRelations );


#ifdef MODIFIED_NONE
#elif MODIFIED_EMPTY
#define MODIFIED
#elif MODIFIED_SIMPLE
#define MODIFIED m_modified = true;
#elif MODIFIED_FULL
#define MODIFIED(value) if ( ! m_manager->notifyPropertyModified( this, __FUNCTION__, value ) ) return; m_modified = true;
#else
#define MODIFIED if ( ! m_manager->notifyPropertyModified( this, __FUNCTION__ ) ) return; m_modified = true;
#endif

/*
#ifdef WITHOUT_MODIFIED_CALLBACKS
#define MODIFIED m_modified = true;
#else
#define MODIFIED(value) if ( ! m_manager->notifyPropertyModified( this, __FUNCTION__, value ) ) return; m_modified = true;
#endif
*/

//TODO: Make versions of these macros for debug mode.
//	For example, use dynamic_cast, and check the pointer

//#define SETOBJECT( Class, Object ) Manager::self()->setRelation( this, #Class, Object )
#define SETOBJECT( Class, Object ) m_manager->setRelation( oid(), classInfo(), #Class, Object ? Object->oid() : 0 )
#define SETOBJECTR( Class, Relation, Object ) m_manager->setRelation( oid(), classInfo(), Relation, Object ? Object->oid() : 0 )
#define GETOBJECT( Class ) static_cast<Class*>(m_manager->load( m_manager->relation( this, #Class ), &Class::createInstance ) )
#define GETOBJECTR( Class, Relation ) static_cast<Class*>(m_manager->load( m_manager->relation( this, Relation ), &Class::createInstance ) )

//#define SETDYNOBJECT( Class, Object ) m_manager->setRelation( oid(), classInfo(), 
#define GETDYNOBJECT( Class ) static_cast<DynamicObject*>(m_manager->load( m_manager->relation( this, #Class ), &DynamicObject::createInstance ) )
#define GETDYNOBJECTR( Class, Relation ) static_cast<DynamicObject*>(m_manager->load( Relation, &DynamicObject::createInstance ) )

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
	Property( const Object *obj, const QString& name );
	QVariant::Type type() const;
	QVariant value() const;
	void setValue( const QVariant& value );
	const QString& name() const;
	bool readOnly() const;

private:
	Object *m_object;
	const Object *m_constObject;
	QString m_name;
};

class PropertiesIterator
{
public:
	PropertiesIterator( Object *object, PropertiesInfoConstIterator it );
	Property data();
	const Property data() const;
	PropertiesIterator& operator++();
	PropertiesIterator& operator--();
	PropertiesIterator operator++(int);
	PropertiesIterator operator--(int);
	bool operator==( const PropertiesIterator& it ) const;
	bool operator!=( const PropertiesIterator& it ) const;
	Property operator*();
	const Property operator*() const;
	PropertiesIterator& operator=(const PropertiesIterator& it);

private:
	Object *m_object;
	PropertiesInfoConstIterator m_it;
};

class PropertiesConstIterator
{
public:
	PropertiesConstIterator( const Object *object, PropertiesInfoConstIterator it );
	const Property data() const;
	PropertiesConstIterator& operator++();
	PropertiesConstIterator& operator--();
	PropertiesConstIterator operator++(int);
	PropertiesConstIterator operator--(int);
	bool operator==( const PropertiesConstIterator& it ) const;
	bool operator!=( const PropertiesConstIterator& it ) const;
	const Property operator*() const;
	PropertiesConstIterator& operator=(const PropertiesConstIterator& it);

private:
	const Object *m_object;
	PropertiesInfoConstIterator m_it;
};

// TODO: Add a method to SET the object like in setObject function of Object class.
class ObjectsIterator
{
public:
	ObjectsIterator( const OidType& oid, RelatedObjectsConstIterator it, Manager* manager );
	Object* data();
	const Object* data() const;
	QString key();
	const QString& key() const;
	const RelatedObject* relatedObject() const;
	ObjectsIterator& operator++();
	ObjectsIterator& operator--();
	ObjectsIterator operator++(int);
	ObjectsIterator operator--(int);
	bool operator==( const ObjectsIterator& it ) const;
	bool operator!=( const ObjectsIterator& it ) const;
	Object* operator*();
	const Object* operator*() const;
	ObjectsIterator& operator=(const ObjectsIterator& it);

private:
	RelatedObjectsConstIterator m_it;
	OidType m_oid;
	Manager* m_manager;
};

class CollectionsIterator
{
public:
	CollectionsIterator( const OidType& oid, RelatedCollectionsConstIterator it, Manager* manager );
	Collection* data();
	const Collection* data() const;
	CollectionsIterator& operator++();
	CollectionsIterator& operator--();
	CollectionsIterator operator++(int);
	CollectionsIterator operator--(int);
	bool operator==( const CollectionsIterator& it ) const;
	bool operator!=( const CollectionsIterator& it ) const;
	Collection* operator*();
	const Collection* operator*() const;
	CollectionsIterator& operator=(const CollectionsIterator& it);
private:
	RelatedCollectionsConstIterator m_it;
	OidType m_oid;
	Manager* m_manager;
};


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
	Object& operator=( const Object& obj );

	static Object* create( Manager* manager = 0 );
	static Object* create( OidType oid, Manager* manager = 0 );
	static Object* createInstance();
	Object* createObjectInstance() const;

	void setClassInfo( const ClassInfo* info );
	const ClassInfo* classInfo() const;
	Manager* manager() const;
	void setManager( Manager* m );

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
	PropertiesIterator propertiesBegin();
	PropertiesConstIterator propertiesBegin() const;
	PropertiesConstIterator propertiesConstBegin() const;
	
	PropertiesIterator propertiesEnd();
	PropertiesConstIterator propertiesEnd() const;
	PropertiesConstIterator propertiesConstEnd() const;
	int numProperties() const;
	//Property property( const char* name ) const;
	Property property( const QString& name );
	const Property property( const QString& name ) const;
	bool containsProperty( const QString& name ) const;

	// This property is kind of special, it is provided to allow
	// consistency. If we create the property() function that returns
	// a Property then we need some kind of function to allow access
	// to the inherited property( const char*) function, to let the
	// Property class do its job.
	virtual QVariant propertyValue( const char* name ) const;

	/*
	Functions for managing the other objects related
	*/
	ObjectsIterator objectsBegin();
	ObjectsIterator objectsEnd();
	bool containsObject( const QString& name ) const;
	int numObjects() const;
	Object* object( const QString& name ) const;
	void setObject( const QString& name, Object* object );
	void setObject( const QString& name, const OidType& oid );

	/*
	Functions for managing the collections of objects related
	*/
	CollectionsIterator collectionsBegin();
	CollectionsIterator collectionsEnd();
	bool containsCollection( const QString& name ) const;
	int numCollections() const;
	Collection* collection( const QString& name ) const;

protected:
	// m_modified is protected because the MODIFIED macro needs it,
	// is it a good solution?
	bool m_modified;
	Manager *m_manager;
	// Used as a cache. It is calculated the first time the classInfo() (non-const) function is called and used from there on
	const ClassInfo *m_classInfo;

private:
	OidType m_oid;
	SeqType m_seq;
};


template <class T>
class ObjectRef
{
public:
	ObjectRef()
	{
		m_oid = 0;
		m_manager = 0;
		m_classInfo = 0;
	}
	
	ObjectRef( T* object )
	{
		if ( object ) {
			Object* obj = static_cast<Object*>( object );
			m_oid = obj->oid();
			m_manager = obj->manager();
			m_classInfo = obj->classInfo();
		} else {
			m_oid = 0;
		}
	}
	
	ObjectRef<T>& operator=( T* object )
	{
		if ( object ) {
			Object* obj = static_cast<Object*>( object );
			m_oid = obj->oid();
			m_manager = obj->manager();
			m_classInfo = obj->classInfo();
		} else {
			m_oid = 0;
		}
		return *this;
	}

	ObjectRef<T>& operator=( ObjectRef<T> ref )
	{
		m_oid = ref.m_oid;
		m_manager = ref.m_manager;
		m_classInfo = ref.m_classInfo;
		return *this;
	}

	T* operator->() const
	{
		return static_cast<T*>( m_manager->load( m_oid, m_classInfo ) );
	}
	
	T& operator*()
	{
		return &(static_cast<T*>( m_manager->load( m_oid, m_classInfo ) ));
	}

	operator T*() const
	{
		return static_cast<T*>( m_manager->load( m_oid, m_classInfo ) );
	}

	OidType oid() const
	{
		return m_oid;
	}

	bool isNull() const
	{
		return m_oid == 0;
	}

	void setNull()
	{
		m_oid = 0;
	}

private:
	OidType m_oid;
	Manager* m_manager;
	const ClassInfo *m_classInfo;
};

template <class T>
class ConstObjectRef
{
public:
	ConstObjectRef()
	{
		m_oid = 0;
		m_manager = 0;
		m_classInfo = 0;
	}

	ConstObjectRef( const T* object )
	{
		const Object* obj = static_cast<const Object*>( object );
		m_oid = obj->oid();
		m_manager = obj->manager();
		m_classInfo = obj->classInfo();
	}
	
	ConstObjectRef<T>& operator=( const T* object )
	{
		const Object* obj = static_cast<const Object*>( object );
		m_oid = obj->oid();
		m_manager = obj->manager();
		m_classInfo = obj->classInfo();
		return *this;
	}

	ConstObjectRef<T>& operator=( ConstObjectRef<T> ref )
	{
		m_oid = ref.m_oid;
		m_manager = ref.m_manager;
		m_classInfo = ref.m_classInfo;
		return *this;
	}

	const T* operator->()
	{
		return static_cast<T*>( m_manager->load( m_oid, m_classInfo ) );
	}
	
	const T& operator*()
	{
		return &(static_cast<T*>( m_manager->load( m_oid, m_classInfo ) ));
	}

	operator T*() const
	{
		return static_cast<T*>( m_manager->load( m_oid, m_classInfo ) );
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
	Manager* m_manager;
	const ClassInfo *m_classInfo;
};


#endif // _OBJECT_H_
