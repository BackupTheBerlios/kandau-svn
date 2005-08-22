/***************************************************************************
 *   Copyright (C) 2004 by Albert Cervera Areny                            *
 *   albertca.com                                                   *
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

#include <dcopclient.h>
#include <kdebug.h>

#include <classes.h>
#include <object.h>

#include "objectaccess.h"

ObjectAccess::ObjectAccess( KApplication *app ) : KDialog(), DCOPObject( "ObjectAccess" )
{
	if ( !app->dcopClient()->isRegistered() ) {
		app->dcopClient()->registerAs( "ObjectAccess" );
		app->dcopClient()->setDefaultObject( objId() );
	}
}

QStringList ObjectAccess::classes() const
{
  QStringList r;
  
	ClassInfoIterator it( Classes::begin() );
	ClassInfoIterator end( Classes::end() );
	for ( ; it != end; ++it )
		r.append( (*it)->name() );
	return r;
}

QStringList ObjectAccess::properties( const QString& className ) const
{
	if ( ! Classes::contains( className ) )
		return QString::null;
	
	Object *obj = Classes::classInfo( className )->createInstance();
	assert( obj );

	PropertiesConstIterator it( obj->propertiesConstBegin() );
	PropertiesConstIterator end( obj->propertiesConstEnd() );
	QStringList list;
	for ( ; it != end; ++it ) {
		list << it.data().name();
	}
	delete obj;
	return list;
}

QStringList ObjectAccess::relatedObjects( const QString& className ) const
{
	QStringList r;
	
	if ( ! Classes::contains( className ) )
		return r;
	
	ClassInfo *c = Classes::classInfo( className );
	assert( c );
	RelatedObjectsIterator it( c->objectsBegin() );
	RelatedObjectsIterator end( c->objectsEnd() );
	kdDebug() << "1" << endl;
	for ( ; it != end; ++it ) {
		kdDebug() << "2" << endl;
		r.append( it.key() );
	}
	return r;
}

QStringList ObjectAccess::relatedCollections( const QString& className ) const
{
	QStringList r;
	
	if ( ! Classes::contains( className ) )
		return r;
	ClassInfo *c = Classes::classInfo( className );
	RelatedCollectionsIterator it( c->collectionsBegin() );
	RelatedCollectionsIterator end( c->collectionsEnd() );
	kdDebug() << "1" << endl;
	for ( ; it != end; ++it ) {
		kdDebug() << "2" << endl;
		r.append( it.key() );
	}
	return r;
}

QString ObjectAccess::create( const QString& className ) const
{
	if ( ! Classes::contains( className ) )
		return 0;
	Object *obj = Classes::classInfo( className )->create();
	return oidToString( obj->oid() );
}

QStringList ObjectAccess::objects() const
{
	QStringList r;
	ManagerObjectIterator it( Manager::self()->begin() );
	ManagerObjectIterator end( Manager::self()->end() );
	for ( ; it != end; ++it ) {
		r.append( oidToString( (*it)->oid() ) );
	}
	return r;
}

bool ObjectAccess::commit()
{
	return Manager::self()->commit();
}

bool ObjectAccess::rollback()
{
	return Manager::self()->rollback();
}

void ObjectAccess::reset()
{
	Manager::self()->reset();
}

QString ObjectAccess::property( const QString& className, const QString& oid, const QString& property )
{
	if ( ! Classes::contains( className ) )
		return QString::null;
	Object *obj = Manager::self()->load( stringToOid( oid ), Classes::classInfo( className ) );
	if ( obj == 0 || ! obj->containsProperty( property ) )
		return QString::null;
	return obj->property( property ).value().toString();
}

void ObjectAccess::setProperty( const QString& className, const QString& oid, const QString& property, const QString& value )
{
	if ( ! Classes::contains( className ) )
		return;
	Object *obj = Manager::self()->load( stringToOid( oid ), Classes::classInfo( className ) );
	if ( obj == 0 || ! obj->containsProperty( property ) )
		return;
	obj->property( property ).setValue( value );
}

QString ObjectAccess::object( const QString& className, const QString& oid, const QString& object )
{
	if ( ! Classes::contains( className ) )
		return QString::null;
	Object *obj = Manager::self()->load( stringToOid( oid ), Classes::classInfo( className ) );
	if ( obj == 0 || ! obj->containsObject( object ) )
		return QString::null;
	if ( obj->object( object ) == 0 )
		return QString::null;
	return oidToString( obj->object( object )->oid() );
}

void ObjectAccess::setObject( const QString& className, const QString& oid, const QString& object, const QString& value )
{
	if ( ! Classes::contains( className ) )
		return;
	Object *obj = Manager::self()->load( stringToOid( oid ), Classes::classInfo( className ) );
	if ( obj == 0 || ! obj->containsObject( object ) )
		return;
	return obj->setObject( object, stringToOid( value ) );
}

QStringList ObjectAccess::collection( const QString& className, const QString& oid, const QString& collection )
{
	if ( ! Classes::contains( className ) )
		return QString::null;
	Object *obj = Manager::self()->load( stringToOid( oid ), Classes::classInfo( className ) );
	if ( obj == 0 || ! obj->containsCollection( collection ) )
		return QString::null;
	Collection *col = obj->collection( collection );
	CollectionIterator it( col->begin() );
	CollectionIterator end( col->end() );
	QStringList list;
	for ( ; it != end; ++it )
		list << oidToString( (*it)->oid() );
	return list;
}

void ObjectAccess::add( const QString& className, const QString& oid, const QString& collection, const QString& value )
{
	if ( ! Classes::contains( className ) )
		return;
	Object *obj = Manager::self()->load( stringToOid( oid ), Classes::classInfo( className ) );
	if ( obj == 0 || ! obj->containsCollection( collection ) )
		return;

	Collection *col = obj->collection( collection );
	Object *ad = Manager::self()->load( stringToOid( value ), col->collectionInfo()->childrenClassInfo() );
	if ( ad == 0 )
		return;

	col->add( ad );
}

void ObjectAccess::remove( const QString& className, const QString& oid, const QString& collection, const QString& value )
{
	if ( ! Classes::contains( className ) )
		return;
	Object *obj = Manager::self()->load( stringToOid( oid ), Classes::classInfo( className ) );
	if ( obj == 0 || ! obj->containsCollection( collection ) )
		return;
	
	Collection *col = obj->collection( collection );
	col->remove( stringToOid( value ) );
}
