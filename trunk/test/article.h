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
#ifndef ARTICLE_H
#define ARTICLE_H


#include <qobject.h>
#include <object.h>
#include <defs.h>

/**
@author Albert Cervera Areny
*/
class Article : public Object
{
	Q_OBJECT
	Q_PROPERTY( QString code READ code WRITE setCode )
	Q_PROPERTY( QString label READ label WRITE setLabel )
	Q_PROPERTY( QString description READ description WRITE setDescription )

public:
	DCLASS( Article );

	const QString& code() const;
	void setCode( const QString& code );

	const QString& label() const;
	void setLabel( const QString& label );

	const QString& description() const;
	void setDescription( const QString& description );

	Article* article();
	void setArticle( Article* article );

	Collection* orders();

private:
	QString m_code;
	QString m_label;
	QString m_description;
};

#endif
