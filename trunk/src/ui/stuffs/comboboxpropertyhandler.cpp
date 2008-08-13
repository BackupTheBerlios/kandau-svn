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
#include <qcombobox.h>

#include "comboboxpropertyhandler.h"

ComboBoxPropertyHandler::ComboBoxPropertyHandler(QObject *parent, const char *name)
 : WidgetHandler(parent, name)
{
}
void ComboBoxPropertyHandler::load( )
{
	Property p = property( QWhatsThis::textFor( widget() ) );
	combo()->clear();
	if ( ! p.propertyInfo()->isEnumType() )
		return;

	QStringList list( p.propertyInfo()->enumKeys() );
	QStringList::const_iterator it( list.constBegin() );
	QStringList::const_iterator end( list.constEnd() );
	for ( ; it != end; ++it )
		combo()->insertItem( *it );

	combo()->setCurrentText( p.propertyInfo()->valueToKey( p.value().toInt() ) );
}

void ComboBoxPropertyHandler::save( )
{
	Property p = property( QWhatsThis::textFor( widget() ) );
	if ( ! p.propertyInfo()->isEnumType() )
		return;
	p.setValue( p.propertyInfo()->keyToValue( combo()->currentText() ) );
}

QComboBox * ComboBoxPropertyHandler::combo( ) const
{
	return static_cast<QComboBox*>( widget() );
}

ComboBoxPropertyHandler* ComboBoxPropertyHandlerFactory::create( QWidget * widget ) const
{
	return new ComboBoxPropertyHandler( widget );
}


#include "comboboxpropertyhandler.moc"
