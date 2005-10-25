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

#include <qapplication.h>
#include <qsqldatabase.h>
#include <qvariant.h>

#include <classes.h>
#include <manager.h>
#include <sqldbbackend.h>
#include <xmldbbackend.h>
#include <labelsmetainfo.h>
#include <dynamicobject.h>
#include <notifier.h>

#include "author.h"
#include "book.h"

int main( int cargs, char **args )
{
	Classes::setup();
	
	QApplication app( cargs, args );
	
/*
	QSqlDatabase *db = QSqlDatabase::addDatabase( "QPSQL7" );
	db->setDatabaseName( "bookshelf" );
	db->setUserName( "albert" );
	db->setPassword( "password" );
	db->setHostName( "localhost" );
	if ( ! db->open() ) {
		kdDebug() << "Failed to open database: " << db->lastError().text() << endl;
		return 1;
	}
*/	
//	DbBackendIface *backend = new SqlDbBackend( db );

	DbBackendIface *backend = new XmlDbBackend( "books.xml", true );
	Manager *manager = new Manager( backend );
	
	//manager->createSchema();
	
	
	// Your stuff here
	ObjectRef<Author> monzo = Author::create();
	monzo->setFirstName( "Quim" );
	monzo->setLastName( "Monzó" );
	monzo->setBirthYear( 1952 );
	monzo->setBiography( "Born in Barcelona in 1952, has worked as a graphic designer, translator, journalist and has contributed to radio and TV programms." );
	
	ObjectRef<Book> benzina = Book::create();
	benzina->setTitle( "Benzina" );
	benzina->setIsbn( "84-7727-434-7" );
	benzina->setYear( 1983 );
	monzo->bibliography()->add( benzina );
	
	kdDebug() << "The author of the book 'Benzina' is " << benzina->author()->fullName() << endl;

	// If you use the SQL backend you should declare this class before
	// creating the schema
	Classes::addClass( "BookExtraInfo", DynamicObject::createInstance, 0 );
	ClassInfo *ci = Classes::classInfo( "BookExtraInfo" );
	ci->addProperty( "notes", QVariant::String );
	ci->addProperty( "numberOfCopies", QVariant::ULongLong );
	ci->addProperty( "dateOfAcquisition", QVariant::Date );
	ci->addObject( "Book", "book", &Book::createInstance );
	ci->addCollection( "Book", "similarBooks" );

	// Browsing classes
	ClassInfoIterator it( Classes::begin() );
	ClassInfoIterator end( Classes::end() );
	ClassInfo *info;
	for ( ; it != end; ++it ) {
		info = *it;
		kdDebug() << "Class: " << info->name() << endl;
		kdDebug() << "\tProperties:" << endl;
		PropertiesInfoConstIterator pit( info->propertiesBegin() );
		PropertiesInfoConstIterator pend( info->propertiesEnd() );
		PropertyInfo *pinfo;
		for ( ; pit != pend; ++pit ) {
			pinfo = *pit;
			kdDebug() << "\t\t" << pinfo->name() << " (" << QVariant::typeToName( pinfo->type() ) << ")" << endl;
		}
	}
	
	// Accessing meta information
	QObject *meta = Classes::classInfo( "Author" )->metaInfo( "labels" );
	if ( meta && meta->inherits( "LabelsMetaInfo" ) ) {
		LabelsMetaInfo *labels = static_cast<LabelsMetaInfo*>( meta );
		PropertiesInfoConstIterator pit( Classes::classInfo( "Author" )->propertiesBegin() );
		PropertiesInfoConstIterator pend( Classes::classInfo( "Author" )->propertiesEnd() );
		PropertyInfo *pinfo;
		for ( ; pit != pend; ++pit ) {
			pinfo = *pit;
			kdDebug() << "Property: " << pinfo->name() << ", Label: " << labels->label( pinfo->name() ) << endl;
		}
	}
	
	Notifier *notifier = dynamic_cast<Notifier*>( Manager::self()->notificationHandler() );
/*	if ( notifier && obj )
		notifier->registerSlot( this, SLOT( slotObjectModified(const ClassInfo*,const OidType&,const PropertyInfo*,const QVariant&) ), 0, obj->oid() );
*/
	delete manager;
}
