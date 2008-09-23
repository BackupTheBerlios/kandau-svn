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

#include "comboboxhandler.h"
#include "defaultpropertymetainfo.h"


ComboBoxHandler::ComboBoxHandler(QObject *parent, const char *name) : WidgetHandler(parent, name)
{
}

void ComboBoxHandler::load( )
{
	Object *rel = relation( QWhatsThis::textFor( widget() ) );
	combo()->clear();
	if ( ! rel )
		return;

	DefaultPropertyMetaInfo *defaultProperty = dynamic_cast<DefaultPropertyMetaInfo*>( rel->classInfo()->metaInfo( "defaultProperty" ) );

	Collection col( rel->classInfo()->name() );
	CollectionIterator it( col.begin() );
	CollectionIterator end( col.end() );
	Object *obj;
	int item;
	combo()->insertItem( "null" );
	m_oids[ 0 ] = 0;
	for ( ; it != end; ++it ) {
		obj = *it;
		if ( defaultProperty )
			combo()->insertItem( defaultProperty->defaultPropertyValue( obj ) );
		else
			combo()->insertItem( oidToString( obj->oid() ) );

		if ( obj->oid() == rel->oid() )
			item = combo()->count() - 1;
		m_oids[ combo()->count() - 1 ] = obj->oid();
	}
	combo()->setCurrentItem( item );
}

void ComboBoxHandler::save( )
{
	Object *rel = relation( QWhatsThis::textFor( widget() ) );
	if ( ! rel )
		return;

	object()->setObject( QWhatsThis::textFor( widget() ), m_oids[ combo()->currentItem() ] );
}

QComboBox * ComboBoxHandler::combo( ) const
{
	return static_cast<QComboBox*>( widget() );
}

ComboBoxHandler * ComboBoxHandlerFactory::create( QWidget * widget ) const
{
	return new ComboBoxHandler( widget );
}


#include "comboboxhandler.moc"
