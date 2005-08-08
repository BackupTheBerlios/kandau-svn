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
#ifndef ORDER_H
#define ORDER_H

#include <qdatetime.h>

#include <object.h>

class Customer;

/**
@author Albert Cervera Areny
*/

class CustomerOrder : public Object
{
	Q_OBJECT
	Q_PROPERTY( Q_ULLONG number READ number WRITE setNumber )
	Q_PROPERTY( QDate date READ date WRITE setDate )

public:
	DCLASS( CustomerOrder );

	CustomerOrder();

	Q_ULLONG number() const;
	void setNumber( Q_ULLONG num );

	const QDate& date() const;
	void setDate( const QDate& date );

	Collection* articles();

	Customer* customer();
	void setCustomer( Customer* customer );

	CustomerOrder* order();
	void setOrder( CustomerOrder* order );

private:
	Q_ULLONG m_number;
	QDate m_date;
};

#endif
