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

#include <klocale.h>

#include <classes.h>
#include <labelsmetainfo.h>

#include "author.h"

ICLASS(Author);

static const LabelDescription authorLabels[] = { 
	{ "firstName", I18N_NOOP("First Name")},
	{ "lastName", I18N_NOOP("Last Name")},
	{ "fullName", I18N_NOOP("Full Name") },
	{ "biography", I18N_NOOP("Biography") },
	{ "birthYear", I18N_NOOP("Birth Year") },
	{ "bibliography", I18N_NOOP("Bibliography") },
	LabelDescriptionLast
};

void Author::createRelations()
{
	COLLECTION( Book );
	ADDMETAINFO( "labels", new LabelsMetaInfo( authorLabels ) );
}

void Author::setFirstName( const QString & name )
{
	m_firstName = name;
}

const QString & Author::firstName( ) const
{
	return m_firstName;
}

void Author::setLastName( const QString & name )
{
	m_lastName = name;
}

const QString & Author::lastName( ) const
{
	return m_lastName;
}

QString Author::fullName( ) const
{
	QString sfirst = m_firstName.trimmed();
	QString slast = m_lastName.trimmed();
	if ( sfirst != "" && slast != "" )
		return slast + ", " + sfirst;
	else
		return sfirst + slast;
}

void Author::setBiography( const QString & biography )
{
	m_biography = biography;
}

const QString & Author::biography( ) const
{
	return m_biography;
}

void Author::setBirthYear( uint year )
{
	m_birthYear = year;
}

uint Author::birthYear( ) const
{
	return m_birthYear;
}

Collection* Author::bibliography()
{
	return GETCOLLECTION( Book );
}

#include "author.moc"
