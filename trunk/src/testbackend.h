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
#ifndef TESTBACKEND_H
#define TESTBACKEND_H

#include "dbbackendiface.h"

/**
@author Albert Cervera Areny
*/
class TestBackend : public DbBackendIface
{
public:
	TestBackend();
	virtual ~TestBackend();

	void setup();
	void shutdown();

	/* Object management related functions */
	bool load( const OidType& oid, Object *object );
	bool save( Object *object );
	bool remove( Object *object );


	/* Collection management related functions */
	bool load( Collection *collection );
	bool add( Collection* collection, Object* object );
	bool remove( Collection* collection, const OidType& oid );

	/* Database schema related functions */
	bool createSchema();

	void setRelation( const OidType& oid, const QString& relation, const OidType& oidRelated, const OidType& oldOid );

	/* Decides whether the object changed in the database since last load */
	bool hasChanged( Object * object );

	/*!
	Starts a transaction
	 */
	bool start();

	/*!
	Commits the current transaction
	 */
	bool commit();

	/*!
	Aborts the current transaction
	 */
	bool rollback();


	OidType newOid();

	void reset();
private:
	OidType m_lastOid;
};

#endif
