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

#include "testbackend.h"
#include "manager.h"
#include "object.h"

using namespace Kandau;

TestBackend::TestBackend()
 : DbBackendIface()
{
	m_lastOid = 0;
}

TestBackend::~TestBackend()
{
}

void TestBackend::setup( Manager *manager )
{
	m_manager = manager;
	m_manager->setMaxObjects( Manager::Unlimited );
}

void TestBackend::shutdown()
{
}

bool TestBackend::load( const OidType& /*oid*/, Object* /*object*/ )
{
	return true;
}

bool TestBackend::load( Collection* /*collection*/ )
{
	return true;
}

bool TestBackend::load( Collection* /*collection*/, const QString& /*query*/ )
{
	return true;
}

bool TestBackend::load( OidType* relatedOid, const OidType& /*oid*/, const RelationInfo* /*related*/ )
{
	relatedOid = 0;
	return true;
}

bool TestBackend::createSchema()
{
	return true;
}

bool TestBackend::hasChanged( Object* /*object*/ )
{
	return false;
}

bool TestBackend::hasChanged( Collection */*collection*/ )
{
	return false;
}

bool TestBackend::hasChanged( const OidType& /*oid*/, const RelationInfo* /*related*/ )
{
	return false;
}

OidType TestBackend::newOid()
{
	return ++m_lastOid;
}

bool TestBackend::commit()
{
	ManagerObjectIterator it( m_manager->objects().begin() );
	ManagerObjectIterator end( m_manager->objects().end() );
	for ( ; it != end; ++it )
		it.data().object()->setModified( false );
	return true;
}

void TestBackend::reset()
{
	m_lastOid = 0;
}
