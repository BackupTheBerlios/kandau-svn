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
#ifndef CLASSA_H
#define CLASSA_H

#include <qdatetime.h>

#include <object.h>

/**
	@author Albert Cervera Areny <albertca@hotpop.com>
*/
class ClassA : public Object
{
	Q_OBJECT
	Q_PROPERTY( QString stringA READ stringA WRITE setStringA )
	Q_PROPERTY( QDate dateA READ dateA WRITE setDateA );
public:
	DCLASS( ClassA );

	const QString& stringA() const;
	void setStringA( const QString& string );
	
	const QDate& dateA() const;
	void setDateA( const QDate& date );

private:
	QString m_stringA;
	QDate m_dateA;
};

#endif
