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
#include <qsqldatabase.h>

#include <kapplication.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <kdialog.h>

#include <classes.h>
#include <sqldbbackend.h>
#include <manager.h>
#include <notifier.h>

#include <dialoggenerator.h>
#include <classmainwindow.h>

#include "article.h"
#include "customer.h"
#include "customerorder.h"

static const KCmdLineOptions options[] =
{
  {"verbose", "Verbose output", 0},
  KCmdLineLastOption
};


int main( int argc, char** argv )
{
	QString dbname = "test";

	KAboutData aboutData( "tests","Test","0.1" );
	KCmdLineArgs::init( argc, argv, &aboutData );
	KCmdLineArgs::addCmdLineOptions( options );

	KApplication app;
	Classes::setup();

	QSqlDatabase *db = QSqlDatabase::addDatabase( "QPSQL7" );
	db->setDatabaseName( dbname );
	db->setUserName( "albert" );
	db->setPassword( "" );
	db->setHostName( "localhost" );
	if ( ! db->open() ) {
		kdDebug() << "Failed to open database: " << db->lastError().text() << endl;
		return 0;
	}
	//Notifier *notifier = new Notifier();
	DbBackendIface *backend = new SqlDbBackend( db );
	//Manager *manager = new Manager( backend, notifier );
	Manager *manager = new Manager( backend );
	manager->setMaxObjects( 1000 );
/*	Collection col( "CustomerOrder" );
	CollectionIterator it( col.begin() );
	CollectionIterator end( col.end() );
	if ( it != end ) {*/
		ClassMainWindow *window = new ClassMainWindow();
		window->show();
		app.setMainWidget( window );
// 	}
	return app.exec();
}
