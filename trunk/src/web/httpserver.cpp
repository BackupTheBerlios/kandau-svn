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
#include <qsocket.h>
#include <qstringlist.h>
#include <qregexp.h>
#include <qdatetime.h>

#include <kdebug.h>
#include <kurl.h>

#include "httpserver.h"
#include "service.h"

HttpServer::HttpServer(Q_UINT16 port, QObject *parent, const char *name)
 : QServerSocket(port, 1, parent, name)
{
}


HttpServer::~HttpServer()
{
}

void HttpServer::newConnection( int socket )
{
	QSocket *s = new QSocket( this );
	connect( s, SIGNAL(readyRead()), SLOT(readClient()) );
	connect( s, SIGNAL(delayedCloseFinished()), SLOT(discardClient()) );
	s->setSocket( socket );
}

void HttpServer::readClient()
{
	QSocket *socket = (QSocket*)sender();
	socket->setReadBufferSize( 1 );
	KURL url;
	QStringList list;
	kdDebug() << k_funcinfo << endl;
	int i;
	char c;
	while ( 1 ) {
		
		i = socket->getch();
		if ( i != -1 ) {
			kdDebug() << k_funcinfo << c << endl;
			c = i;
			printf( "%c", c );
		} 
		//kdDebug() << k_funcinfo << socket->bytesAvailable() << endl;
		/*
		if ( socket->canReadLine() ) {
			kdDebug() << k_funcinfo << socket->readLine() << endl;
			//QStringList tokens = QStringList::split( QRegExp("[ \r\n][ \r\n]*"), socket->readLine() );
			//kdDebug() << k_funcinfo << tokens.join( " " ) << endl;
		}
		*/
	}

	if ( socket->canReadLine() ) {
		QStringList tokens = QStringList::split( QRegExp("[ \r\n][ \r\n]*"), socket->readLine() );
		kdDebug() << k_funcinfo << tokens.join( " " ) << endl;
		if ( tokens[0] == "GET" ) {
			//path = tokens[1];
			url = tokens[1];
			QTextStream os( socket );
			os.setEncoding( QTextStream::UnicodeUTF8 );
			os << "HTTP/1.0 200 Ok\r\n"
				"Content-Type: text/html; charset=\"utf-8\"\r\n"
				"\r\n";
			//os << " Path: " << url.path();

			if ( m_services.contains( url.path() ) ) {
				QVariant val = m_services[ url.path() ]->process( url.queryItems() );
				switch( m_protocols[ url.path() ] ) {
					case Json:
						os << variantToJson( val );
						break;
					case Xml:
						os << variantToXml( val );
						break;
					case String:
						os << variantToString( val );
						break;
				}
			} else {
				os << "<h1>Service not found</h1>";
			}
/*			os << "HTTP/1.0 200 Ok\r\n"
			"Content-Type: text/html; charset=\"utf-8\"\r\n"
			"\r\n"
			"<h1>" + QDateTime::currentDateTime().toString() + "</h1>\n";*/
			socket->close();
		}
	}
}

void HttpServer::discardClient()
{
	QSocket *socket = (QSocket*)sender();
	delete socket;
}

void HttpServer::addService( const QString & name, Service * service, Protocol protocol )
{
	m_services[ name ] = service;
	m_protocols[ name ] = protocol;
}

void HttpServer::removeService( const QString & name )
{
	m_services.remove( name );
}

QString HttpServer::variantToJson( const QVariant & val )
{
	return "{ 'answer': '" + val.toString() + "' }";
}

QString HttpServer::variantToXml( const QVariant & val )
{
	return "<answer>" + val.toString() + "</answer>";
}

QString HttpServer::variantToString( const QVariant& val )
{
	return val.toString();
}

#include "httpserver.moc"
