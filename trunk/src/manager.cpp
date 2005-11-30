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


Manager::Manager( DbBackendIface *backend, NotificationHandler *handler )
{
	assert( backend );
	m_dbBackend = backend;
	m_notificationHandler = handler;
	m_self = this;
	m_maxObjects = MaxObjects;
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
		delete it.data();
	
	ManagerRelatedCollectionIterator it2( m_collections.begin() );
	ManagerRelatedCollectionIterator end2( m_collections.end() );
	for ( ; it2 != end2; ++it2 ) {
		QMapIterator<QString, Collection*> it3( (*it2).begin() );
		QMapIterator<QString, Collection*> end3( (*it2).end() );
		for ( ; it3 != end3; ++it3 ) 
			delete (*it3);
	}
	assert( m_dbBackend );
	delete m_dbBackend;
	m_self = 0;
}

Manager* Manager::self()
{
	return m_self;
}

void Manager::setMaxObjects( Q_ULLONG max )
{
	m_maxObjects = max;
}

Q_ULLONG Manager::maxObjects() const
{
	return m_maxObjects;
}

uint Manager::count() const
{
	return m_objects.count();
}

uint Manager::countObjectRelations() const
{
	return m_relations.count();
}

uint Manager::countCollectionRelations() const
{
	return m_collections.count();
}

void Manager::status() const
{
	QString info;

	kdDebug() << "Number of objects: " << m_objects.count() << endl;
	ManagerObjectConstIterator oit( m_objects.begin() );
	ManagerObjectConstIterator oend( m_objects.end() );
	for ( ; oit!=oend; ++oit ) {
		info += " " + (*oit)->oid();
	}
	kdDebug() << "Objects: " << info << endl;

	info = "";
	kdDebug() << "Number of relations: " << m_relations.count()  << endl;
	ManagerRelatedObjectConstIterator rit( m_relations.begin() );
	ManagerRelatedObjectConstIterator rend( m_relations.end() );
	for ( ; rit != rend; ++rit ) {
		info += " " + rit.key();
	}
	kdDebug() << "Objects with relations in memory: " << info << endl;

	info = "";
	kdDebug() << "Number of collections: " << m_collections.count() << endl;
	ManagerRelatedCollectionConstIterator cit( m_collections.begin() );
	ManagerRelatedCollectionConstIterator cend( m_collections.end() );
	for ( ; cit != cend; ++cit ) {
		info += " " + cit.key();
	}
	kdDebug() << "Objects with collections in memory: " << info << endl;
}

bool Manager::add( Object* object )
{
	assert( object );

	if ( object->isNull() )
		object->setOid( m_dbBackend->newOid() );
	// Call before m_objects.insert as it might free the just added object.
	ensureUnderMaxObjects();
	m_objects.insert( object->oid(), object );
	checkObjects();
	return true;
}

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
	for ( ; it != end; ++it ) {
		if ( it.data()->classInfo()->name().lower() == className ) {
			collection->simpleAdd( it.data()->oid() );
		}
	}
	return m_dbBackend->load( collection, query );
}

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

void Manager::setEverythingUnmodified()
{
	ManagerObjectIterator mit( begin() );
	ManagerObjectIterator mend( end() );
	for ( ; mit != mend; ++mit )
		(*mit)->setModified( false );

	ManagerRelatedObjectIterator oit( m_relations.begin() );
	ManagerRelatedObjectIterator oend( m_relations.end() );
	for ( ; oit != oend; ++oit ) {
		QMapIterator<QString, QPair<OidType, bool> > oit2( (*oit).begin() );
		QMapIterator<QString, QPair<OidType, bool> > oend2( (*oit).end() );
		for ( ; oit2 != oend2; ++oit2 )
			(*oit2).second = false;
	}

	ManagerRelatedCollectionIterator cit( m_collections.begin() );
	ManagerRelatedCollectionIterator cend( m_collections.end() );
	for ( ; cit != cend; ++cit ) {
		QMapIterator<QString, Collection*> cit2( (*cit).begin() );
		QMapIterator<QString, Collection*> cend2( (*cit).end() );
		for ( ; cit2 != cend2; ++cit2 )
			(*cit2)->setModified( false );
	}
}

bool Manager::rollback()
{
	// Delete all modified objects
	ManagerObjectIterator oit( m_objects.begin() );
	ManagerObjectIterator oend( m_objects.end() );
	QValueList<OidType> olist;
	for ( ; oit != oend; ++oit ) {
		if ( (*oit)->isModified() )
			olist.append( oit.key() );
	}
	QValueListIterator<OidType> lit( olist.begin() );
	QValueListIterator<OidType> lend( olist.end() );
	for ( ; lit != lend; ++lit ) {
		delete m_objects[ *lit ];
		m_objects.remove( *lit );
	}
	olist.clear();

	// Delete all modified relations
	//TODO: Is it necessary to delete an object if it has a relation modified
	ManagerRelatedObjectIterator rit ( m_relations.begin() );
	ManagerRelatedObjectIterator rend ( m_relations.end() );
	for ( ; rit != rend; ++rit ) {
		removeObjectReferences( *rit, Modified );
		if ( (*rit).count() == 0 )
			olist.append( rit.key() );
	}
	lit = olist.begin();
	lend = olist.end();
	for ( ; lit != lend; ++lit ) {
		if ( m_objects.contains( *lit ) ) {
			kdDebug() << "deleting object due to its modified relations" << endl;
			delete m_objects[ *lit ];
			m_objects.remove( *lit );
		}
		m_relations.remove( *lit );
	}
	olist.clear();

	// Delete all modified collections
	ManagerRelatedCollectionIterator cit( m_collections.end() );
	ManagerRelatedCollectionIterator cend( m_collections.end() );
	for ( ; cit != cend; ++cit ) {
		removeCollectionReferences( *cit, Modified );
		if ( (*cit).count() == 0 )
			olist.append( cit.key() );
	}
	lit = olist.begin();
	lend = olist.end();
	for ( ; lit != lend; ++lit ) {
		if ( m_objects.contains( *lit ) ) {
			kdDebug() << "deleting object due to its modified relations" << endl;
			delete m_objects[ *lit ];
			m_objects.remove( *lit );
		}
		m_collections.remove( *lit );
	}
	olist.clear();

	// Backend callback
	m_dbBackend->afterRollback();

	return true;
}

/*

	// Delete all modified relations
	QMapIterator <OidType, QMap<QString, QPair<OidType, bool> > > rit ( m_relations.begin() );
	QMapIterator <OidType, QMap<QString, QPair<OidType, bool> > > rend ( m_relations.end() );
	for ( ; rit != rend; ++oit ) {
		QValueListIterator<QString> rlist;
		/QMapIterator<QString, QPair<OidType, bool> > rrit( (*rit).begin() );
		QMapIterator<QString, QPair<OidType, bool> > rrend( (*rit).end() );
		for (; rrit != rrend; ++rrit ) {
		  if ( (*rrit).second == true )
		    rlist.append( rrit.key() );
}
		QValueListIterator<QString> rlit( rlist.begin() );
		QValueListIterator<QString> rlend( rlist.end() );
		for (; rlit != rlend; ++rlit ) {
		  (*rit).remove( *rlit );
}
		if ( *rlit

}
	QValueListIterator<OidType> lit( olist.begin() );
	QValueListIterator<OidType> lend( olist.end() );
	for ( ; lit != lend; ++lit ) {
	  delete m_objects[ *lit ];
	  m_objects.remove( *lit );
}
*/

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
		object = m_objects[ oid ];
		if ( ! m_dbBackend->hasChanged( object ) )
			return object;
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
	ensureUnderMaxObjects();

	m_objects.insert( oid, object );
	checkObjects();
	return object;
}

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
		object = m_objects[ oid ];
		if ( ! m_dbBackend->hasChanged( object ) )
			return object;
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
	ensureUnderMaxObjects();

	m_objects.insert( oid, object );
	return object;
}

bool Manager::createSchema()
{
	return m_dbBackend->createSchema();
}

void Manager::ensureUnderMaxObjects( Object *object )
{
	if ( m_objects.count() < m_maxObjects )
		return;

	OidType oid = 0;
	if ( object )
		oid = object->oid();
		
	int total = m_objects.count();

	ManagerObjectConstIterator it( m_objects.begin() );
	ManagerObjectConstIterator end( m_objects.end() );
	QValueVector<OidType> list;
	for ( ; it != end; ++it ) {
		if ( ! it.data()->isModified() && it.key() != oid && m_lockedObjects.contains( it.data() ) == false ) {
			//delete it.data();
			list.append( it.key() );
			total--;
			if ( m_objects.count() - list.count() <= m_maxObjects )
				break;
		}
	}
	QValueVector<OidType>::const_iterator vIt( list.begin() );
	QValueVector<OidType>::const_iterator vEnd( list.end() );
	for ( ; vIt != vEnd; ++vIt ) {
		assert( m_objects.contains( *vIt ) );
		delete m_objects[ *vIt ];
		m_objects.remove( *vIt );
		kdDebug() << "Freed: " << oidToString( *vIt ) << endl;
		assert( ! m_objects.contains( *vIt ) );
		//removeObjectReferences( *vIt, Unmodified );
	}
	checkObjects();
	//assert( total == m_objects.count() );
}

void Manager::checkObjects()
{
	ManagerObjectConstIterator it( m_objects.begin() );
	ManagerObjectConstIterator end( m_objects.end() );
	for ( ; it != end; ++it ) {
		assert( it.key() );
		assert( it.data() );
	}
}

void Manager::ensureUnderMaxRelations()
{
	if ( m_relations.count() < m_maxObjects )
		return;
	bool modified;

	QValueVector<OidType> list;
	ManagerRelatedObjectConstIterator it( m_relations.constBegin() );
	ManagerRelatedObjectConstIterator end( m_relations.constEnd() );
	for ( ; it != end; ++it ) {
		modified = false;
		QMapConstIterator<QString, QPair<OidType, bool> > it2( (*it).constBegin() );
		QMapConstIterator<QString, QPair<OidType, bool> > end2( (*it).constEnd() );
		for ( ; it2 != end2; ++it2 ) {
			if ( (*it2).second ) {
				modified = true;
				break;
			}
		}
		if ( ! modified ) {
			list.append( it.key() );
			if ( m_relations.count() - list.count() <= m_maxObjects )
				break;
		}
	}
	QValueVector<OidType>::const_iterator vIt( list.constBegin() );
	QValueVector<OidType>::const_iterator vEnd( list.constEnd() );
	for ( ; vIt != vEnd; ++vIt ) {
		m_relations.remove( *vIt );
	}
}

void Manager::ensureUnderMaxCollections()
{
	if ( m_collections.count() < m_maxObjects )
		return;
	bool modified;

	QValueVector<OidType> list;
	ManagerRelatedCollectionConstIterator it( m_collections.constBegin() );
	ManagerRelatedCollectionConstIterator end( m_collections.constEnd() );
	for ( ; it != end; ++it ) {
		modified = false;
		QMapConstIterator<QString, Collection*> it2( (*it).constBegin() );
		QMapConstIterator<QString, Collection*> end2( (*it).constEnd() );
		for ( ; it2 != end2; ++it2 ) {
			if ( (*it2)->modified() ) {
				modified = true;
				break;
			}
		}
		if ( ! modified ) {
			list.append( it.key() );
			if ( m_collections.count() - list.count() <= m_maxObjects )
				break;
		}
	}
	QValueVector<OidType>::const_iterator vIt( list.constBegin() );
	QValueVector<OidType>::const_iterator vEnd( list.constEnd() );
	for ( ; vIt != vEnd; ++vIt ) {
		m_collections.remove( *vIt );
	}
}

void Manager::removeObjectReferences( QMap<QString, QPair<OidType, bool> > map, Filter filter )
{
  	bool boolFilter = filter == Modified ? true : false;

	QMapIterator<QString,QPair<OidType,bool> > it( map.begin() );
	QMapIterator<QString,QPair<OidType,bool> > end( map.end() );
	QValueList<QString> list;
	for ( ; it != end; ++it )
		if ( (*it).second == boolFilter )
			list.append( it.key() );
	QValueListIterator<QString> vIt( list.begin() );
	QValueListIterator<QString> vEnd( list.end() );
	for ( ; vIt != vEnd; ++vIt )
		map.remove( *vIt );
}

void Manager::removeObjectReferences( const OidType& oid, Filter filter )
{
        QMapIterator<OidType, QMap<QString, QPair<OidType, bool> > > it = m_relations.find( oid );
        if ( it == m_relations.end() )
                        return;
        removeObjectReferences( *it, filter );
        if ( (*it).count() == 0 )
                m_relations.remove( oid );
}

void Manager::removeCollectionReferences( QMap<QString, Collection*> map, Filter filter )
{
  	bool boolFilter = filter == Modified ? true : false;

	QMapIterator<QString, Collection*> it( map.begin() );
	QMapIterator<QString, Collection*> end( map.end() );
	QValueList<QString> list;
	for ( ; it != end; ++it )
		if ( (*it)->modified() == boolFilter )
			list.append( it.key() );
	QValueListIterator<QString> vIt( list.begin() );
	QValueListIterator<QString> vEnd( list.end() );
	for ( ; vIt != vEnd; ++vIt ) {
		delete map[ *vIt ];
		map.remove( *vIt );
	}
}

ManagerObjectIterator Manager::begin()
{
	return m_objects.begin();
}

ManagerObjectIterator Manager::end()
{
	return m_objects.end();
}


/*
	Relation management functions
*/

void Manager::setRelation( const OidType& oid, const ClassInfo* classInfo, const QString& relationName, const OidType& oidRelated, bool recursive )
{
	assert( classInfo );
	QString relation = ClassInfo::relationName( relationName, classInfo->name() );

	bool isCollection = false;
	QString relatedClassName;

	if ( classInfo->containsCollection( relation ) ) {
		isCollection = true;
		RelatedCollection* col = classInfo->collection( relation );
		relatedClassName = col->childrenClassInfo()->name();
	} else if ( classInfo->containsObject( relation ) ) {
		isCollection = false;
		RelatedObject* obj = classInfo->object( relation );
		relatedClassName = obj->relatedClassInfo()->name();
	} else {
		if ( ! recursive ) {
			// It is not browseable from this object so we return
			return;
		} else {
			// There is an error !!!
			kdDebug() << "ClassInfo: " << classInfo->name() << ", Relation: " << relation << endl;
			assert( false );
		}
	}

	ensureUnderMaxRelations();
	if ( m_relations.contains( oid ) ) {
		OidType old = m_relations[ oid ][ relation ].first;
		// Avoid removing the previously defined relation (we're probably inside the recursion)
		if ( old != oidRelated ) {
			if ( ! isCollection ) {
				if ( m_relations.contains( old ) ) {
					m_relations[ old ].remove( relation );
				}
			} else {
				removeRelation( old, classInfo->collection( relation ), oid, false );
			}
		}
		m_relations[ oid ][ relation ].first = oidRelated;
		m_relations[ oid ][ relation ].second = true; //Modified;
	} else {
		QMap<QString,QPair<OidType,bool> > map;
		map.insert( relation, QPair<OidType,bool>(oidRelated,true) ); // Modified
		m_relations[ oid ] = map;
	}
	if ( recursive && oidRelated != 0 ) {
		if ( ! isCollection ) {
			setRelation( oidRelated, classInfo, relation, oid, false );
		} else {
			addRelation( oidRelated, classInfo->collection( relation ), oid, false );
		}
	}
}

void Manager::addRelation( const OidType& oid, const RelatedCollection* collection, const OidType& oidRelated, bool recursive )
{
	QString relation = ClassInfo::relationName( collection->name(), collection->parentClassInfo()->name() );

	if ( m_collections[ oid ][ relation ] == 0 )
		m_collections[ oid ][ relation ] = new Collection( collection, oid, this );
	m_collections[ oid ][ relation ]->simpleAdd( oidRelated );
	
	// EnsureUnderMaxCollections AFTER modifying it (simpleAdd) we couldn't ensure
	// the collection was still in memory otherwise.
	ensureUnderMaxCollections();

	if ( recursive ) {
		if ( Classes::classInfo( collection->childrenClassInfo()->name() )->containsObject( relation ) ) {
			setRelation( oidRelated, collection->childrenClassInfo(), relation, oid, false );
		} else if ( Classes::classInfo( collection->childrenClassInfo()->name() )->containsCollection( relation ) ) {
			addRelation( oidRelated, collection->childrenClassInfo()->collection( relation ), oid, false );
		} else {
			// It is not browseable from the other object
			return;
		}
	}
}

void Manager::removeRelation( const OidType& oid, const RelatedCollection* collection, const OidType& oidRelated, bool recursive )
{
	QString relation = ClassInfo::relationName( collection->name(), collection->parentClassInfo()->name() );

	if ( ! m_collections.contains( oid ) )
		return;
	if ( ! m_collections[ oid ].contains( relation ) )
		return;

	m_collections[ oid ][ relation ]->simpleRemove( oidRelated );
/* 
We shouldn't remove the collection as that will make the system try to reload when accessed again.
	if ( m_collections[ oid ][ relation ]->count() == 0 ) {
		delete m_collections[ oid ][ relation ];
		m_collections[ oid ].remove( relation );
		if ( m_collections[ oid ].count() == 0 ) {
			m_collections.remove( oid );
		}
	}
*/
	if ( recursive ) {
		if ( Classes::classInfo( collection->childrenClassInfo()->name() )->containsObject( relation ) ) {
			setRelation( oidRelated, collection->childrenClassInfo(), 0, false );
		} else if ( Classes::classInfo( collection->childrenClassInfo()->name() )->containsCollection( relation ) ) {
			removeRelation( oidRelated, collection->childrenClassInfo()->collection( relation ), oid, false );
		} else {
			// It is not browseable from the other object
			return;
		}
	}
}

void Manager::setModifiedRelation( const OidType& oid, const ClassInfo* classInfo, const QString& relationName, bool modified, bool recursive )
{
	assert( classInfo );
	QString relation = ClassInfo::relationName( relationName, classInfo->name() );

	bool isCollection = false;
	QString relatedClassName;

	if ( classInfo->containsCollection( relation ) ) {
		isCollection = true;
		RelatedCollection* col = classInfo->collection( relation );
		relatedClassName = col->childrenClassInfo()->name();
	} else if ( classInfo->containsObject( relation ) ) {
		isCollection = false;
		RelatedObject* obj = classInfo->object( relation );
		relatedClassName = obj->relatedClassInfo()->name();
	} else {
		if ( ! recursive ) {
			// It is not browseable from this object so we return
			return;
		} else {
			// There is an error !!!
			kdDebug() << "ClassInfo: " << classInfo->name() << ", Relation: " << relation << endl;
			assert( false );
		}
	}
	OidType oidRelated = 0;
	if ( m_relations.contains( oid ) && m_relations[ oid ].contains( relation ) ) {
		oidRelated = m_relations[ oid ][ relation ].first;
		m_relations[ oid ][ relation ].second = modified;
	}
	if ( recursive && oidRelated != 0 ) {
		if ( ! isCollection ) {
			setModifiedRelation( oidRelated, classInfo, relationName, modified, false );
		} else {
			setModifiedRelation( oidRelated, classInfo->collection( relationName ), oid, modified, false );
		}
	}
}

void Manager::setModifiedRelation( const OidType& oid, RelatedCollection* collection, const OidType& oidRelated, bool modified, bool recursive )
{
	QString relation = ClassInfo::relationName( collection->name(), collection->parentClassInfo()->name() );

	
	if ( ! m_collections.contains( oid ) )
		return;
	if ( ! m_collections[ oid ].contains( relation ) )
		return;
	m_collections[ oid ][ relation ]->setModified( oidRelated, modified );
	if ( recursive ) {
		if ( Classes::classInfo( collection->childrenClassInfo()->name() )->containsObject( relation ) ) {
			setModifiedRelation( oidRelated, collection->childrenClassInfo(), collection->name(), modified, false );
		} else if ( Classes::classInfo( collection->childrenClassInfo()->name() )->containsCollection( relation ) ) {
			setModifiedRelation( oidRelated, collection->childrenClassInfo()->collection( relation ), oid, modified, false );
		} else {
			// It is not browseable from the other object
			return;
		}
	}
}

OidType Manager::relation( const OidType& oid, const QString& relation )
{
	if ( m_relations.contains( oid ) ) {
		return m_relations[ oid ][ relation ].first;
	} else {
		return 0;
	}
}

OidType Manager::relation( const Object* object, const QString& relationName )
{
	assert( object );
	OidType oid = object->oid();
	if ( m_relations.contains( oid ) ) {
		QString relation = ClassInfo::relationName( relationName, object->classInfo()->name() );
		return m_relations[ oid ][ relation ].first;
	} else {
		return 0;
	}
}

Collection* Manager::collection( const Object* object, const QString& relation )
{
	assert( object );
	if ( ! m_collections[ object->oid() ].contains( relation ) ) {
		if ( ! object->classInfo()->containsCollection( relation ) ) {
			kdDebug() << k_funcinfo << "Collection '" << relation << "' doesn't exist in class '" << object->classInfo()->name() << "'" << endl;
			return 0;
		}
		RelatedCollection* col = object->classInfo()->collection( relation );

		m_collections[ object->oid() ][ relation ] = new Collection( col, object->oid(), this );
		load( m_collections[ object->oid() ][ relation ] );
	}
	assert( m_collections[ object->oid() ][ relation ] );
	return m_collections[ object->oid() ][ relation ];
}

Collection* Manager::collection( const OidType& oid, const RelatedCollection* collection )
{
	if ( ! m_collections[ oid ].contains( collection->name() ) ) {
		m_collections[ oid ][ collection->name() ] = new Collection( collection, oid, this );
		load( m_collections[ oid ][ collection->name() ] );
	}
	return m_collections[ oid ][ collection->name() ];
}

void Manager::reset()
{
	ManagerObjectIterator it( m_objects.begin() );
	ManagerObjectIterator end( m_objects.end() );
	for ( ; it != end; ++it )
		delete it.data();
	m_objects.clear();
	ManagerRelatedCollectionIterator cit( m_collections.begin() );
	ManagerRelatedCollectionIterator cend( m_collections.end() );
	for ( ; cit != cend; ++cit ) {
		QMapIterator <QString, Collection*> sit( (*cit).begin() );
		QMapIterator <QString, Collection*> send( (*cit).end() );
		for ( ; sit != send; ++sit )
			delete (*sit);
	}
	m_collections.clear();

	m_relations.clear();

	m_dbBackend->reset();
}

void Manager::copy( Manager* manager )
{
	// There are no pointers in this structure so we can copy it using the default assignment operator
	manager->m_relations = m_relations;
	
	ManagerObjectIterator it( m_objects.begin() );
	ManagerObjectIterator end( m_objects.end() );
	Object *srcObj, *dstObj;
	for ( ; it != end; ++it ) {
		srcObj = (*it);
		dstObj = srcObj->classInfo()->create( srcObj->oid(), manager, true );
		*dstObj = *srcObj;
		dstObj->setModified( true );
	}

	ManagerRelatedCollectionIterator cit( m_collections.begin() );
	ManagerRelatedCollectionIterator cend( m_collections.end() );
	for ( ; cit != cend; ++cit ) {
		QMapIterator <QString, Collection*> sit( (*cit).begin() );
		QMapIterator <QString, Collection*> send( (*cit).end() );
		QMap<QString,Collection*> newMap;
		Collection *col;
		for ( ; sit != send; ++sit ) {
			col = new Collection( (*sit)->collectionInfo() , (*sit)->parentOid(), manager);
			*col = *(*sit);
			newMap.insert( sit.key(), col );
		}
		manager->m_collections.insert( cit.key(), newMap );
	}
}

QMap<OidType, QMap<QString, QPair<OidType, bool> > >& Manager::relations()
{
	return m_relations;
}

QMap<OidType, QMap<QString, Collection*> >& Manager::collections()
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

void Manager::lockObject( Object *object )
{
	m_lockedObjects.insert( object, true );
}

void Manager::unlockObject( Object *object )
{
	m_lockedObjects.remove( object );
}
