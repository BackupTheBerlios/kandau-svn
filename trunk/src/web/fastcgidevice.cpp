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
#include "fastcgidevice.h"

FastCgiDevice::FastCgiDevice( FCGX_Stream *stream )
{
	m_stream = stream;
}

void FastCgiDevice::setStream( FCGX_Stream *stream )
{
	m_stream = stream;
}

FCGX_Stream* FastCgiDevice::stream() const
{
	return m_stream;
}

bool FastCgiDevice::open( int mode )
{
	return true;
}

void FastCgiDevice::close()
{
	// Warning!! Not recommended by FCGI documentation
	FCGX_FClose( m_stream );
}

void FastCgiDevice::flush()
{
	// Warning!! Not recommended by FCGI documentation
	FCGX_FFlush( m_stream );
}

Q_ULONG FastCgiDevice::size() const
{
	return 0;
}

Q_ULONG  FastCgiDevice::at() const
{
	return 0;
}

bool FastCgiDevice::at( int )
{
	return true;
}

bool FastCgiDevice::atEnd() const
{
	return FCGX_HasSeenEOF( m_stream );
}

Q_LONG FastCgiDevice::readBlock( char *data, Q_ULONG maxlen )
{
	return FCGX_GetStr( data, maxlen, m_stream );
}

Q_LONG FastCgiDevice::writeBlock( const char *data, Q_ULONG len )
{
	return FCGX_PutStr( data, len, m_stream );
}

Q_LONG FastCgiDevice::readLine( char *data, uint maxlen )
{
	// Return bytes read including \0
	if ( FCGX_GetLine( data, maxlen, m_stream ) == 0 )
		return 0;
	else
		return strnlen( data, maxlen ) + 1; 
}

int FastCgiDevice::getch()
{
	return FCGX_GetChar( m_stream ); 
}

int FastCgiDevice::putch( int c )
{
	return FCGX_PutChar( c, m_stream );
}

int FastCgiDevice::ungetch( int c )
{
	return FCGX_UnGetChar( c, m_stream );
}
