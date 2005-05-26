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
#include <stdarg.h>

#include <kdebug.h>

#include "labels.h"

QMap<QString, QString>* Labels::m_labels = 0;
QString Labels::m_defaultClass;

void Labels::setDefaultClass( const QString &className )
{
	m_defaultClass = className;
}

void Labels::add( const QString &property, const QString &label )
{
	add( m_defaultClass, property, label );
}

void Labels::add( const QString &className, const QString &property, const QString &label )
{
	if ( ! m_labels )
		m_labels = new QMap<QString, QString>();
	m_labels->insert( className + "-" + property, label );
}

QString Labels::label( const QString &className, const QString &property )
{
	return (*m_labels)[ className + "-" + property ];
}

void Labels::dump()
{
	QMapConstIterator <QString,QString> it( m_labels->constBegin() );
	QMapConstIterator <QString,QString> end( m_labels->constEnd() );
	kdDebug() << "Labels::dump()" << endl;
	for ( ; it != end; ++it )
		kdDebug() << "Key = " << it.key() << ", Value = " << (*it) << endl;
}
