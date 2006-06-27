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
#include <collection.h>
#include <labelsmetainfo.h>

#include "classchooser.h"

ClassChooser::ClassChooser( QWidget *parent ) : QToolBox( parent )
{
        ClassInfoConstIterator it( Classes::begin() );
        ClassInfoConstIterator end( Classes::end() );
        for ( ; it != end; ++it ) {
                ClassInfo *info = it.data();

                int i = m_classes.size();
                m_classes.resize( i + 1 );
                m_classes[ i ] = info;

                LabelsMetaInfo *labels = dynamic_cast<LabelsMetaInfo*>( info->metaInfo( "labels" ) );
                if ( labels )
                        addItem( new QWidget( this ), labels->label( info->name() ) );
                else
                        addItem( new QWidget(this), info->name() );
        }
        connect( this, SIGNAL(currentChanged(int)), SLOT(slotCurrentChanged(int)) );
}

const ClassInfo* ClassChooser::currentClass() const
{
        int index = currentIndex();
        if ( index < 0 || index >= m_classes.size() )
                return 0;
        return m_classes[ index ];
}

void ClassChooser::slotCurrentChanged( int index )
{
        if ( index < 0 || index >= m_classes.size() )
                return;
        emit classSelected( m_classes[ index ] );
}

#include "classchooser.moc"
