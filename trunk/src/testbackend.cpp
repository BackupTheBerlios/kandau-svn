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
	//Manager::self()->setMaxObjects( Manager::Unlimited );
}

void TestBackend::shutdown()
{
}

bool TestBackend::load( const OidType& /*oid*/, Object* /*object*/ )
{
	return true;
}

bool TestBackend::save( Object* object )
{
	kdDebug() << "object (" << oidToString( object->oid() ) << ") saved" << endl;
	return true;
}

bool TestBackend::remove( Object* /*object*/ )
{
	return true;
}

bool TestBackend::load( Collection* /*collection*/ )
{
	return true;
}

bool TestBackend::add( Collection* /*collection*/, Object* /*object*/ )
{
	return true;
}

bool TestBackend::remove( Collection* /*collection*/, const OidType& /*oid*/ )
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

void TestBackend::setRelation( const OidType& /*oid*/, const QString& /*relation*/, const OidType& /*oidRelated*/, const OidType& /*oldOid*/ )
{
}

/*!
Starts a transaction
*/
bool TestBackend::start()
{
	return true;
}

/*!
Commits the current transaction
*/
bool TestBackend::commit()
{
	return true;
}

/*!
Aborts the current transaction
*/
bool TestBackend::rollback()
{
	return true;
}

void TestBackend::reset()
{
	m_lastOid = 0;
}
