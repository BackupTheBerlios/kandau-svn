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
#ifndef FASTCGIDEVICE_H
#define FASTCGIDEVICE_H

#include <qiodevice.h>

#include <fcgiapp.h>

/**
	@author Albert Cervera Areny <albertca@hotpop.com>
*/

class FastCgiDevice : public QIODevice
{
public:
	FastCgiDevice( FCGX_Stream *stream );

	void setStream( FCGX_Stream *stream );
	FCGX_Stream* stream() const;

	bool open( int mode );
	void close();
	void flush();
	
	Q_ULONG size() const;
	Q_ULONG  at() const;        // non-pure virtual
	bool at( int );         // non-pure virtual
	bool atEnd() const;     // non-pure virtual
	
	Q_LONG readBlock( char *data, Q_ULONG maxlen );
	Q_LONG writeBlock( const char *data, Q_ULONG len );
	Q_LONG readLine( char *data, uint maxlen );
	
	int getch();
	int putch( int );
	int ungetch( int );
private:
	FCGX_Stream *m_stream;
};

#endif
