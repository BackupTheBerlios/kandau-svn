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
#include <assert.h>

#include <qsqldatabase.h>

#include <kdebug.h>

#include "object.h"
#include "manager.h"
#include "dbbackendiface.h"
#include "testbackend.h"

Manager* Manager::m_self = 0;


Manager::Manager( DbBackendIface *backend )
{
	//m_dbBackend = new TestBackend();
	assert( backend );
	assert( m_self == 0 );
	m_dbBackend = backend;
	m_self = this;
//	m_maxObjects = MaxObjects;
	m_maxObjects = Unlimited;

	// Initialize the backend (maybe it needs to initialize some
	// values of the manager such as maxObjects or fill the map of objects in a
	// memory only backend).
	m_dbBackend->setup();
}

Manager::~Manager()
{
	m_dbBackend->shutdown();
	QMapIterator<OidType,Object*> it( m_objects.begin() );
	QMapIterator<OidType,Object*> end( m_objects.end() );
	for ( ; it != end; ++it )
		delete it.data();

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

bool Manager::add( Object* object )
{
	assert( object );

	if ( object->isNull() )
		object->setOid( m_dbBackend->newOid() );
	kdDebug() << k_funcinfo << ": Oid=" << object->oid() << endl;
	// Call before m_objects.insert as it might free the just added object.
	ensureUnderMaxObjects();
	kdDebug() << k_funcinfo << ": Oid=" << object->oid() << endl;
	m_objects.insert( object->oid(), object );
	return true;
}

bool Manager::remove( Object* object )
{
	assert( object );
	m_objects.remove( object->oid() );
	return true;
}

bool Manager::contains( OidType oid )
{
	return m_objects.contains( oid );
}

Object* Manager::object( OidType oid )
{
	return m_objects[oid];
}

/*
	Functions related to collection management
*/

//bool Manager::add( Collection* collection, Object* object )
//{
//	return m_dbBackend->add( collection, object );
//}

//bool Manager::remove( Collection* collection, const OidType& oid )
//{
//	return m_dbBackend->remove( collection, oid );
//}

bool Manager::load( Collection* collection )
{
	return m_dbBackend->load( collection );
}

/*
Transaction management functions
*/
/*
bool Manager::start()
{
	return m_dbBackend->start();
}
*/

bool Manager::commit()
{
	// Save all modified objects
	/*
	QMapIterator<OidType, Object*> oit( m_objects.begin() );
	QMapIterator<OidType, Object*> oend( m_objects.end() );
	QValueList<OidType> olist;
	for ( ; oit != oend; ++oit ) {
		if ( ! (*oit)->isModified() )
			continue;
		if ( m_dbBackend->save( *oit ) ) {
			(*oit)->setModified( false );
			kdDebug() << "Object " << oidToString( (*oit)->oid() ) << " set as unmodified " << endl;
		}
	}
	*/
	/*
	QValueListIterator<OidType> lit( olist.begin() );
	QValueListIterator<OidType> lend( olist.end() );
	for ( ; lit != lend; ++lit ) {
		delete m_objects[ *lit ];
		m_objects.remove( *lit );
	}
	olist.clear();

	// Delete all modified relations
	QMapIterator<OidType, QMap<QString, QPair<OidType, bool> > > rit ( m_relations.begin() );
	QMapIterator<OidType, QMap<QString, QPair<OidType, bool> > > rend ( m_relations.end() );
	for ( ; rit != rend; ++rit ) {
		removeObjectReferences( rit, Modified );
		if ( (*rit).count() == 0 )
			olist.append( rit.key() );
	}
	lit = olist.begin();
	lend = olist.end();
	for ( ; lit != lend; ++lit ) {
		if ( m_objects.contains( *lit ) ) {
			delete m_objects[ *lit ];
			m_objects.remove( *lit );
		}
		m_relations.remove( *lit );
	}
	*/
	return m_dbBackend->commit();
}

bool Manager::rollback()
{
	m_dbBackend->rollback();
	// Delete all modified objects
	QMapIterator<OidType, Object*> oit( m_objects.begin() );
	QMapIterator<OidType, Object*> oend( m_objects.end() );
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
	QMapIterator<OidType, QMap<QString, QPair<OidType, bool> > > rit ( m_relations.begin() );
	QMapIterator<OidType, QMap<QString, QPair<OidType, bool> > > rend ( m_relations.end() );
	for ( ; rit != rend; ++rit ) {
		removeObjectReferences( rit, Modified );
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
	
	QMapIterator<OidType, QMap<QString, Collection*> > cit( m_collections.begin() );
	QMapIterator<OidType, QMap<QString, Collection*> > cend( m_collections.end() );
	for ( ; cit != cend; ++cit ) {
		removeCollectionReferences( cit, Modified );
		if ( (*cit).count() == 0 )
			olist.append( cit.key() );
	}
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

bool Manager::save( Object* object )
{
	return m_dbBackend->save( object );
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

	Object *object;
	assert( f );
	object = m_objects[oid];
	if ( object ) {
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
	OidType oid = 0;
	if ( object )
		oid = object->oid();
	if ( m_objects.count() >= m_maxObjects ) {
		QMapIterator<OidType,Object*> it( m_objects.begin() );
		QMapIterator<OidType,Object*> end( m_objects.end() );
		QValueList<OidType> list;
		for ( ; it != end; ++it ) {
			if ( ! it.data()->isModified() && it.key() != oid ) {
				delete it.data();
				list.append( it.key() );
				if ( m_objects.count() - list.count() <= m_maxObjects )
					break;
			}
		}
		QValueListIterator<OidType> vIt( list.begin() );
		QValueListIterator<OidType> vEnd( list.end() );
		for ( ; vIt != vEnd; ++vIt ) {
			m_objects.remove( *vIt );
			removeObjectReferences( *vIt, Unmodified );
		}
	}
}

void Manager::removeObjectReferences( QMapIterator<OidType, QMap<QString, QPair<OidType, bool> > > oid, Filter filter )
{
  	bool boolFilter = filter == Modified ? true : false;

	QMapIterator<QString,QPair<OidType,bool> > it( (*oid).begin() );
	QMapIterator<QString,QPair<OidType,bool> > end( (*oid).end() );
	QValueList<QString> list;
	for ( ; it != end; ++it )
		if ( (*it).second == boolFilter )
			list.append( it.key() );
	QValueListIterator<QString> vIt( list.begin() );
	QValueListIterator<QString> vEnd( list.end() );
	for ( ; vIt != vEnd; ++vIt )
		(*oid).remove( *vIt );
}


void Manager::removeObjectReferences( const OidType& oid, Filter filter )
{
	QMapIterator<OidType, QMap<QString, QPair<OidType, bool> > > it = m_relations.find( oid );
	if ( it == m_relations.end() )
			return;
	removeObjectReferences( it, filter );
	if ( (*it).count() == 0 )
		m_relations.remove( oid );
}

void Manager::removeCollectionReferences( QMapIterator<OidType, QMap<QString, Collection*> > oid, Filter filter )
{
  	bool boolFilter = filter == Modified ? true : false;

	QMapIterator<QString, Collection*> it( (*oid).begin() );
	QMapIterator<QString, Collection*> end( (*oid).end() );
	QValueList<QString> list;
	for ( ; it != end; ++it )
		if ( (*it)->modified() == boolFilter )
			list.append( it.key() );
	QValueListIterator<QString> vIt( list.begin() );
	QValueListIterator<QString> vEnd( list.end() );
	for ( ; vIt != vEnd; ++vIt )
		(*oid).remove( *vIt );
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

/*
void Manager::setRelation( const Object* object, const QString& relationName, const OidType& oidRelated, bool  recursive )
{
	QString relation = ClassInfo::relationName( relationName, object->classInfo()->name() );

	OidType oid = object->oid();
	if ( m_relations.contains( oid ) ) {
		// We should first remove previous relation but it's quite hard to do
		// as we should know whether it's a collection or not and so on...
		m_relations[ oid ][ relation ].first = oidRelated;
		m_relations[ oid ][ relation ].second = true; // Modified
	} else {
		QMap<QString,QPair<OidType,bool> > map;
		map.insert( relation, QPair<OidType,bool>(oidRelated,true) );
		m_relations[ oid ] = map;
	}
}


void Manager::setRelation( const Object* object, const QString& relationName, const Object* objRelated, bool recursive )
{
	kdDebug() << "Hello from setRelation" << endl;
	QString relation = ClassInfo::relationName( relationName, object->classInfo()->name() );
	kdDebug() << "SETRELATION: " << relation << endl;

	OidType oid = object->oid();
	OidType oidRelated = objRelated ? objRelated->oid() : 0;

	bool isCollection = false;
	QString relatedClassName;

	kdDebug() << "Searching relation: " << relation << endl;
	if ( object->classInfo()->containsCollection( relation ) ) {
		isCollection = true;
		RelatedCollection* col = object->classInfo()->collection( relation );
		relatedClassName = col->childrenClassInfo()->name();
	} else if ( object->classInfo()->containsObject( relation ) ) {
		isCollection = false;
		RelatedObject* obj = object->classInfo()->object( relation );
		relatedClassName = obj->relatedClassInfo()->name();
	} else {
		if ( ! recursive ) {
			// It is not browseable from this object so we return
			return;
		} else {
			// There is an error !!!
			assert( false );
		}
	}

	if ( m_relations.contains( oid ) ) {
		OidType old = m_relations[ oid ][ relation ].first;
		// Avoid removing the previously defined relation (we're probably inside the recursion)
		if ( old != oidRelated ) {
			if ( ! isCollection ) {
				if ( m_relations.contains( old ) ) {
					m_relations[ old ].remove( relation );
				}
			} else {
				ObjectRef<Object> oldObject( old );
				removeRelation( oldObject, relation, object, false );
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
			setRelation( objRelated, relation, object, false );
		} else {
			addRelation( objRelated, relation, object, false );
		}
	}
	kdDebug() << "Bye, bye from setRelation" << endl;
}

void Manager::addRelation( const Object *object, const QString& relationName, const Object* objRelated, bool recursive )
{
	QString relation = ClassInfo::relationName( relationName, object->classInfo()->name() );

	OidType oid = object->oid();
	OidType oidRelated = objRelated->oid();
	assert( object->classInfo()->containsCollection( relation ) );
	RelatedCollection* col = object->classInfo()->collection( relation );
	if ( m_collections[ oid ][ relation ] == 0 )
		m_collections[ oid ][ relation ] = new Collection( col, oid );

	m_collections[ oid ][ relation ]->add( oidRelated );

	if ( recursive ) {
		if ( Classes::classInfo( col->childrenClassInfo()->name() )->containsObject( relation ) ) {
			setRelation( objRelated, relation, object, false );
		} else if ( Classes::classInfo( col->childrenClassInfo()->name() )->containsCollection( relation ) ) {
			addRelation( objRelated, relation, object, false );
		} else {
			// It is not browseable from the other object
			return;
		}
	}
}

void Manager::removeRelation( const Object* object, const QString& relationName, const Object* objRelated, bool recursive )
{
	QString relation = ClassInfo::relationName( relationName, object->classInfo()->name() );

	OidType oid = object->oid();
	OidType oidRelated = objRelated->oid();
	if ( ! m_collections.contains( oid ) )
		return;
	if ( ! m_collections[ oid ].contains( relation ) )
		return;
	m_collections[ oid ][ relation ]->remove( oidRelated );

	if ( m_collections[ oid ][ relation ]->count() == 0 ) {
		delete m_collections[ oid ][ relation ];
		m_collections[ oid ].remove( relation );
		if ( m_collections[ oid ].count() == 0 ) {
			m_collections.remove( oid );
		}
	}
	if ( recursive ) {
		RelatedCollection* col = object->classInfo()->collection( relation );
		if ( Classes::classInfo( col->childrenClassInfo()->name() )->containsObject( relation ) ) {
			setRelation( objRelated, relation, (Object*)0, false );
		} else if ( Classes::classInfo( relation )->containsCollection( relation ) ) {
			removeRelation( objRelated, relation, object, false );
		} else {
			// It is not browseable from the other object
			return;
		}
	}
}
*/

void Manager::setRelation( const OidType& oid, ClassInfo* classInfo, const QString& relationName, const OidType& oidRelated, bool recursive )
{
	assert( classInfo );
	kdDebug() << "Hello from setRelation" << endl;
	QString relation = ClassInfo::relationName( relationName, classInfo->name() );
	kdDebug() << "SETRELATION: " << relation << endl;

	bool isCollection = false;
	QString relatedClassName;

	kdDebug() << "Searching relation: " << relation << endl;
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
	kdDebug() << "Bye, bye from setRelation" << endl;
}

void Manager::addRelation( const OidType& oid, RelatedCollection* collection, const OidType& oidRelated, bool recursive )
{
	QString relation = ClassInfo::relationName( collection->name(), collection->parentClassInfo()->name() );

	if ( m_collections[ oid ][ relation ] == 0 )
		m_collections[ oid ][ relation ] = new Collection( collection, oid );

	m_collections[ oid ][ relation ]->simpleAdd( oidRelated );

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

void Manager::removeRelation( const OidType& oid, RelatedCollection* collection, const OidType& oidRelated, bool recursive )
{
	QString relation = ClassInfo::relationName( collection->name(), collection->parentClassInfo()->name() );

	if ( ! m_collections.contains( oid ) )
		return;
	if ( ! m_collections[ oid ].contains( relation ) )
		return;
	//m_collections[ oid ][ relation ]->remove( oidRelated );
	m_collections[ oid ][ relation ]->simpleRemove( oidRelated );

	if ( m_collections[ oid ][ relation ]->count() == 0 ) {
		delete m_collections[ oid ][ relation ];
		m_collections[ oid ].remove( relation );
		if ( m_collections[ oid ].count() == 0 ) {
			m_collections.remove( oid );
		}
	}
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
	kdDebug() << k_funcinfo << ": relationName = '" << relationName << "'" << endl;
	if ( m_relations.contains( oid ) ) {
		QString relation = ClassInfo::relationName( relationName, object->classInfo()->name() );
		kdDebug() << k_funcinfo << ": relation = '" << relation << "'" << endl;
		return m_relations[ oid ][ relation ].first;
	} else {
		return 0;
	}
}

Collection* Manager::collection( const Object* object, const QString& relation )
{
	assert( object );
	if ( ! m_collections.contains( object->oid() ) ) {
		if ( ! object->classInfo()->containsCollection( relation ) )
			return 0;

		RelatedCollection* col = object->classInfo()->collection( relation );

		m_collections[ object->oid() ][ relation ] = new Collection( col, object->oid() );
	}
	return m_collections[ object->oid() ][ relation ];
}

Collection* Manager::collection( const OidType& oid, RelatedCollection* collection )
{
	if ( ! m_collections.contains( oid ) ) {
		if ( ! m_collections[ oid ].contains( collection->name() ) )
			m_collections[ oid ][ collection->name() ] = new Collection( collection, oid );
	}
	return m_collections[ oid ][ collection->name() ];
}

void Manager::reset()
{
	m_relations.clear();
	m_objects.clear();
	m_dbBackend->reset();
}

QMap<OidType, QMap<QString, QPair<OidType, bool> > >& Manager::relations()
{
	return m_relations;
}

QMap<OidType, QMap<QString, Collection*> >& Manager::collections()
{
	return m_collections;
}
