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
#include "xmlbackendtest.h"

#include <manager.h>
#include <xmldbbackend.h>
#include <object.h>
#include "customerorder.h"
#include "article.h"

void XmlBackendTest::commit()
{
	ObjectRef<CustomerOrder> order = CustomerOrder::create();
	order->setNumber( 50000 );
	order->setDate( QDate::currentDate() );


	ObjectRef<Article> a1 = Article::create();
	a1->setCode( "1" );
	a1->setLabel( "Article One" );
	a1->setDescription( "Description of article number one" );

	order->articles()->add( a1 );
	Manager::self()->commit();
}

void XmlBackendTest::rollback()
{

}

void XmlBackendTest::setup()
{

}

void XmlBackendTest::shutdown()
{

}

void XmlBackendTest::allTests()
{
	Classes::setup();
	DbBackendIface *backend = new XmlDbBackend( "database.xml" );
	m_manager = new Manager( backend );
	Manager::self()->setMaxObjects( Manager::Unlimited );
	m_manager->createSchema();

	commit();

	delete m_manager;
}
