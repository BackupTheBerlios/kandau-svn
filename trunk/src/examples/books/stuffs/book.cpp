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
#include <qstring.h>

#include <classes.h>

#include "book.h"
#include "author.h"

ICLASS( Book );

void Book::createRelations()
{
	OBJECT( Author );
}

void Book::setTitle( const QString & title )
{
	m_title = title;
}

const QString & Book::title( ) const
{
	return m_title;
}

void Book::setIsbn( const QString & isbn )
{
	m_isbn = isbn;
}

const QString & Book::isbn( ) const
{
	return m_isbn;
}

void Book::setYear( uint year )
{
	m_year = year;
}

const uint Book::year( ) const
{
	return m_year;
}

void Book::setAuthor( Author *author )
{
	SETOBJECT( Author, author );
}

Author* Book::author() const
{
	return GETOBJECT( Author );
}

#include "book.moc"
