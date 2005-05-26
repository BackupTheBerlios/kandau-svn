/***************************************************************************
 *   Copyright (C) 2004 by Albert Cervera Areny                            *
 *   albertca.com                                                   *
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
#ifndef CUSTOMER_H
#define CUSTOMER_H

#include <object.h>

/**
@author Albert Cervera Areny
*/

class Customer : public Object
{
	Q_OBJECT
	Q_PROPERTY( QString code READ code WRITE setCode )
	Q_PROPERTY( QString customerName READ customerName WRITE setCustomerName )
	Q_PROPERTY( QString address READ address WRITE setAddress )
	Q_PROPERTY( QString city READ city WRITE setCity )
	Q_PROPERTY( QString zipCode READ zipCode WRITE setZipCode )
	Q_PROPERTY( QString country READ country WRITE setCountry )

public:
	DCLASS( Customer );

	const QString& code() const;
	void setCode( const QString& code );
	const QString& customerName() const;
	void setCustomerName( const QString& name );
	const QString& address() const;
	void setAddress( const QString& address );
	const QString& city() const;
	void setCity( const QString& city );
	const QString& zipCode() const;
	void setZipCode( const QString& zipCode );
	const QString& country() const;
	void setCountry( const QString& country );

private:
	QString m_code;
	QString m_customerName;
	QString m_address;
	QString m_city;
	QString m_zipCode;
	QString m_country;
};

#endif
