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
#include <qtable.h>
#include <qwhatsthis.h>
#include <qpopupmenu.h>
#include <klocale.h>

#include "tablehandler.h"
#include "collection.h"


TableHandler::TableHandler( QObject * parent, const char* name ) : WidgetHandler( parent, name )
{
}

void TableHandler::load( )
{
	Collection *col = collection( QWhatsThis::textFor( widget() ) );
	table()->setNumRows( 0 );

	CollectionIterator it( col->begin() );
	CollectionIterator end( col->end() );
	Object *obj;
	QHeader *header = table()->horizontalHeader();
	table()->setNumRows( col->count() );
	table()->setNumCols( table()->numCols() + 1 );
	header->setLabel( table()->numCols(), "oid" );
	for ( uint j = 0; it != end; ++it, ++j ) {
		obj = *it;
		for ( int i = 0; i < header->count() - 1; ++i ) {
			if ( obj->containsProperty( header->label( i ) ) )
				table()->setText( j, i, obj->property( header->label( i ) ).value().toString() );
		}
		table()->setColumnWidth( table()->numCols() - 1, 0 );
		table()->setText( j, table()->numCols() - 1, oidToString( obj->oid() ) );
	}
	connect( table(), SIGNAL(contextMenuRequested(int,int,const QPoint&)), SLOT(tablePopup(int,int,const QPoint&)) );
}

void TableHandler::save()
{
	Collection *col = collection( QWhatsThis::textFor( widget() ) );
	QString oid;
	//Object *obj;
	ObjectRef<Object> obj;
	QHeader *header = table()->horizontalHeader();
	for ( uint j = 0; j < table()->numRows(); ++j ) {
		oid = table()->text( j, table()->numCols() - 1 );
		if ( oid.isNull() || ! col->contains( stringToOid( oid ) ) ) {
			obj = col->childrenClassInfo()->create( col->manager() );
		} else {
			obj = col->object( stringToOid( oid ) );
		}
		for ( int i = 0; i < header->count() - 1; ++i ) {
			kdDebug() << k_funcinfo << "Searching property '" << header->label( i ) << "'..." << endl;
			if ( obj->containsProperty( header->label( i ) ) )
				obj->setProperty( header->label( i ), table()->text( j, i ) );
			else
				kdDebug() << k_funcinfo << "Property '" << header->label( i ) << "' not found." << endl;
		}
		collection( QWhatsThis::textFor( widget() ) )->add( obj );
	}
	// Remove objects
	QValueVector<OidType>::const_iterator it( m_removed.constBegin() );
	QValueVector<OidType>::const_iterator end( m_removed.constEnd() );
	for ( ; it != end; ++it )
		object()->manager()->remove( col->object( *it ) );
}

void TableHandler::tablePopup( int row, int col, const QPoint & pos )
{
	QPopupMenu *menu = new QPopupMenu( widget() );
	menu->insertItem( i18n( "&Add row" ), this, SLOT(tableAddRow()) );
	menu->insertItem( i18n( "&Remove row" ), this, SLOT(tableRemoveRow()) );
	menu->popup( pos );
}

void TableHandler::tableAddRow( )
{
	table()->setNumRows( table()->numRows() + 1 );
}

void TableHandler::tableRemoveRow( )
{
	QString oid;
	oid = table()->text( table()->currentRow(), table()->numCols() - 1 );
	table()->removeRow( table()->currentRow() );
	if ( ! oid.isNull() )
		m_removed.append( stringToOid( oid ) );
}

QTable* TableHandler::table() const
{
	return static_cast<QTable*>( widget() );
}

TableHandler * TableHandlerFactory::create( QWidget * widget ) const
{
	return new TableHandler( widget );
}

#include "tablehandler.moc"
