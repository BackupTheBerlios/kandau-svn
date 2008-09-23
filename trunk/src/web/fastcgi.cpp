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
#include <assert.h>

#include <kdebug.h>

#include "fastcgi.h"


void FastCgi::run()
{
	kdDebug() << "Accepting..." << endl;
	while (FCGX_Accept(&m_in, &m_out, &m_error, &m_parameters) >= 0) {
		kdDebug() << "Accepted" << endl;

		m_outputStream = new QTextStream( new FastCgiDevice( m_out ) );
		m_inputStream = new QTextStream( new FastCgiDevice( m_in ) );

		handleRequest();

		delete m_outputStream;
		delete m_inputStream;
	}
	kdDebug() << "Rejecting..." << endl;
}

QString FastCgi::parameter( const QString& name ) const
{
	return FCGX_GetParam( name.ascii(), m_parameters );
}

QStringList FastCgi::parameters() const
{
	QStringList list;
	char **p;
	for( p = m_parameters; *p != NULL; p++ ) {
		list.append( *p );
	}
	return list;
}

QTextStream & FastCgi::input( )
{
	return *m_inputStream;
}

QTextStream & FastCgi::output( )
{
	return *m_outputStream;	
}

void FastCgi::setContentType( const QString & type )
{
	assert( m_outputStream ); 
	output() << "Content-type: " << type << "\r\n\r\n";
	m_contentType = type;
}

const QString & FastCgi::contentType() const
{
	return m_contentType;
}
