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
#ifndef XMLDBBACKEND_H
#define XMLDBBACKEND_H

#include <dbbackendiface.h>

class QDomDocument;
class QDomElement;

/**
@author Albert Cervera Areny
*/
class XmlDbBackend : public DbBackendIface
{
public:
	XmlDbBackend( const QString& fileName, bool truncate = false );
	virtual ~XmlDbBackend();

	void setup( Manager* manager );
	void shutdown();
	void init();

	bool load( const OidType& oid, Object *object );
	
	bool load( Collection *collection );
	bool load( Collection *collection, const QString& query );
	bool load( OidType* relatedOid, const OidType& oid, const RelationInfo* related );

	bool remove( Collection* collection, const OidType& oid );

	bool createSchema();

	bool hasChanged( Object * object );
	bool hasChanged( Collection *collection );
	bool hasChanged( const OidType& oid, const RelationInfo* related );

	OidType newOid();

	bool commit();

	void reset() {};

	void afterRollback();
	void beforeRemove( Object* /*object*/ ) {};

protected:
	OidType elementToObject( const QDomElement& e );
	void objectToElement( Object* object, QDomDocument *doc, QDomElement *parent );

private:
	QString m_fileName;
	OidType m_currentOid;
	Manager *m_manager;
};

#endif
