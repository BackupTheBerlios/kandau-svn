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
#ifndef AUTHOR_H
#define AUTHOR_H

#include <object.h>

class Collection;

/**
	@author Albert Cervera Areny <albertca@hotpop.com>
*/

class Author : public Object
{
	Q_OBJECT
	Q_PROPERTY( QString firstName READ firstName WRITE setFirstName );
	Q_PROPERTY( QString lastName READ lastName WRITE setLastName );
	Q_PROPERTY( QString fullName READ fullName );
	Q_PROPERTY( QString biography READ biography WRITE setBiography );
	Q_PROPERTY( uint birthYear READ birthYear WRITE setBirthYear );
public:
	DCLASS( Author );

	void setFirstName( const QString& name );
	const QString& firstName() const;

	void setLastName( const QString& name );
	const QString& lastName() const;

	QString fullName() const;

	void setBiography( const QString& biography );
	const QString& biography() const;

	void setBirthYear( uint year );
	uint birthYear() const;

	Collection* bibliography();

private:
	QString m_firstName;
	QString m_lastName;
	QString m_biography;
	uint m_birthYear;
};

#endif
