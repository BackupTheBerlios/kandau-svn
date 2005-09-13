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
#include <klocale.h>

#include <labelsmetainfo.h>
#include <defaultpropertymetainfo.h>

#include "customerorder.h"
#include "customer.h"
#include "article.h"

ICLASS( CustomerOrder );

static const LabelDescription customerOrderLabels[] = {
	{ "CustomerOrder", I18N_NOOP( "Customer Order" ) },
	{ "number", I18N_NOOP( "Number" ) },
	{ "date", I18N_NOOP( "Date" ) },
	{ "Customer_CustomerOrder", I18N_NOOP( "Customer" ) },
	{ "CustomerOrder_CustomerOrder", I18N_NOOP( "Customer Order" ) },
	{ "one_to_one", I18N_NOOP( "One To One" ) },
	{ "Article_CustomerOrder", I18N_NOOP( "Article" ) }
};

CustomerOrder::CustomerOrder()
{
	m_number = 0;
}

void CustomerOrder::createRelations()
{
	OBJECTR( Customer, "one_to_one" );
	OBJECT( Customer );
	OBJECT( CustomerOrder ); // For test purposes only, see if the other order is updated
	COLLECTION( Article );
	ADDMETAINFO( "labels", new LabelsMetaInfo( customerOrderLabels ) );
	ADDMETAINFO( "defaultProperty", new DefaultPropertyMetaInfo( "{number} - {date}" ) );
}

Q_ULLONG CustomerOrder::number() const
{
	return m_number;
}

void CustomerOrder::setNumber( Q_ULLONG num )
{
	MODIFIED;
	m_number = num;
}

const QDate& CustomerOrder::date() const
{
	return m_date;
}

void CustomerOrder::setDate( const QDate& date )
{
	MODIFIED;
	m_date = date;
}

Collection* CustomerOrder::articles()
{
	return GETCOLLECTION( Article );
}

Customer* CustomerOrder::customer()
{
	return GETOBJECT( Customer );
}

void CustomerOrder::setCustomer( Customer* customer )
{
	SETOBJECT( Customer, customer );
}

CustomerOrder* CustomerOrder::order()
{
	return GETOBJECT( CustomerOrder );
}

void CustomerOrder::setOrder( CustomerOrder* order )
{
	SETOBJECT( CustomerOrder, order );
}

#include "customerorder.moc"
