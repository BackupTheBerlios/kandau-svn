/***************************************************************************
 *   Copyright (C) 2005 by Albert Cervera Areny   *
 *   albertca@hotpop.com   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include <assert.h>

#include <qsqldatabase.h>
#include <qvaluevector.h>
#include <qregexp.h>

#include <kdebug.h>

#include "object.h"
#include "manager.h"
#include "dbbackendiface.h"
#include "testbackend.h"
#include "notificationhandler.h"

Manager* Manager::m_self = 0;

/* ObjectHandler */

/*!
Created for convenience only. Needed by QMap.
*/
ObjectHandler::ObjectHandler()
{
	m_object = 0;
	m_valid = false;
}

ObjectHandler::ObjectHandler( Object* object )
{
	m_object = object;
	m_valid = true;
}

void ObjectHandler::setValid( bool valid )
{
	m_valid = valid;
}

bool ObjectHandler::isValid() const
{
	return m_valid;
}

void ObjectHandler::setObject( Object* object )
{
	m_object = object;
}

Object* ObjectHandler::object() const
{
	return m_object;
}

/* RelationHandler */

RelationHandler::RelationHandler()
{
	m_oid = 0;
	m_modified = false;
	m_valid = false;
}

RelationHandler::RelationHandler( const OidType& oid, bool modified )
{
	m_oid = oid;
	m_modified = modified;
	m_valid = true;
}

void RelationHandler::setValid( bool valid )
{
	m_valid = valid;
}

bool RelationHandler::isValid() const
{
	return m_valid;
}

void RelationHandler::setOid( const OidType& oid )
{
	m_oid = oid;
}

const OidType& RelationHandler::oid() const
{
	return m_oid;
}

void RelationHandler::setModified( bool modified )
{
	m_modified = modified;
}

bool RelationHandler::isModified() const
{
	return m_modified;
}

/* CollectionHandler */

CollectionHandler::CollectionHandler()
{
	m_collection = 0;
	m_valid = false;
}

CollectionHandler::CollectionHandler( Collection *collection )
{
	m_collection = collection;
	m_valid = true;
}

void CollectionHandler::setValid( bool valid )
{
	m_valid = valid;
}

bool CollectionHandler::isValid() const
{
	return m_valid;
}

void CollectionHandler::setCollection( Collection* collection )
{
	m_collection = collection;
}

Collection* CollectionHandler::collection() const
{
	return m_collection;
}

/* Reference */

Reference::Reference()
{
	m_oid = 0;
	m_name = QString::null;
}

Reference::Reference( const Reference& reference )
{
	m_oid = reference.m_oid;
	m_name = reference.m_name;
}

Reference::Reference( const OidType& oid, const QString& name )
{
	m_oid = oid;
	m_name = name;
}

bool Reference::operator==( const Reference& reference ) const
{
	return m_oid == reference.m_oid && m_name == reference.m_name;
}

bool Reference::operator<( const Reference& reference ) const
{
	return ( oidToString( m_oid ) + "XX" + m_name ) < 
		( oidToString( reference.m_oid ) + "XX" + reference.m_name );
}

void Reference::setOid( const OidType& oid )
{
	m_oid = oid;
}

const OidType& Reference::oid() const
{
	return m_oid;
}

void Reference::setName( const QString& name )
{
	m_name = name;
}

const QString& Reference::name() const
{
	return m_name;
}

/* Manager */

Manager::Manager( DbBackendIface *backend, NotificationHandler *handler )
{
	assert( backend );
	m_dbBackend = backend;
	m_notificationHandler = handler;
	m_self = this;
	m_maxObjects = MaxObjects;
	m_cachePolicy = FreeMaxOnLoad;
	// Initialize the backend (maybe it needs to initialize some
	// values of the manager such as maxObjects or fill the map of objects in a
	// memory only backend).
	m_dbBackend->setup( this );
}

Manager::~Manager()
{
	// Backend hook
	m_dbBackend->shutdown();
	
	ManagerObjectIterator it( m_objects.begin() );
	ManagerObjectIterator end( m_objects.end() );
	for ( ; it != end; ++it )
		delete it.data().object();
	
	ManagerCollectionIterator it2( m_collections.begin() );
	ManagerCollectionIterator end2( m_collections.end() );
	for ( ; it2 != end2; ++it2 )
		delete it2.data().collection();
	assert( m_dbBackend );
	delete m_dbBackend;
	m_self = 0;
}

Manager* Manager::self()
{
	return m_self;
}

void Manager::setCachePolicy( CachePolicy policy )
{
	m_cachePolicy = policy;
}

Manager::CachePolicy Manager::cachePolicy() const
{
	return m_cachePolicy;
}

void Manager::setMaxObjects( Q_ULLONG max )
{
	m_maxObjects = max;
}

Q_ULLONG Manager::maxObjects() const
{
	return m_maxObjects;
}

/*!
This function prints with kdDebug() some information about the current status of the Manager. It is used for debugging purposes.
Shown information includes:
	* Number of objects kept in memory 
	* Number of relations kept in memory
	* Number of collections kept in memory
*/
void Manager::status() const
{
	QString info;

	kdDebug() << "Number of objects: " << m_objects.count() << endl;
	ManagerObjectConstIterator oit( m_objects.begin() );
	ManagerObjectConstIterator oend( m_objects.end() );
	for ( ; oit!=oend; ++oit ) {
		info += " " + oit.data().object()->oid();
	}
	kdDebug() << "Objects: " << info << endl;

	info = "";
	kdDebug() << "Number of relations: " << m_relations.count()  << endl;
	ManagerRelationConstIterator rit( m_relations.begin() );
	ManagerRelationConstIterator rend( m_relations.end() );
	for ( ; rit != rend; ++rit ) {
		info += " " + oidToString( rit.key().oid() ) + " " + rit.key().name();
	}
	kdDebug() << "Objects with relations in memory: " << info << endl;

	info = "";
	kdDebug() << "Number of collections: " << m_collections.count() << endl;
	ManagerCollectionConstIterator cit( m_collections.begin() );
	ManagerCollectionConstIterator cend( m_collections.end() );
	for ( ; cit != cend; ++cit ) {
		info += " " + oidToString( cit.key().oid() ) + " " + rit.key().name();
	}
	kdDebug() << "Objects with collections in memory: " << info << endl;
}

bool Manager::add( Object* object )
{
	assert( object );

	if ( object->isNull() )
		object->setOid( m_dbBackend->newOid() );

	if ( m_cachePolicy == FreeMaxOnLoad || m_cachePolicy == FreeAllOnLoad )
		ensureUnderMaxObjects();
	m_objects.insert( object->oid(), ObjectHandler( object ) );
	checkObjects();
	return true;
}

/*!
Removes an object from the persistency system and from memory.
TODO: What about data integrity?
@return Should return false if there was an error removing the object. Currently always returns true.
*/
bool Manager::remove( Object* object )
{
	assert( object );
	assert( object->oid() );
	assert( m_objects.count() > 0 );
	uint num = m_objects.count() - 1;
	//Backend hook
	m_dbBackend->beforeRemove( object );
	m_objects.remove( object->oid() );
	delete object;
	object = 0;
	assert( m_objects.count() == num );
	checkObjects();
	return true;
}

bool Manager::load( Collection* collection )
{
	checkObjects();
	return m_dbBackend->load( collection );
}

bool Manager::load( Collection* collection, const QString& query )
{
	// Add all the objects already loaded of className query to
	// the collection
	checkObjects();
	ManagerObjectIterator it( m_objects.begin() );
	ManagerObjectIterator end( m_objects.end() );
	QString className = query.lower();
	collection->clear();
	Object *obj;
	for ( ; it != end; ++it ) {
		obj = it.data().object();
		if ( obj->classInfo()->name().lower() == className ) {
			collection->simpleAdd( obj->oid() );
		}
	}
	return m_dbBackend->load( collection, query );
}

/*!
Commits the current transaction
@return true if the transaction was commited correctly. False otherwise.
*/
bool Manager::commit()
{
	checkObjects();
	if ( m_dbBackend->commit() ) {
		setEverythingUnmodified();
		ensureUnderMaxObjects();
		ensureUnderMaxRelations();
		ensureUnderMaxCollections();
		checkObjects();
		return true;
	}
	checkObjects();
	return false;
}

/*!
Sets objects and relations as unmodified. Used by commit once the backend has returned true from its commit function.
*/
void Manager::setEverythingUnmodified()
{
	ManagerObjectIterator mit( m_objects.begin() );
	ManagerObjectIterator mend( m_objects.end() );
	for ( ; mit != mend; ++mit ) {
		mit.data().object()->setModified( false );
		mit.data().setValid( false );
	}
	ManagerRelationIterator oit( m_relations.begin() );
	ManagerRelationIterator oend( m_relations.end() );
	for ( ; oit != oend; ++oit ) {
		oit.data().setModified( false );
		oit.data().setValid( false );
	}
	ManagerCollectionIterator cit( m_collections.begin() );
	ManagerCollectionIterator cend( m_collections.end() );
	for ( ; cit != cend; ++cit ) {
		cit.data().collection()->setModified( false );
		cit.data().setValid( false );
	}
}

/*!
Aborts the current transaction.
@return Should return false if couldn't rollback. Currently always returns true.
*/
bool Manager::rollback()
{
	// Delete all modified objects
	ManagerObjectIterator oit( m_objects.begin() );
	ManagerObjectIterator oend( m_objects.end() );
	QValueList<OidType> olist;
	for ( ; oit != oend; ++oit ) {
		oit.data().setValid( false );
		if ( oit.data().object()->isModified() )
			olist.append( oit.key() );
	}
	QValueListIterator<OidType> olit( olist.begin() );
	QValueListIterator<OidType> olend( olist.end() );
	for ( ; olit != olend; ++olit ) {
		delete m_objects[ *olit ].object();
		m_objects.remove( *olit );
	}
	olist.clear();
	ensureUnderMaxObjects();

	ManagerRelationIterator rit ( m_relations.begin() );
	ManagerRelationIterator rend ( m_relations.end() );
	QValueList<Reference> rlist;
	for ( ; rit != rend; ++rit ) {
		rit.data().setValid( false );
		if ( rit.data().isModified() )
			rlist.append( rit.key() );
	}
	QValueListIterator<Reference> rlit( rlist.begin() );
	QValueListIterator<Reference> rlend( rlist.end() );
	for ( ; rlit != rlend; ++rlit ) {
		m_relations.remove( *rlit );
	}
	rlist.clear();
	ensureUnderMaxRelations();

	// Delete all modified collections
	ManagerCollectionIterator cit( m_collections.begin() );
	ManagerCollectionIterator cend( m_collections.end() );
	for ( ; cit != cend; ++cit ) {
		cit.data().setValid( false );
		if ( cit.data().collection()->modified() )
			rlist.append( cit.key() );
	}
	rlit = rlist.begin();
	rlend = rlist.end();
	for ( ; rlit != rlend; ++rlit ) {
		m_collections.remove( *rlit );
	}
	rlist.clear();
	ensureUnderMaxCollections();

	// Backend callback
	m_dbBackend->afterRollback();

	return true;
}

Object* Manager::load( OidType oid, const ClassInfo* info )
{
	// This shouldn't be necessary but it is here because
	// GETOBJECT macro doesn't work correctly when you try to
	// load an object which hasn't been initialized. Could be solved
	// there too. So let's put a TODO
	// The problem exists with TestBackend because always returns true, but
	// with other backends it would simply be suboptimal/inefficient
	if ( oid == 0 )
		return 0;
	assert( info );
	checkObjects();
	Object *object;
	if ( m_objects.contains( oid ) ) {
		// If object has been loaded in this transaction return it
		if ( m_objects[ oid ].isValid() ) {
			return m_objects[ oid ].object();
		}
		// Otherwise check if it has changed;
		object = m_objects[ oid ].object();
		if ( ! m_dbBackend->hasChanged( object ) ) {
			// Has it hasn't changed set it as valid for this transaction too
			m_objects[ oid ].setValid( true );
			return object;
		}
		// The object might have been deleted so we remove
		// it and we'll see what happens next
		delete object;
		m_objects.remove( oid );
	}
	object = info->createInstance();
	if ( ! object )
		return 0;
	object->setManager( this );
	// IMPORTANT: This will set the ClassInfo for the object!! Necessary for DynamicObjects
	object->setClassInfo( info );
	if ( ! m_dbBackend->load( oid, object ) ) {
		delete object;
		return 0;
	}
	// Call before m_objects.insert as it might free the just added object.
	if ( m_cachePolicy == FreeMaxOnLoad || m_cachePolicy == FreeAllOnLoad )
		ensureUnderMaxObjects();
	m_objects.insert( oid, ObjectHandler( object ) );
	checkObjects();
	return object;
}

/*!
Returns a pointer to the requested object.
@param oid The oid of the object
@param f The pointer to the function that creates an object of the type of the expected object
*/
Object* Manager::load( OidType oid, CreateObjectFunction f )
{
	// This shouldn't be necessary but it is here because
	// GETOBJECT macro doesn't work correctly when you try to
	// load an object which hasn't been initialized. Could be solved
	// there too. So let's put a TODO
	// The problem exists with TestBackend because always returns true, but
	// with other backends it would simply be suboptimal/inefficient

	if ( oid == 0 )
		return 0;
	checkObjects();
	Object *object;
	assert( f );
	if ( m_objects.contains( oid ) ) {
		// If object has been loaded in this transaction return it
		if ( m_objects[ oid ].isValid() ) {
			return m_objects[ oid ].object();
		}
		object = m_objects[ oid ].object();
		if ( ! m_dbBackend->hasChanged( object ) ) {
			m_objects[ oid ].setValid( true );
			return object;
		}
		// The object might have been deleted so we remove
		// it and we'll see what happens next
		delete object;
		m_objects.remove( oid );
	}
	object = (*f)();
	if ( ! object )
		return 0;
	object->setManager( this );

	if ( ! m_dbBackend->load( oid, object ) ) {
		delete object;
		return 0;
	}

	// Call before m_objects.insert as it might free the just added object.
	if ( m_cachePolicy == FreeMaxOnLoad || m_cachePolicy == FreeAllOnLoad )
		ensureUnderMaxObjects();

	m_objects.insert( oid, ObjectHandler( object ) );
	return object;
}

/*!
Creates the database schema. The type of schema depends on the current backend.
@returns true if the schema was created correctly. False otherwise.
*/
bool Manager::createSchema()
{
	return m_dbBackend->createSchema();
}

void Manager::checkObjects()
{
	ManagerObjectConstIterator it( m_objects.begin() );
	ManagerObjectConstIterator end( m_objects.end() );
	for ( ; it != end; ++it ) {
		assert( it.key() );
		assert( it.data().object() );
	}
}

/*!
Ensures the total number of objects stays under maxObjects() as long as there
are unmodified objects. Right now the parameter isn't used anywhere as we call this
function before adding the object to the map. We'll see if it's necessary somewhere
in the future, we could remove the parameter if isn't useful.
*/
void Manager::ensureUnderMaxObjects()
{
	if ( m_objects.count() < m_maxObjects )
		return;

	ManagerObjectConstIterator it( m_objects.begin() );
	ManagerObjectConstIterator end( m_objects.end() );
	QValueVector<OidType> list;
	Object *obj;
	for ( ; it != end; ++it ) {
		// Only invalid objects can be freed
		if ( it.data().isValid() )
			continue;
		obj = it.data().object();
		if ( ! obj->isModified() ) {
			list.append( it.key() );
			if ( ( m_cachePolicy == FreeMaxOnLoad || 
				m_cachePolicy == FreeMaxOnTransaction ) &&
				m_objects.count() - list.count() <= m_maxObjects )
				break;
		}
	}
	QValueVector<OidType>::const_iterator vIt( list.begin() );
	QValueVector<OidType>::const_iterator vEnd( list.end() );
	for ( ; vIt != vEnd; ++vIt ) {
		assert( m_objects.contains( *vIt ) );
		delete m_objects[ *vIt ].object();
		m_objects.remove( *vIt );
		assert( ! m_objects.contains( *vIt ) );
	}
	checkObjects();
}

void Manager::ensureUnderMaxRelations()
{
	if ( m_relations.count() < m_maxObjects )
		return;

	QValueVector<Reference> list;
	ManagerRelationConstIterator it( m_relations.constBegin() );
	ManagerRelationConstIterator end( m_relations.constEnd() );
	for ( ; it != end; ++it ) {
		// If the object is not valid it can't have been modified
		if ( !it.data().isValid() ) {
			list.append( it.key() );
			if ( ( m_cachePolicy == FreeMaxOnLoad || m_cachePolicy == FreeMaxOnTransaction ) && m_relations.count() - list.count() <= m_maxObjects )
				break;
		}
	}
	QValueVector<Reference>::const_iterator vIt( list.constBegin() );
	QValueVector<Reference>::const_iterator vEnd( list.constEnd() );
	for ( ; vIt != vEnd; ++vIt ) {
		m_relations.remove( *vIt );
	}
}

void Manager::ensureUnderMaxCollections()
{
	if ( m_collections.count() < m_maxObjects )
		return;

	QValueVector<Reference> list;
	ManagerCollectionConstIterator it( m_collections.constBegin() );
	ManagerCollectionConstIterator end( m_collections.constEnd() );
	for ( ; it != end; ++it ) {
		// If the object is not valid it can't have been modified
		if ( !it.data().isValid() ) {
			list.append( it.key() );
			if ( ( m_cachePolicy == FreeMaxOnLoad || m_cachePolicy == FreeMaxOnTransaction ) && m_collections.count() - list.count() <= m_maxObjects )
				break;
		}
	}
	QValueVector<Reference>::const_iterator vIt( list.constBegin() );
	QValueVector<Reference>::const_iterator vEnd( list.constEnd() );
	for ( ; vIt != vEnd; ++vIt ) {
		m_collections.remove( *vIt );
	}
}

/*
Relation management functions
*/

void Manager::setRelation( const OidType& oid, const ClassInfo* classInfo, const QString& relationName, const OidType& oidRelated, bool recursive )
{
	assert( classInfo );
	QString relstr = ClassInfo::relationName( relationName, classInfo->name() );

	bool isCollection = false;
	QString relatedClassName;

	if ( classInfo->containsCollection( relstr ) ) {
		isCollection = true;
		CollectionInfo* col = classInfo->collection( relstr );
		relatedClassName = col->childrenClassInfo()->name();
	} else if ( classInfo->containsObject( relstr ) ) {
		isCollection = false;
		RelationInfo* obj = classInfo->object( relstr );
		relatedClassName = obj->relatedClassInfo()->name();
	} else {
		if ( ! recursive ) {
			// It is not browseable from this object so we return
			return;
		} else {
			// There is an error !!!
			kdDebug() << "ClassInfo: " << classInfo->name() << ", Relation: " << relstr << endl;
			assert( false );
		}
	}

	if ( m_cachePolicy == FreeMaxOnLoad || m_cachePolicy == FreeAllOnLoad )
		ensureUnderMaxRelations();
		
	OidType oldOid = relation( oid, classInfo->object( relstr ) );
	if ( oldOid != 0 ) {
		if ( oldOid != oidRelated ) {
			if ( ! isCollection ) {
				m_relations[ Reference( oldOid, relstr ) ].setOid( 0 );
				m_relations[ Reference( oldOid, relstr ) ].setModified( true );
			} else {
				removeRelation( oldOid, classInfo->collection( relstr ), oid, false );
			}
		}
	}
	Reference ref( oid, relstr );
	m_relations[ ref ].setOid( oidRelated );
	m_relations[ ref ].setModified( true );
	if ( recursive && oidRelated != 0 ) {
		if ( ! isCollection ) {
			setRelation( oidRelated, classInfo, relstr, oid, false );
		} else {
			addRelation( oidRelated, classInfo->collection( relstr ), oid, false );
		}
	}
}

void Manager::addRelation( const OidType& oid, const CollectionInfo* relcol, const OidType& oidRelated, bool recursive )
{
	QString relation = ClassInfo::relationName( relcol->name(), relcol->parentClassInfo()->name() );
	
	Collection *col = collection( oid, relcol );
	col->simpleAdd( oidRelated );

	// EnsureUnderMaxCollections AFTER modifying it (simpleAdd) we couldn't ensure
	// the collection was still in memory otherwise.
	if ( m_cachePolicy == FreeMaxOnLoad || m_cachePolicy == FreeAllOnLoad )
		ensureUnderMaxCollections();

	if ( recursive ) {
		if ( Classes::classInfo( relcol->childrenClassInfo()->name() )->containsObject( relation ) ) {
			setRelation( oidRelated, relcol->childrenClassInfo(), relation, oid, false );
		} else if ( Classes::classInfo( relcol->childrenClassInfo()->name() )->containsCollection( relation ) ) {
			addRelation( oidRelated, relcol->childrenClassInfo()->collection( relation ), oid, false );
		} else {
			// It is not browseable from the other object
			return;
		}
	}
}

void Manager::removeRelation( const OidType& oid, const CollectionInfo* relcol, const OidType& oidRelated, bool recursive )
{
	QString relation = ClassInfo::relationName( relcol->name(), relcol->parentClassInfo()->name() );
	
	Collection *col = collection( oid, relcol );
	col->simpleRemove( oidRelated );

	if ( recursive ) {
		if ( Classes::classInfo( relcol->childrenClassInfo()->name() )->containsObject( relation ) ) {
			setRelation( oidRelated, relcol->childrenClassInfo(), 0, false );
		} else if ( Classes::classInfo( relcol->childrenClassInfo()->name() )->containsCollection( relation ) ) {
			removeRelation( oidRelated, relcol->childrenClassInfo()->collection( relation ), oid, false );
		} else {
			// It is not browseable from the other object
			return;
		}
	}
}

/*!
Obtains the oid of the object that relates to the relation "related" for object with oid "oid". It assumes that the object with oid "oid" can have a relation of type "related".
*/
OidType Manager::relation( const OidType& oid, const RelationInfo* related )
{
	Reference ref( oid, related->name() );
	
	if ( m_relations.contains( ref ) ) {
		RelationHandler r = m_relations[ ref ];
		if ( r.isValid() )
			return r.oid();

		if ( ! m_dbBackend->hasChanged( oid, related ) ) {
			// As it hasn't changed set it as valid for this transaction too
			m_relations[ ref ].setValid( true );
			return m_relations[ ref ].oid();
		}
	}
	OidType relOid = 0;
	m_dbBackend->load( &relOid, oid, related );
	m_relations[ ref ].setOid( relOid );
	m_relations[ ref ].setValid( true );
	m_relations[ ref ].setModified( false );
	if ( m_cachePolicy == FreeMaxOnLoad || m_cachePolicy == FreeAllOnLoad )
		ensureUnderMaxRelations();
	return relOid;
}

OidType Manager::relation( const Object* object, const QString& related )
{
	assert( object->classInfo()->object( ClassInfo::relationName( related, object->classInfo()->name() ) ) );
	return relation( object->oid(), object->classInfo()->object( ClassInfo::relationName( related, object->classInfo()->name() ) ) );
}

Collection* Manager::collection( const OidType& oid, const CollectionInfo* related )
{
	// TODO: We are assuming the relation exists!!!
	Reference ref( oid, related->name() );
	if ( m_collections.contains( ref ) ) {
		if ( m_collections[ ref ].isValid() )
			return m_collections[ ref ].collection();

		if ( ! m_dbBackend->hasChanged( m_collections[ ref ].collection() ) ) {
			// As it hasn't changed set it as valid for this transaction too
			m_collections[ ref ].setValid( true );
			return m_collections[ ref ].collection();
		}
	}
	Collection *col = new Collection( related, oid, this );
	m_dbBackend->load( col );
	m_collections[ ref ].setCollection( col );
	m_collections[ ref ].setValid( true );
	if ( m_cachePolicy == FreeMaxOnLoad || m_cachePolicy == FreeAllOnLoad )
		ensureUnderMaxCollections();
	return col;
}


/*!
For testing purposes only. Used by the tests to ensure no objects remain from the previous test.
*/
void Manager::reset()
{
	ManagerObjectIterator it( m_objects.begin() );
	ManagerObjectIterator end( m_objects.end() );
	for ( ; it != end; ++it )
		delete it.data().object();
	m_objects.clear();
	ManagerCollectionIterator cit( m_collections.begin() );
	ManagerCollectionIterator cend( m_collections.end() );
	for ( ; cit != cend; ++cit )
		delete cit.data().collection();
	m_collections.clear();

	m_relations.clear();

	m_dbBackend->reset();
}

/*!
Copies the contents (objects & relations) to another manager. Only objects and relations that are currently in memory are copied ATM. This means it will only copy the whole data in those src managers that have MaxObjects Unlimited. So it is possible to copy the whole Xml database to an Sql Database but not viceversa.
@param manager The manager to which the contents of the current manager will be copied.
*/
void Manager::copyTo( Manager* manager )
{
	// There are no pointers in this structure so we can copy it using the default assignment operator
	manager->m_relations = m_relations;
	
	ManagerObjectIterator it( m_objects.begin() );
	ManagerObjectIterator end( m_objects.end() );
	Object *srcObj, *dstObj;
	for ( ; it != end; ++it ) {
		srcObj = it.data().object();
		dstObj = srcObj->classInfo()->create( srcObj->oid(), manager, true );
		*dstObj = *srcObj;
		dstObj->setModified( true );
	}

	ManagerCollectionIterator cit( m_collections.begin() );
	ManagerCollectionIterator cend( m_collections.end() );
	Collection *col, *srcCol;
	for ( ; cit != cend; ++cit ) {
		srcCol = cit.data().collection();
		col = new Collection( srcCol->collectionInfo(), srcCol->parentOid(), manager );
		*col = *srcCol;
		manager->m_collections.insert( cit.key(), CollectionHandler( col ) );
	}
}

ManagerObjectMap& Manager::objects()
{
	return m_objects;
}

ManagerRelationMap& Manager::relations()
{
	return m_relations;
}

ManagerCollectionMap& Manager::collections()
{
	return m_collections;
}

bool Manager::notifyPropertyModified( const Object* object, const QString& function, const QVariant& value )
{
	if ( ! m_notificationHandler )
		return true;

	QString property;
	// Remove the 'set' prefix
	property = function.right( function.length() - 3 );
	// Substitute capital letters with underscore + lowercase
	property.replace( QRegExp( "([A-Z])" ), "_\\1" );
	property = property.lower();
	if ( property.left( 1 ) == "_" )
		property = property.right( property.length() -1 );
	kdDebug() << k_funcinfo << property << endl;
	
	return m_notificationHandler->propertyModified( object->classInfo(), object->oid(), property, value );
}

void Manager::setNotificationHandler( NotificationHandler* handler )
{
	m_notificationHandler = handler;
}

NotificationHandler* Manager::notificationHandler() const
{
	return m_notificationHandler;
}
