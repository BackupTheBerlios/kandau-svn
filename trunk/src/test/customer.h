/***************************************************************************
 *   Copyright (C) 2004 by Albert Cervera Areny                            *
 *   albertca.com                                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef CUSTOMER_H
#define CUSTOMER_H

#include <object.h>

class Article;

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
	Q_PROPERTY( CustomerType t READ type WRITE setType )
	Q_SETS( CustomerType )

public:
	DCLASS( Customer );
	
	enum CustomerType {
		TypeA = 1,
		TypeB = 2,
		TypeC = 4
	};

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
	int type() const;
	void setType( int type );

	Article* defaultArticle() const;
	void setDefaultArticle( Article *article );
	Article* secondDefaultArticle() const;
	void setSecondDefaultArticle( Article *article );
	
	Collection* discountedArticles();
	Collection* adaptedArticles();
private:
	QString m_code;
	QString m_customerName;
	QString m_address;
	QString m_city;
	QString m_zipCode;
	QString m_country;
	int m_type;
};

#endif
