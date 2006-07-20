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
#include <qdragobject.h>

#include <classes.h>
#include <collection.h>

#include "collectionlistview.h"

CollectionListView::CollectionListView( const ClassInfo *classInfo, QWidget *parent ) :
	KListView(parent)
{
	setAllColumnsShowFocus( true );
	setAlternateBackground( QColor(40, 40, 40) );
	setClassInfo( classInfo );
	m_manager = 0;
}

void CollectionListView::fill()
{
	clear();
	while (  columns() > 0 )
		removeColumn( 0 );

	if ( ! m_classInfo )
		return;

	PropertiesInfoConstIterator it( m_classInfo->propertiesBegin() );
	PropertiesInfoConstIterator end( m_classInfo->propertiesEnd() );
	addColumn( "oid" );
	for ( ; it != end; ++it ) {
		addColumn( it.data()->name() );
	}
	Collection col( m_classInfo->name() );
	CollectionIterator it2( col.begin() );
	CollectionIterator end2( col.end() );
	for (; it2 != end2; ++it2 ) {
		add( it2.data() );
	}
}

void CollectionListView::add( const Object* object )
{
	QString val;
	QListViewItem *item = new QListViewItem( this );
	item->setDropEnabled( true );
	item->setDragEnabled( true );
	item->setText( 0, oidToString( object->oid() ) );
	for ( int i = 1; i < columns(); ++i ) {
		const PropertyInfo *info = object->classInfo()->property( columnText( i ) );
		if ( info->isEnumType() ) {
			if ( info->isSetType() ) {
				val = info->valueToKeys( object->property( columnText( i ) ).value().toInt() ).join( ", " );
			} else {
				val = info->valueToKey( object->property( columnText( i ) ).value().toInt() );
			}
		} else {
			val = object->property( columnText( i ) ).value().toString();
		}
		item->setText( i, val );
	}
}

void CollectionListView::remove( const OidType& oid )
{
	delete findItem( oidToString( oid ), 0 );
}

void CollectionListView::setClassInfo( const ClassInfo *classInfo )
{
	m_classInfo = classInfo;

	while (  columns() > 0 )
		removeColumn( 0 );

	if ( m_classInfo ) {
		PropertiesInfoConstIterator it( m_classInfo->propertiesBegin() );
		PropertiesInfoConstIterator end( m_classInfo->propertiesEnd() );
		addColumn( "oid" );
		for ( ; it != end; ++it ) {
			addColumn( it.data()->name() );
		}
	}
}

const ClassInfo* CollectionListView::classInfo() const
{
	return m_classInfo;
}

/*!
Obtains a pointer to the Object refered by the currently selected item.
*/
Object * CollectionListView::currentObject() const
{
	if ( ! currentItem() )
		return 0;

	if ( m_manager )
		return m_classInfo->create( stringToOid( currentItem()->text( 0 ) ), m_manager );
	else
		return m_classInfo->create( stringToOid( currentItem()->text( 0 ) ) );
}

/*!
Obtains the Oid of the currently selected item.
*/
OidType CollectionListView::currentOid() const
{
	if ( currentItem() )
		return stringToOid( currentItem()->text( 0 ) );
	else
		return 0;
}

QDragObject * CollectionListView::dragObject()
{
	if ( ! currentItem() )
		return 0;

	QTextDrag *text = new QTextDrag();
	text->setText( oidToString( currentOid() ) );
	return text;
}

#include "collectionlistview.moc"
