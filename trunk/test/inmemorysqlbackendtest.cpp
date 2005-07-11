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
#include <qsqldatabase.h>

#include <kdebug.h>

#include <inmemorysqldbbackend.h>
#include <manager.h>
#include <object.h>

#include "inmemorysqlbackendtest.h"
#include "customerorder.h"
#include "article.h"
#include "customer.h"

void InMemorySqlBackendTest::commit()
{
	ObjectRef<CustomerOrder> order = CustomerOrder::create();
	order->setNumber( 50000 );
	order->setDate( QDate::currentDate() );

	printClasses();
	
	order->setOrder( order );

	ObjectRef<Article> a1 = Article::create();
	a1->setCode( "1" );
	a1->setLabel( "Article One" );
	a1->setDescription( "Description of article number one" );
	order->articles()->add( a1 );

	ObjectRef<Article> a2 = Article::create();
	a2->setCode( "2" );
	a2->setLabel( "Article Two" );
	a2->setDescription( "Description of article number two" );
	//a1->setArticle( a2 );

	ObjectRef<Customer> c = Customer::create();
	c->setCode( "0001" );
	c->setCustomerName( "Customer One" );
	c->setAddress( "Street" );
	c->setCity( "City" );
	c->setZipCode( "Zip Code" );
	c->setCountry( "Country" );
	order->setCustomer( c );
}

void InMemorySqlBackendTest::rollback()
{
}

void InMemorySqlBackendTest::printClasses()
{
	kdDebug() << k_funcinfo << endl;

	ClassInfoIterator cit( Classes::begin() );
	ClassInfoIterator cend( Classes::end() );
	ClassInfo *current;
	for ( ; cit != cend; ++cit ) {
		current = (*cit);

		kdDebug() << "Class: " << current->name() << endl;
		kdDebug() << "    Objects:" << endl;
		RelatedObjectsIterator oit( current->objectsBegin() );
		RelatedObjectsIterator oend( current->objectsEnd() );
		RelatedObject *obj;
		for ( ; oit != oend; ++oit ) {
			obj = (*oit);
			QString n;
			n = obj->isOneToOne() ? "1-1" : "N-1";
			kdDebug() << "        Name: " << obj->name() << "  Class: " << obj->relatedClassInfo()->name() << "  Relation: " << n << endl;
		}

		kdDebug() << "    Collections:" << endl;
		RelatedCollectionsIterator lit( current->collectionsBegin() );
		RelatedCollectionsIterator lend( current->collectionsEnd() );
		RelatedCollection *col;
		for ( ; lit != lend; ++lit ) {
			col = (*lit);

			QString n;
			n = col->isNToOne() ? "N-1" : "N-N";
			kdDebug() << "        Name: " << col->name() << "  Class: " << col->childrenClassInfo()->name() << "  Relation: " << n << endl;
		}
	}
}


void InMemorySqlBackendTest::allTests()
{
	Classes::setup();
	QSqlDatabase *db = QSqlDatabase::addDatabase( "QPSQL7" );
	db->setDatabaseName( "testinmemory" );
	db->setUserName( "albert" );
	db->setPassword( "" );
	db->setHostName( "localhost" );
	if ( ! db->open() ) {
		kdDebug() << "Failed to open database: " << db->lastError().text() << endl;
		return;
	}
	DbBackendIface *backend = new InMemorySqlDbBackend( db );

	m_manager = new Manager( backend );
	m_manager->createSchema();

	commit();
	rollback();

	delete m_manager;
}
