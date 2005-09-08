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
#ifndef CHOOSEOBJECTDIALOG_H
#define CHOOSEOBJECTDIALOG_H

#include <kdialogbase.h>

class Collection;
class Object;
class KListViewSearchLine;
class CollectionListView;

/**
	@author Albert Cervera Areny <albertca@hotpop.com>
*/
class ChooseObjectDialog : public KDialogBase
{
Q_OBJECT
public:
	ChooseObjectDialog( Collection *collection, QWidget *parent = 0 );
	Object* selectedObject() const;
	void setCollection( Collection *collection );
	Collection *collection() const;
private:
	Collection *m_collection;
	KListViewSearchLine *m_searchLine;
	CollectionListView *m_listView;
};

#endif
