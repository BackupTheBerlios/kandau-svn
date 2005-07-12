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
#ifndef INMEMORYSQLDBBACKEND_H
#define INMEMORYSQLDBBACKEND_H

#include <dbbackendiface.h>
#include "oidtype.h"
#include "seqtype.h"
#include "object.h"

class QSqlDatabase;
class QSqlCursor;

/**
@author Albert Cervera Areny
*/
class InMemorySqlDbBackend : public DbBackendIface
{
public:
	InMemorySqlDbBackend( QSqlDatabase* db );
	virtual ~InMemorySqlDbBackend();

	void setup();
	void shutdown();
	bool load( const OidType& oid, Object *object );
	bool load( Collection *collection );
	bool createSchema();
	bool hasChanged( Object * object );
	OidType newOid();
	bool commit();
	void reset() {};

	// Callbacks
	void afterRollback();
	void beforeRemove( Object* /*object*/ ) {};

protected:
	QString sqlType( QVariant::Type type );
	void loadObject( const QSqlCursor& cursor, ObjectRef<Object> object );
	void saveObject( ObjectRef<Object> object );

	QString idFieldName( RelatedCollection *collection ) const;

private:
	QSqlDatabase *m_db;
	OidType m_currentOid;
	QStringList m_savedCollections;
};

#endif
