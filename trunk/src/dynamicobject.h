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
#ifndef DYNAMICOBJECT_H
#define DYNAMICOBJECT_H

#include <object.h>

/**
@author Albert Cervera Areny
*/
class DynamicObject : public Object
{
	Q_OBJECT
public:
	static DynamicObject* create( Manager* manager = 0 );
	static DynamicObject* create( OidType oid, Manager* manager = 0 );
	static Object* createInstance();
	DynamicObject* createObjectInstance() const;

	bool setProperty( const char* name, const QVariant& value );
	QVariant propertyValue( const char* name ) const;

private:
	QMap<QString,QVariant> m_properties;
};

#endif
