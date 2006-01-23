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
#include <ksimpleconfig.h>

#include <manager.h>
#include <object.h>

#include "desktopconfigdbbackend.h"

DesktopConfigDbBackend::DesktopConfigDbBackend( const QString& fileName )
{
	m_fileName = fileName;
}

DesktopConfigDbBackend::~DesktopConfigDbBackend()
{
}

void DesktopConfigDbBackend::setup( Manager* manager )
{
	m_manager = manager;
	m_manager->setMaxObjects( Manager::Unlimited );
	init();
}

void DesktopConfigDbBackend::init()
{
	m_manager->reset();
	m_currentOid = 1;

	KSimpleConfig config( m_fileName );
	
	QStringList groups = config.groupList();
	QStringList::ConstIterator it( groups.begin() );
	QStringList::ConstIterator end( groups.end() );
	QMap<QString,QString> map;
	for ( ; it != end; ++it ) {
		map = config.entryMap( *it );
		ObjectRef<Object> obj;
		if ( map.contains( "oid" ) ) 
			obj = Classes::classInfo( *it )->create( stringToOid( map["oid"] ), m_manager, true );
		else
			obj = Classes::classInfo( *it )->create( m_manager );
		assert( obj );
		if ( m_currentOid < obj->oid() )
			m_currentOid = obj->oid();
		QMapConstIterator<QString,QString> mit( map.begin() );
		QMapConstIterator<QString,QString> mend( map.end() );
		for ( ; mit != mend; ++mit ) {
			if ( mit.key() != "oid" ) 
				obj->setProperty( mit.key(), mit.data() );
		}
	}
}

void DesktopConfigDbBackend::shutdown()
{
	commit();
}

bool DesktopConfigDbBackend::load( const OidType& /*oid*/, Object */*object*/ )
{
	return true;
}

bool DesktopConfigDbBackend::load( Collection */*collection*/ )
{
	return true;
}

bool DesktopConfigDbBackend::load( Collection */*collection*/, const QString& /*query*/ )
{
	return true;
}

bool DesktopConfigDbBackend::load( OidType* /*relatedOid*/, const OidType& /*oid*/, const RelationInfo* /*related*/ )
{
	return true;
}

bool DesktopConfigDbBackend::createSchema()
{
	return true;
}

bool DesktopConfigDbBackend::hasChanged( Object */*object*/ )
{
	return false;
}

bool DesktopConfigDbBackend::hasChanged( Collection */*collection*/ )
{
	return false;
}

bool DesktopConfigDbBackend::hasChanged( const OidType& /*oid*/, const RelationInfo* /*related*/ )
{
	return false;
}

void DesktopConfigDbBackend::objectToElement( const Object *object, KConfig *config )
{
	config->setGroup( object->classInfo()->name() );
	
	config->writeEntry( "oid", object->oid() );
	PropertiesConstIterator it( object->propertiesConstBegin() );
	PropertiesConstIterator end( object->propertiesConstEnd() );
	Property p;
	for ( ; it != end; ++it ) {
		p = *it;
		if ( ! p.readOnly() )
			config->writeEntry( p.name(), p.value() );
	}
}

bool DesktopConfigDbBackend::commit()
{
	KSimpleConfig config( m_fileName );

	ManagerObjectIterator it( m_manager->objects().begin() );
	ManagerObjectIterator end( m_manager->objects().end() );
	for ( ; it != end; ++it )
		objectToElement( it.data().object(), &config );

	config.sync();
	return true;
}

OidType DesktopConfigDbBackend::newOid()
{
	return m_currentOid++;
}

void DesktopConfigDbBackend::reset()
{
}

/* Callbacks */

void DesktopConfigDbBackend::beforeRemove( Object */*object*/ )
{
}

void DesktopConfigDbBackend::afterRollback()
{
	init();
}
