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
#include <kdebug.h>

#include "service.h"

QVariant Service::process( const QMap< QString, QString > & parameters )
{
	m_parameters = parameters;
	kdDebug() << k_funcinfo << QString( " " ) << endl;
	if ( ! parameters.contains( "function" ) )
		return QString::null;
	kdDebug() << k_funcinfo << endl;
	m_parameters.remove( "function" );
	kdDebug() << k_funcinfo << ": " << SLOT( list() ) << endl;
	connect( this, SIGNAL( run() ), "1" + parameters[ "function" ] + "()" );
	emit run();
	return answer();
}

void Service::setAnswer( const QVariant & val )
{
	m_answer = val;
}

const QVariant & Service::answer( ) const
{
	return m_answer;
}

/*
const QString & Service::answer( ) const
{
	return m_answer;
}

void Service::setAnswer( const QString & text )
{
	m_answer = text;
}
*/

#include "service.moc"
