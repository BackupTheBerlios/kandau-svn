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
		kdDebug() << k_funcinfo << object->oid() << endl;\
		return object;\
	}\
	class* class::create( OidType oid )\
	{\
		return static_cast<class*>( Manager::self()->load( oid, &createInstance ) );\
	}\
	Object* class::createInstance()\
	{\
		return new class();\
	}\
	class* class::createObjectInstance()\
	{\
		return new class();\
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


class ObjectIterator
{
public:
//	ObjectIterator( QMapIterator<QString,QPair<OidType,CreateObjectFunction> > it );
	ObjectIterator( const OidType& oid, RelatedObjectsIterator it );
	ObjectIterator( QMapIterator<OidType,int> it, CreateObjectFunction function );
	Object* data();
	const Object* data() const;
	QString key();
	const QString& key() const;
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
	QMapIterator<OidType,int> m_colit;
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
	//QMapIterator<QString,Collection*> m_it;
	RelatedCollectionsIterator m_it;
	OidType m_oid;
};


/*!
	We should provide an easy way to check if any class that inherits Object,
	has the MODIFIED macro to all set operations and doesn't have it on GET
	operations. Maybe an optional compilation flag could add a function, or
	simply a test case should be provided.
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

	bool save();
	/*
	This function, though necessary, introduces some problems to the
	design that need to be resolved. What happens when an object is
	deleted and other objects are refering it?
	Maybe it is needed an SQL like solution. The idea is that we
	are able to configure the Manager to one of the following behaviours:
	- NoAction -> Does not allow the removal of the object and thus remove()
	returns	a false.
	- Cascade -> Delete any objects refering this object.
	- SetNull -> Set to null the references of the refering objects.
	- Ignore -> This one introduces inconsistency but might be desired for
	performance reasons. Indeed, it could enable for example progressive
	nullification of the references. For example, object One is removed and
	object Two which referes to One is instanciated, and then from Two we want
	to browse to One, of course this one doesn't exist it is not loaded and
	set to null at that moment. However, this solution doesn't look good as one
	should test if an object is null and then load it. But maybe it could be
	an option. Just writing my thoughts.
	- Delay -> Another option, maybe not very nice could be to delay the
	nullification or the cascade options until commit() but it would bring
	inconsistencies in memory before the commit so most probably not a good
	idea.

	Another possibility to the whole deletion problem could be specify the
	behaviour in each reference something like OBJECT( ReferedObject, CASCADE )
	This would surely add more flexibility to the programer/designer, though it
	could be left to a later development cycle.
	*/
	bool remove();

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


	// If it is possible, we should try to avoid the need for this function
	// as objects are stored in a QMap() it would be very inefficent to allow
	// acces by an index. We have an iterator anyway and if access to an specific
	// Object is needed we have the QString as the key.
	//Object* object( int pos );

	/*
	Functions for managing the collections of objects related
	*/
	CollectionIterator collectionsBegin();
	CollectionIterator collectionsEnd();
	bool containsCollection( const QString& name ) const;
	int numCollections() const;
	Collection* collection( const QString& name ) const;

	// Does it make any sense to make a setCollection function? I don't
	// see the point right now so...

protected:
	/*
	Routines to ease the creation of relations between objects, better if they
	are used through the OBJECT() and COLLECTION(), macros.
	*/
	//void createSubobject( const QString& className, CreateObjectFunction function );
	//void createSubcollection( const QString& className, CreateObjectFunction function, bool nToOne = true );

	// m_modified is protected because the MODIFIED macro needs it,
	// is it a good solution?
	bool m_modified;

private:
	bool m_saved, m_removed, m_loaded;
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
