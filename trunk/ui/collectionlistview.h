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
#ifndef COLLECTIONLISTVIEW_H
#define COLLECTIONLISTVIEW_H

#include <klistview.h>

class ClassInfo;

/**
	@author Albert Cervera Areny <albertca@hotpop.com>
*/
class CollectionListView : public KListView
{
Q_OBJECT
public:
	CollectionListView( const ClassInfo *classInfo = 0, QWidget *parent = 0 );
	void setClassInfo( const ClassInfo *classInfo );
	const ClassInfo* classInfo() const;
	void fill();

private:
	const ClassInfo *m_classInfo;
};

#endif
