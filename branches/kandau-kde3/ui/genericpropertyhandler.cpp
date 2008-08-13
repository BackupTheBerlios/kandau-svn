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
#include <qwhatsthis.h>
#include <qwidget.h>

#include "genericpropertyhandler.h"

using namespace Kandau;
using namespace Kandau::Ui;

GenericPropertyHandler::GenericPropertyHandler(QObject *parent, const char *name)
 : WidgetHandler(parent, name)
{
}

void GenericPropertyHandler::load()
{
	if ( ! existsProperty( QWhatsThis::textFor( widget() ) ) )
		return;
	Property p = property( QWhatsThis::textFor( widget() ) );
	widget()->setProperty( m_widgetProperty, p.value() );
}

void GenericPropertyHandler::save()
{
	if ( ! existsProperty( QWhatsThis::textFor( widget() ) ) )
		return;
	Property p = property( QWhatsThis::textFor( widget() ) );
	p.setValue( widget()->property( m_widgetProperty ) );
}

void GenericPropertyHandler::setWidgetProperty( const QString & p )
{
	m_widgetProperty = p;
}

const QString & GenericPropertyHandler::widgetProperty( ) const
{
	return m_widgetProperty;
}

GenericPropertyHandlerFactory::GenericPropertyHandlerFactory( const QString & property )
{
	m_property = property;
}

GenericPropertyHandler * GenericPropertyHandlerFactory::create( QWidget * widget ) const
{
	GenericPropertyHandler *h = new GenericPropertyHandler( widget );
	h->setWidgetProperty( m_property );
	return h;
}

#include "genericpropertyhandler.moc"
