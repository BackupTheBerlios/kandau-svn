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
#ifndef DESKTOPCONFIGDBBACKEND_H
#define DESKTOPCONFIGDBBACKEND_H

#include <dbbackendiface.h>

#include <oidtype.h>

class KSimpleConfig;
class Manager;
class Object;
class Collection;
class KConfig;

/**
	@author Albert Cervera Areny <albertca@hotpop.com>
*/
class DesktopConfigDbBackend : public DbBackendIface
{
public:
	DesktopConfigDbBackend( const QString& fileName );
	virtual ~DesktopConfigDbBackend();
	
	void setup( Manager* manager );

	void shutdown();
	bool load( const OidType& oid, Object *object );

	bool load( Collection *collection );

	bool load( Collection *collection, const QString& query );


	bool createSchema();

	bool hasChanged( Object * object );

	bool commit();

	OidType newOid();

	void reset();

	/* Callbacks */

	void beforeRemove( Object *object );

	void afterRollback();
	
	//
	void init();
	void objectToElement( const Object *object, KConfig *config );
private:
	Manager *m_manager;
	QString m_fileName;
	OidType m_currentOid;
};

#endif
