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
#include <classes.h>
#include <collection.h>
#include <labelsmetainfo.h>
#include "collectionlistview.h"

CollectionListView::CollectionListView( const ClassInfo *classInfo, QWidget *parent ) :
	KListView(parent),
	m_classInfo(classInfo)
{
	setAllColumnsShowFocus( true );
	setAlternateBackground( QColor(40, 40, 40) );
}

void CollectionListView::fill()
{
	clear();

	while ( columns() > 0 )
		removeColumn( 0 );

	if ( ! m_classInfo )
		return;

	PropertiesInfoConstIterator it( m_classInfo->propertiesBegin() );
	PropertiesInfoConstIterator end( m_classInfo->propertiesEnd() );

	LabelsMetaInfo *labels = dynamic_cast<LabelsMetaInfo*>( m_classInfo->metaInfo( "labels" ) );
	addColumn( "oid" );
	QString name;
	for ( ; it != end; ++it ) {
		if ( labels )
			name = labels->label( it.data()->name() );
		else
			name = it.data()->name();
		addColumn( name );
		m_map.insert( name, it.data()->name() );
		m_map2.insert( it.data()->name(), name );
	}

	Collection col( m_classInfo->name() );
	CollectionIterator it2( col.begin() );
	CollectionIterator end2( col.end() );
	for ( ; it2 != end2; ++it2 ) {
		QListViewItem *item = new QListViewItem( this );
		item->setText( 0, oidToString( it2.data()->oid() ) );
		for ( int i = 1; i < columns(); ++i ) {
			item->setText( i, it2.data()->property( propertyName( i ) ).value().toString() );
		}
	}
}

void CollectionListView::setClassInfo( const ClassInfo *classInfo )
{
	m_classInfo = classInfo;
}

const ClassInfo* CollectionListView::classInfo() const
{
	return m_classInfo;
}

int CollectionListView::column( const QString& property )
{
	if ( m_map2.contains( property ) ) {
		QString mapEntry = m_map2[ property ];
		for ( int i = 0; i < columns(); i++ ) {
			if ( mapEntry == columnText( i ) )
				return i;
		}
	}
	return -1;
}

QString CollectionListView::propertyName( int column )
{
	return propertyName( columnText( column ) );
}

QString CollectionListView::propertyName( const QString& columnName )
{
	if ( m_map.contains( columnName ) )
		return m_map[ columnName ];
	else
		return QString::null;
}

#include "collectionlistview.moc"
