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
#include <desktopconfigdbbackend.h>
#include <manager.h>

#include "desktopconfigbackendtest.h"
#include "article.h"
#include "customerorder.h"
#include "customer.h"

void DesktopConfigBackendTest::allTests()
{
	DesktopConfigDbBackend *backend = new DesktopConfigDbBackend( "desktopconfigtest.desktop" );
	Manager *manager = new Manager( backend );
		
	ObjectRef<Article> article = Article::create();
	article->setCode( "This is the code" );
	article->setLabel( "This is the label" );
	article->setDescription( "This is the description" );
	
	ObjectRef<Customer> customer = Customer::create();
	customer->setCustomerName( "Name" );
	customer->setAddress( "Address" );
	customer->setCity( "City" );
	customer->setCode( "Code" );
	customer->setCountry( "Country" );
	customer->setZipCode( "Zip Code" );
	
	ObjectRef<CustomerOrder> order = CustomerOrder::create();
	order->setDate( QDate::currentDate() );
	order->setNumber( 10000 );
	
	manager->commit();
	delete manager;
}


