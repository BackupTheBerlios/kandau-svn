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
#include <xmldbbackend.h>
#include <notifier.h>
#include <manager.h>

#include "article.h"
#include "customer.h"

#include "notificationstest.h"

void NotificationsTest::allTests()
{
	Notifier *notifier = new Notifier();
	DbBackendIface *backend = new XmlDbBackend( "notificationstest.xml", true );
	Manager *manager = new Manager( backend, notifier );

	m_counter = 0;
	notifier->registerSlot( this, SLOT( slotSomethingModified(const ClassInfo*,const OidType&,const PropertyInfo*,const QVariant&) ) );
	notifier->registerSlot( this, SLOT( slotClassModified(const ClassInfo*,const OidType&,const PropertyInfo*,const QVariant&) ), Classes::classInfo( "Customer" ) );

	ObjectRef<Article> article = Article::create();
	article->setCode( "code" );
	CHECK( m_counter, 1 );

	ObjectRef<Customer> customer = Customer::create();
	customer->setAddress( "address" );
	CHECK( m_counter, 3 );

	notifier->registerSlot( this, SLOT( slotObjectModified(const ClassInfo*,const OidType&,const PropertyInfo*,const QVariant&) ), 0, article->oid() );
	article->setLabel( "Code" );
	CHECK( m_counter, 5 );
	
	notifier->registerSlot( this, SLOT( slotPropertyModified(const ClassInfo*,const OidType&,const PropertyInfo*,const QVariant&) ), 0, customer->oid(), "city" );
	customer->setCountry( "Country" );
	CHECK( m_counter, 7 );
	customer->setCity( "City" );
	CHECK( m_counter, 10 );
	
	notifier->unregisterSlot( this, SLOT( slotPropertyModified(const ClassInfo*,const OidType&,const PropertyInfo*,const QVariant&) ) );
	customer->setCity( "None" );
	CHECK( m_counter, 10 );
	article->setLabel( "None" );
	CHECK( m_counter, 10 );
}

void NotificationsTest::slotSomethingModified( const ClassInfo* classInfo, const OidType& object, const PropertyInfo *property, const QVariant& newValue )
{
	m_counter++;
}

void NotificationsTest::slotClassModified( const ClassInfo* classInfo, const OidType& object, const PropertyInfo *property, const QVariant& newValue )
{
	CHECK( classInfo->name(), QString( "Customer" ) );
	m_counter++;
}

void NotificationsTest::slotObjectModified( const ClassInfo* classInfo, const OidType& object, const PropertyInfo *property, const QVariant& newValue )
{
	CHECK( classInfo->name(), QString( "Article" ) );
	CHECK( object, 1 );
	m_counter++;	
}

void NotificationsTest::slotPropertyModified( const ClassInfo* classInfo, const OidType& object, const PropertyInfo *property, const QVariant& newValue )
{
	CHECK( classInfo->name(), QString( "Customer" ) );
	CHECK( object, 2 );
	CHECK( property->name(), QString( "city" ) );
	m_counter++;
}

#include "notificationstest.moc"
