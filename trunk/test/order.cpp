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

#include <labels.h>

#include "order.h"
#include "customer.h"
#include "article.h"

ICLASS( Order );

void Order::createRelations()
{
	OBJECT( Customer );
	OBJECT( Order ); // For test purposes only, see if the other order is updated
	COLLECTION( Article );
}

void Order::createLabels()
{
	LABEL( "number", i18n( "Number" ) );
	LABEL( "date", i18n( "Date" ) );
}

Q_ULLONG Order::number() const
{
	return m_number;
}

void Order::setNumber( Q_ULLONG num )
{
	MODIFIED;
	m_number = num;
}

const QDate& Order::date() const
{
	return m_date;
}

void Order::setDate( const QDate& date )
{
	MODIFIED;
	m_date = date;
}

Collection* Order::articles()
{
	return GETCOLLECTION( Article );
}

Customer* Order::customer()
{
	return GETOBJECT( Customer );
}

void Order::setCustomer( Customer* customer )
{
	SETOBJECT( Customer, customer );
}

Order* Order::order()
{
	return GETOBJECT( Order );
}

void Order::setOrder( Order* order )
{
	SETOBJECT( Order, order );
}

#include "order.moc"
