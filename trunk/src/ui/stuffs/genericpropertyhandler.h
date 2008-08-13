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
#ifndef GENERICPROPERTYHANDLER_H
#define GENERICPROPERTYHANDLER_H

#include <widgethandler.h>

/**
	@author Albert Cervera Areny <albertca@hotpop.com>
*/
class GenericPropertyHandler : public WidgetHandler
{
	Q_OBJECT
public:
	GenericPropertyHandler(QObject *parent = 0, const char *name = 0);
	
	void setWidgetProperty( const QString& p );
	const QString& widgetProperty() const;
	
	void load();
	void save();
private:
	QString m_widget;
	QString m_widgetProperty;
};

class GenericPropertyHandlerFactory : public WidgetHandlerFactory
{
public:
	GenericPropertyHandlerFactory( const QString& property );
	GenericPropertyHandler *create( QWidget* widget ) const;
private:
	QString m_property;
};

#endif
