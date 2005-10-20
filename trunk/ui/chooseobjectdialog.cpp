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
#include <qwidget.h>
#include <qlayout.h>

#include <klistviewsearchline.h>

#include <collection.h>

#include "collectionlistview.h"
#include "chooseobjectdialog.h"

ChooseObjectDialog::ChooseObjectDialog( Collection *collection, Object *currentObject, QWidget *parent ) :
	KDialogBase(parent)
{
	m_currentObject = currentObject;
	m_collection = collection;
	QWidget *dummy = new QWidget( this );
	setMainWidget( dummy );
	QVBoxLayout *vlayout = new QVBoxLayout( dummy );
	
	KListViewSearchLine *m_searchLine = new KListViewSearchLine( dummy );
	if ( collection )
		m_listView = new CollectionListView( collection->childrenClassInfo(), dummy );
	else
		m_listView = new CollectionListView( 0, dummy );
	m_searchLine->setListView( m_listView );
	vlayout->addWidget( m_searchLine );
	vlayout->addWidget( m_listView );
	m_listView->fill();
	if ( currentObject ) {
		QListViewItem *item = m_listView->findItem( oidToString( currentObject->oid() ), 0 );
		if ( item )
			m_listView->setSelected( item, true );
	}
}

Object* ChooseObjectDialog::selectedObject() const
{
	QListViewItem *selected = m_listView->selectedItem();
	if ( ! selected )
		return 0;
		
	OidType oid = stringToOid( selected->text( 0 ) );
	return Classes::classInfo( m_collection->childrenClassInfo()->name() )->create( oid );
}

void ChooseObjectDialog::setCollection( Collection *collection )
{
	m_collection = collection;
	if ( collection )
		m_listView->setClassInfo( collection->collectionInfo()->childrenClassInfo() );
	else
		m_listView->setClassInfo(0);
	m_listView->fill();
}

Collection* ChooseObjectDialog::collection() const
{
	return m_collection;
}


#include "chooseobjectdialog.moc"
