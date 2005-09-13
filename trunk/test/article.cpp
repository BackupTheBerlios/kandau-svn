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
#include <assert.h>

#include <qstring.h>

#include <klocale.h>

#include <labelsmetainfo.h>
#include <defaultpropertymetainfo.h>

#include "article.h"
#include "customerorder.h"

ICLASS( Article );

static const LabelDescription articleLabels[] = { 
	{ "Article", I18N_NOOP("Product")},
	{ "code", I18N_NOOP("Code")},
	{ "label", I18N_NOOP("Label") },
	{ "description", I18N_NOOP("Description") },
	LabelDescriptionLast
};

void Article::createRelations()
{
	OBJECT( Article );
	COLLECTION( CustomerOrder );
	ADDMETAINFO( "labels", new LabelsMetaInfo( articleLabels ) );
	ADDMETAINFO( "defaultProperty", new DefaultPropertyMetaInfo( "{label} - {description}" ) );
}

const QString& Article::code() const
{
	return m_code;
}

void Article::setCode( const QString& code )
{
	MODIFIED;
	m_code = code;
}

const QString& Article::label() const
{
	return m_label;
}

void Article::setLabel( const QString& label )
{
	MODIFIED;
	m_label = label;
}

const QString& Article::description() const
{
	return m_description;
}

void Article::setDescription( const QString& description )
{
	MODIFIED;
	m_description = description;
}

Article* Article::article()
{
	return GETOBJECT( Article );
}

void Article::setArticle( Article* article )
{
	SETOBJECT( Article, article );
}

Collection* Article::orders()
{
	return GETCOLLECTION( CustomerOrder );
}

#include "article.moc"
