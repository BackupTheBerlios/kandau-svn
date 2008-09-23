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
#include <qregexp.h>
#include <QStringList>

#include "object.h"

#include "defaultpropertymetainfo.h"

DefaultPropertyMetaInfo::DefaultPropertyMetaInfo( const QString& defaultProperty ) : QObject()
{
	m_defaultProperty = defaultProperty;
}

QString DefaultPropertyMetaInfo::defaultPropertyValue( const Object *object )
{
	assert( object );
	QString ret = m_defaultProperty;
	QRegExp ex( "(\\{\\w+\\})" );
	int pos = 0;
	QString item;

	//while ( (pos = ex.search( m_defaultProperty, pos ) ) != -1 ) { // DEPRECATED Qt3 -> Qt4 by Percy
    while ( (pos = ex.indexIn( m_defaultProperty, pos ) ) != -1 ) {
		//ex.capturedTexts();
		item = *(ex.capturedTexts().begin());
		item.remove( "{" );
		item.remove( "}" );
		if ( ! object->containsProperty( item ) ) {
			kdDebug() << k_funcinfo << "Warning: property '" << item << "' not found." << endl;
			continue;
		}
		ret.replace( *(ex.capturedTexts().begin()), object->property( item ).value().toString() );
		pos += ex.matchedLength();
	}
	return ret;
}

#include "defaultpropertymetainfo.moc"
