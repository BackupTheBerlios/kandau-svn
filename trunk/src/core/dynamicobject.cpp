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
#include "dynamicobject.h"

DynamicObject* DynamicObject::create( Manager* manager )
{
	DynamicObject *o = new DynamicObject();
	assert( o );
	o->m_modified = true;
	o->setManager( manager );
	o->m_manager->add( o );
	return o;
}

DynamicObject* DynamicObject::create( OidType oid, Manager* manager )
{
	if ( manager )
		return static_cast<DynamicObject*>( manager->load( oid, &createInstance ) );
	else
		return static_cast<DynamicObject*>( Manager::self()->load( oid, &createInstance ) );
}

DynamicObject* DynamicObject::createObjectInstance() const
{
	DynamicObject *obj = new DynamicObject();
	obj->m_classInfo = m_classInfo;
	return obj;
}

Object* DynamicObject::createInstance()
{
	return new DynamicObject();
}

bool DynamicObject::setProperty( const char* name, const QVariant& value )
{
	assert( classInfo()->containsProperty( name ) );
	m_properties[ QString( name ) ] = value;
	return true;
}

QVariant DynamicObject::propertyValue( const char* name ) const
{
	// If we don't check if the property exists and let QMap to return a default QVariant object, then the QMap will create an entry for that property 
	
	if ( m_properties.contains( QString( name ) ) ) 
		return m_properties[ QString( name ) ];
	else
		// QVariant default constructor creates an Invalid QVariant
		return QVariant(); 
}

#include "dynamicobject.moc"
