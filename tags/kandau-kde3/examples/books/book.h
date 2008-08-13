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
#ifndef BOOK_H
#define BOOK_H

#include <object.h>

using namespace Kandau;

class Author;

class Book : public Object
{
	Q_OBJECT
	Q_PROPERTY( QString title READ title WRITE setTitle )
	Q_PROPERTY( QString isbn READ isbn WRITE setIsbn )
	Q_PROPERTY( uint year READ year WRITE setYear )
public:
	DCLASS( Book );

	void setTitle( const QString& title );
	const QString& title() const;

	void setIsbn( const QString& isbn );
	const QString& isbn() const;

	void setYear( uint year );
	const uint year() const;

	void setAuthor( Author *author );
	Author* author() const;

private:
	QString m_title;
	QString m_isbn;
	uint m_year;
};


#endif
