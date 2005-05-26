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
#ifndef LABELS_H
#define LABELS_H

#include <qstring.h>
#include <qmap.h>

/**
@author Albert Cervera Areny
*/
#define LABEL( property, label ) Labels::add( property, label );

class Labels
{
public:
	static void setDefaultClass( const QString &className );
	static void add( const QString &className, const QString &property, const QString &label  );
	static void add( const QString &property, const QString &label  );
	static QString label( const QString &className, const QString &property );
	static void dump();
private:
	static QMap<QString, QString> *m_labels;
	static QString m_defaultClass;
};


#endif
