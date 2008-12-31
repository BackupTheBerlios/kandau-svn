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
#include <assert.h>

#include <QMap>

#include "labelsmetainfo.h"

LabelsMetaInfo::LabelsMetaInfo( const LabelDescription *labels ) : QObject()
{
	assert( labels );
	int i = 0;
	while ( labels[i].name ) {
		m_labels.insert( labels[i].name, labels[i].description );
		i++;
	}
}

QString LabelsMetaInfo::label( const QString& name )
{
	if ( m_labels.contains( name ) )
		return m_labels[ name ];
	else
		return QString();
}

#include "labelsmetainfo.moc"
