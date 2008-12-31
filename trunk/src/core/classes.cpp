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
#include <qmetaobject.h>
#include <QList>
#include <QStringList>

#include "classes.h"
#include "object.h"

#include "collection.h"

ClassInfoMap *Classes::m_classes = 0;
ClassInfo* Classes::m_currentClass = 0;
TmpClassMap* Classes::m_tmpClasses = 0;



/* ClassInfo */

/*!
@param name Name of the class
@param function Pointer to the function that can create an instance of an object of "class" type
*/
ClassInfo::ClassInfo( const QString& name, CreateObjectFunction function )
{
	m_name = name;
	m_function = function;
	m_parent = 0;
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

/*!
Creates an instance of the class type given by name().
@param oid Oid for the newly created/loaded object
@param manager Optional manager that will hold the object (Manager::self() will be used if not specified)
@param create Optional create the object instead of try to load it.
@return The pointer to the newly created object
*/
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

/*!
Creates an instance of the class type given by name() without assigning an oid nor adding the object to the manager. Equivalent to 'createObjectFunction()()'
@return The pointer to the newly created object
*/
Object* ClassInfo::createInstance() const
{
	return m_function();
}

/*!
Returns a pointer to the function that can create new objects of this type.
*/
CreateObjectFunction ClassInfo::createObjectFunction() const
{
	return m_function;
}

/*!
Gets the name of the class.
@return The name of the class
*/
const QString& ClassInfo::name() const
{
	return m_name;
}

/*!
Adds a relation to the class. Indicates that the class has a 1-1 relation. Called only inside the OBJECT macro in object.h
@param className The name of the related class
@param relationName The name of the relation. If QString::null, the name will be calculated by concatenating both class names in alphabetical order.
@param function The pointer to the function that can create objects.
*/
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

/*!
Adds a relation to the class. Indicates that the class has a N-1 or N-N relation. Called only inside the COLLECTION and COLLECTIONN macros in object.h
@param className The name of the related class
@param relationName The name of the relation. If QString::null, the name will be calculated by concatenating both class names in alphabetical order.
@param function The pointer to the function that can create objects.
@param nToOne Specifies if the relation is N to One, or N to N. This is a hint only and only needed if in the related class there is no declaration.
@param definitiveName true if the name of the relation shouldn't be calculated by the function. If false, the function will, create a relation name using the name of both classes (concatenated in alphabetical order).
*/
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

//void ClassInfo::addProperty( const QString& name, QVariant::Type type, bool readOnly, bool inherited )
void ClassInfo::addProperty( PropertyInfo* property )
{
	if ( ! property->name().isNull() )
		m_properties.insert( property->name(), property );
	//m_properties.insert( name, new PropertyInfo( name, type, readOnly, inherited ) );
}

/*!
Used internally. This function fills in the classInfo properties structure
from the QObject information. The function is called from Classes::setup()
as if it is called from the ClassInfo::addClass() function and a property
is of type QPixmap, Qt will abort because a QPaintDevice is created before
a QApplication. That's why Classes::setup() sould be called after creating
a QApplication object.
*/
void ClassInfo::createProperties()
{
	// Fill in the Properties Map
	Object* obj = createInstance();
	for ( int i = 0; i < obj->metaObject()->propertyCount(); ++i ) {
		QMetaProperty meta = obj->metaObject()->property( i );
		// We look if it's inherited or not
		// TODO: Shouldn't work with Qt4
		bool inherited = obj->metaObject()->indexOfProperty( meta.name() ) == -1 ? true : false;
		// TODO: Improve enums support.
		PropertyInfo *p;
		p = new PropertyInfo();
		p->setName( meta.name() );
		p->setType( meta.type() );
		p->setReadOnly( ! meta.isWritable() );
		p->setInherited( inherited );
		p->setEnumType( meta.isEnumType() );
		// TODO: Change setSetType to setFlagType...
		p->setSetType( meta.isFlagType() );

		//char* name; // DEPRECATED Qt3 -> Qt4 by Percy
		//meta.enumerator(); // DEPRECATED Qt3 -> Qt4 by Percy
        //meta.enumerator()
		//QStringList list = meta.enumKeys();
        QStringList list;
        for (int i = 0; i < meta.enumerator().keyCount(); i++)
            list.append(meta.enumerator().key(i));

        // DEPRECATED Qt3 -> Qt4 by Percy
// 		for ( name = list.first(); name; name = list.next() ) {
// 			p->addKeyAndValue( name, meta.keyToValue( name ) );
// 		}

        for ( int i = 0; i < list.count(); i++)
            p->addKeyAndValue( list[i], meta.enumerator().keyToValue( list[i].toAscii() ) );


		m_properties.insert( meta.name(), p );
	}
	delete obj;
}

/*!
Gets the beggining of the list of related objects. This is the const version.
@return An iterator pointing to the first position of the list of related objects.
*/
RelationInfosConstIterator ClassInfo::relationsBegin() const
{
	return m_objects.begin();
}

/*!
Gets the last item of the list of related objects. This is the const version.
@return An iterator pointing to the last position of the list of related objects.
*/
RelationInfosConstIterator ClassInfo::relationsEnd() const
{
	return m_objects.end();
}

/*!
Gets the beggining of the list of related objects.
@return An iterator pointing to the first position of the list of related objects.
*/
RelationInfosIterator ClassInfo::relationsBegin()
{
	return m_objects.begin();
}

/*!
Gets the beggining of the list of related objects.
@return An iterator pointing to the first position of the list of related objects.
*/
RelationInfosIterator ClassInfo::relationsEnd()
{
	return m_objects.end();
}

/*!
Searches if the object contains a 1-to-1 relation with the given name.
@param name Name of the relation to look for.
@return true, if the class contains such a relation. false otherwise.
*/
bool ClassInfo::containsObject( const QString& name ) const
{
	return m_objects.contains( name );
}

/*!
Returns the RelationInfo for a given 1-to-1 relation.
@param name Name of the relation to look for.
@return The RelationInfo.
*/
RelationInfo* ClassInfo::object( const QString& name ) const
{
	return m_objects[ name ];
}

/*!
Gets the number of 1-to-1 relations
@return The number of 1-to-1 relations.
*/
int ClassInfo::objectsCount() const
{
	return m_objects.count();
}

/*!
Gets the number of 1-to-1 relations
@return The number of 1-to-1 relations.
*/
int ClassInfo::numObjects() const
{
	return m_objects.count();
}

/*!
Gets the beggining of the list of related collections. This is the const version.
@return An iterator pointing to the first position of the list of related collections.
*/
CollectionInfosConstIterator ClassInfo::collectionsBegin() const
{
	return m_collections.begin();
}

CollectionInfosConstIterator ClassInfo::collectionsEnd() const
{
	return m_collections.end();
}

/*!
Gets the beggining of the list of related collections.
@return An iterator pointing to the first position of the list of related collections.
*/
CollectionInfosIterator ClassInfo::collectionsBegin()
{
	return m_collections.begin();
}

/*!
Gets the last entry of the list of related objects.
@return An iterator pointing to the first position of the list of related collections.
*/
CollectionInfosIterator ClassInfo::collectionsEnd()
{
	return m_collections.end();
}

/*!
Searches if the class contains a N-to-1 or N-to-N relation with the given name.
@param name Name of the relation to look for.
@return true, if the class contains such a relation. false otherwise.
*/
bool ClassInfo::containsCollection( const QString& name ) const
{
	return m_collections.contains( name );
}

/*!
Returns the CollectionInfo for a given 1-to-1 relation.
@param name Name of the relation to look for.
@return The CollectionInfo.
*/
CollectionInfo* ClassInfo::collection( const QString& name ) const
{
	return m_collections[ name ];
}

/*!
Gets the number of N-to-1 or N-to-N relations
@return The number of N-to-1 or N-to-N relations.
*/
int ClassInfo::collectionsCount() const
{
	return m_collections.count();
}

/*!
Gets the number of N-to-1 or N-to-N relations
@return The number of N-to-1 or N-to-N relations.
*/
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

const ClassInfo* ClassInfo::parent() const
{
	return m_parent;
}

void ClassInfo::setParent( const ClassInfo* parent )
{
	m_parent = parent;
}

const QList<const ClassInfo*>& ClassInfo::children() const
{
	return m_children;
}

void ClassInfo::addChild( const ClassInfo* child )
{
	m_children.append( child );
}

/*!
@param classInfo
@return True if the class inherits classInfo
*/
bool ClassInfo::inherits( const ClassInfo* classInfo ) const
{
	if ( parent() == 0 )
		return false;
	if ( parent() == classInfo )
		return true;
	return parent()->inherits( classInfo );
}

/*!
This function is provided for convenience only and behaves like the above function.
@param className Name of the class to look for.
@return True if the class inherits className
*/
bool ClassInfo::inherits( const QString& className ) const
{
	return inherits( Classes::classInfo( className ) );
}

/*!
Obtains a list with all the ancestors of the class.
@return StringList with the names of all the ancestors of the class.
*/
QStringList ClassInfo::ancestors( ) const
{
	QStringList list;
	for ( const ClassInfo *info = parent(); info != 0; info = info->parent() )
		list.append( info->name() );
	return list;
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

	setupHierarchy();
	setupRelations();
	setupRelationsHierarchy();
}

/*!
Returns a list with all class names. There is no order except that it guarantees
that any given class will be before any of its children.
@return StringList with all class names.
*/
QStringList Classes::parentsFirst()
{
	QStringList list;
	ClassInfoConstIterator it( Classes::begin() );
	ClassInfoConstIterator end( Classes::end() );
	const ClassInfo *info;
	for ( ; it != end; ++it ) {
		info = *it;
		if ( info->parent() == 0 ) {
			list.append( info->name() );
			continue;
		}
		QStringList::iterator it2( list.begin() );
		QStringList::iterator end2( list.end() );
		for ( ; it2 != end2; ++it2 ) {
			if ( info->inherits( *it2 ) ) {
				list.insert( it2, info->name() );
				break;
			}
		}
	}
	QStringList ret;
	QStringList::iterator it3( list.begin() );
	QStringList::iterator end3( list.end() );
	for ( ; it3 != end3; ++it3 ) {
		ret.prepend( *it3 );
	}
	return ret;
}

void Classes::setupRelationsHierarchy()
{
	QStringList list = parentsFirst();
	QStringList::const_iterator it( list.constBegin() );
	QStringList::const_iterator end( list.constEnd() );
	ClassInfo *info;
	for ( ; it != end; ++it ) {
		info = Classes::classInfo( *it );
		if ( ! info->parent() )
			continue;

		QStringList ancestors = info->ancestors();
		QStringList::const_iterator ait( ancestors.constBegin() );
		QStringList::const_iterator aend( ancestors.constEnd() );
		ClassInfo *ancestor;
		for ( ; ait != aend; ++ait ) {
			ancestor = Classes::classInfo( *ait );
			RelationInfosConstIterator rit( ancestor->relationsBegin() );
			RelationInfosConstIterator rend( ancestor->relationsEnd() );
			RelationInfo *relinfo;
			for ( ; rit != rend; ++rit ) {
				// relinfo = rit.data(); // DEPRECATED Qt3 -> Qt4 by Percy
                relinfo = rit.value();
				if ( ! info->containsObject( relinfo->name() ) ) {
					info->addObject( relinfo->relatedClassInfo()->name(), relinfo->name(), relinfo->relatedClassInfo()->createObjectFunction() );
				}
			}

			CollectionInfosConstIterator cit( ancestor->collectionsBegin() );
			CollectionInfosConstIterator cend( ancestor->collectionsEnd() );
			CollectionInfo *colinfo;
			for ( ; cit != cend; ++cit ) {
				// colinfo = cit.data(); // DEPRECATED Qt3 -> Qt4 by Percy
                colinfo = cit.value();
				if ( ! info->containsCollection( colinfo->name() ) ) {
					info->addCollection( colinfo->childrenClassInfo()->name(), colinfo->name(), colinfo->childrenClassInfo()->createObjectFunction() );
				}
			}
		}
	}
}


/*!
Called by Classes::setup(). You should make sure you call it after creating new classes
(using DynamicObjects).
*/
void Classes::setupRelations()
{
	// Browse all relations and collections.
	// And add new non-browsable collections where needed.
	QVector<QStringList> list;

	ClassInfoConstIterator it2( Classes::begin() );
	ClassInfoConstIterator end2( Classes::end() );
	ClassInfo *classInfo;
	RelationInfo *relInfo;
	for ( ; it2 != end2; ++it2 ) {
		//classInfo = it2.data(); // DEPRECATED Qt3 -> Qt4 by Percy
        classInfo = it2.value();
		RelationInfosConstIterator rit( classInfo->relationsBegin() );
		RelationInfosConstIterator rend( classInfo->relationsEnd() );
		for ( ; rit != rend; ++rit ) {
			// relInfo = rit.data(); // DEPRECATED Qt3 -> Qt4 by Percy
            relInfo = rit.value();
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
			// colInfo = cit.data();  // DEPRECATED Qt3 -> Qt4 by Percy
            colInfo = cit.value();
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
	for ( int i = 0; i < list.count(); ++i ) {
		QStringList str = list[ i ];
		ClassInfo *t = Classes::classInfo( str[0] );
		t->addCollection( str[1], str[2] );
		t->collection( str[2] )->setBrowsable( false );
		t->collection( str[2] )->setNToOne( str[3] == "true" );
	}
}

/*!
Called by Classes::setup(). You should make sure you call it after creating new classes
(using DynamicObjects).
Creates the class hierarchy.
*/
void Classes::setupHierarchy()
{
	ClassInfoConstIterator it( Classes::begin() );
	ClassInfoConstIterator end( Classes::end() );
	ClassInfo *classInfo;
	for ( ; it != end; ++it ) {
		classInfo = *it;
		Object* obj = classInfo->createInstance();
        // DEPRECATED Qt3 -> Qt4 by Percy
// 		if ( obj->metaObject()->superClassName() ) {
// 			QString super = obj->metaObject()->superClassName();
// 			if ( Classes::contains( super ) ) {
// 				classInfo->setParent( Classes::classInfo( super ) );
// 				Classes::classInfo( super )->addChild( classInfo );
// 			}
// 		}

        if ( obj->metaObject()->superClass()->className() )
        {
          QString super(obj->metaObject()->superClass()->className());
          if ( Classes::contains( super ) )
          {
              classInfo->setParent( Classes::classInfo( super ) );
              Classes::classInfo( super )->addChild( classInfo );
          }
        }

		delete obj;
	}
}

/*!
Registers a new class to the list of known classes. This function is used by DeclareClass.
@param name The name of the class.
@param createInstance The pointer to the function that allows the creation of objects of the type class given by name.
@param createRelations The pointer to the function that creates the relations of the class.
*/
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

/*!
Returns an iterator pointing to the first class
*/
ClassInfoIterator Classes::begin()
{
	if ( ! m_classes )
		m_classes = new ClassInfoMap();

	return m_classes->begin();
}

/*!
Returns an iterator pointing to the last class
*/
ClassInfoIterator Classes::end()
{
	 if ( ! m_classes )
		m_classes = new ClassInfoMap();

	return m_classes->end();
}

ClassInfoConstIterator Classes::constBegin()
{
	 if ( ! m_classes )
		m_classes = new ClassInfoMap();

	return m_classes->constBegin();
}

ClassInfoConstIterator Classes::constEnd()
{
	 if ( ! m_classes )
		m_classes = new ClassInfoMap();

	return m_classes->constEnd();
}

/*!
Checks if the given class exists.
@param name The name of the class to search for.
@return true if the class exists, false otherwise.
*/
bool Classes::contains( const QString& name )
{
	return m_classes->contains( name );
}

/*!
Gets the information of a given class.
@param name The name of the class to search for.
@return A pointer to the ClassInfo structure of the given class.
*/
ClassInfo* Classes::classInfo( const QString& name )
{
	return (*m_classes)[ name ];
}

/*!
Gets the current class which is being created. This is a convenience class used by the OBJECT, COLLECTION and COLLECTIONN macros.
@return The pointer to the class which is being created.
*/
ClassInfo* Classes::currentClass()
{
	return m_currentClass;
}

