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
#ifndef WIDGETHANDLER_H
#define WIDGETHANDLER_H

#include <qobject.h>

#include <object.h>

/**
@author Albert Cervera Areny
*/
class WidgetHandler : public QObject
{
	Q_OBJECT
public:
	WidgetHandler( QObject *object = 0, const char* name = 0 );
	void setObject( Object* object );
	Object* object() const;
	void setWidget( QWidget *widget );
	QWidget* widget() const;
	virtual void load() = 0;
	virtual void save() = 0;

	Object* relation( const QString& path );
	bool existsRelation( const QString& path );

	Collection* collection( const QString& path );
	bool existsCollection( const QString& path );

	Property property( const QString& path );
	bool existsProperty( const QString& path );

private:
	ObjectRef<Object> m_object;
	QWidget *m_widget;
};

class WidgetHandlerFactory
{
public:
	virtual WidgetHandler* create( QWidget *widget ) const = 0;
};

#endif
