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
#include <qstring.h>

#include <kapplication.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>

#include "httpserver.h"
#include "listservice.h"
#include "kandauservice.h"

static const KCmdLineOptions options[] =
{
  {"verbose", "Verbose output", 0},
  KCmdLineLastOption
};


int main( int argc, char** argv )
{
	KAboutData aboutData( "tests","Test","0.1" );
	KCmdLineArgs::init( argc, argv, &aboutData );
	KCmdLineArgs::addCmdLineOptions( options );

	KApplication app;
	
	HttpServer server( 8080 );
	server.addService( "/list", new ListService(), HttpServer::String );
	server.addService( "/kandau", new KandauService(), HttpServer::Json );
	server.addService( "/kandau-xml", new KandauService(), HttpServer::Xml );
	server.addService( "/kandau-string", new KandauService(), HttpServer::String );
	return app.exec();
}
