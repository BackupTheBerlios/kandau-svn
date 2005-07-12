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
#include <kdebug.h>

#include "testbackend.h"
#include "manager.h"
#include "object.h"

TestBackend::TestBackend()
 : DbBackendIface()
{
	m_lastOid = 0;
}

TestBackend::~TestBackend()
{
}

void TestBackend::setup()
{
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

bool TestBackend::createSchema()
{
	return true;
}

/* Decides whether the object changed in the database since last load */
bool TestBackend::hasChanged( Object* /*object*/ )
{
	return false;
}

OidType TestBackend::newOid()
{
	return ++m_lastOid;
}

bool TestBackend::commit()
{
	return true;
}

void TestBackend::reset()
{
	m_lastOid = 0;
}
