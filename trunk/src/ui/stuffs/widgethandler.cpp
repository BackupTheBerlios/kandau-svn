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
#include "widgethandler.h"

WidgetHandler::WidgetHandler( QObject *object, const char* name ) : QObject( object, name )
{
}

void WidgetHandler::setWidget( QWidget * widget )
{
	m_widget = widget;
}

QWidget * WidgetHandler::widget( ) const
{
	return m_widget;
}

void WidgetHandler::setObject( Object * object )
{
	m_object = object;
}

Object * WidgetHandler::object( ) const
{
	return m_object;
}

Object * WidgetHandler::relation( const QString & path )
{
	if ( m_object.isNull() )
		return 0;
	if ( m_object->containsObject( path ) )
		return m_object->object( path );
	// TODO: Analyze path

	return 0;
}

bool WidgetHandler::existsRelation( const QString & path )
{
	if ( m_object.isNull() )
		return false;
	if ( m_object->containsObject( path ) )
		return true;
	// TODO: Analyze path

	return false;
}

Collection * WidgetHandler::collection( const QString & path )
{
	if ( m_object.isNull() )
		return 0;
	if ( m_object->containsCollection( path ) )
		return m_object->collection( path );
	// TODO: Analyze path

	return 0;
}

bool WidgetHandler::existsCollection( const QString & path )
{
	if ( m_object.isNull() )
		return false;
	if ( m_object->containsCollection( path ) )
		return true;
	// TODO: Analyze path

	return false;
}

Property WidgetHandler::property( const QString & path )
{
	return m_object->property( path );
	// TODO: Analyze path
}

bool WidgetHandler::existsProperty( const QString & path )
{
	if ( m_object.isNull() )
		return false;
	if ( m_object->containsProperty( path ) )
		return true;
	// TODO: Analyze path

	return false;
}

#include "widgethandler.moc"
