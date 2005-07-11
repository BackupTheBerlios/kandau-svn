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

#include "customerorder.h"
#include "customer.h"
#include "article.h"

ICLASS( CustomerOrder );

void CustomerOrder::createRelations()
{
	OBJECT( Customer );
	OBJECT( CustomerOrder ); // For test purposes only, see if the other order is updated
	COLLECTION( Article );
}

void CustomerOrder::createLabels()
{
	LABEL( "number", i18n( "Number" ) );
	LABEL( "date", i18n( "Date" ) );
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
