/***************************************************************************
 *   Copyright (C) 2004 by Albert Cervera Areny                            *
 *   albertca@hotpop.com                                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef CLASSMAINWINDOW_H
#define CLASSMAINWINDOW_H

#include <kmainwindow.h>

class ClassSelector;
class CollectionListView;
class KListViewSearchLine;
class ClassInfo;
class QListViewItem;
class KDialog;

/**
	@author Albert Cervera Areny <albertca@hotpop.com>
*/
class ClassMainWindow : public KMainWindow
{
	Q_OBJECT
public:
	ClassMainWindow(QWidget *parent = 0, const char *name = 0);

protected:
	void initGUI();
	void fillListView();

protected slots:
	void slotObjectSelected( Object *object );
	void slotDialogFinished();
	void slotDoubleClicked ( QListViewItem *item, const QPoint &, int );
	void slotCurrentClassChanged( const ClassInfo* );
	void slotSave();

private slots:
	void slotSetup();

private:
	QMap<OidType,KDialog*> m_mapDialogs;
	QWidget *m_centralWidget;
	ClassSelector *m_classSelector;
	CollectionListView *m_listView;
	KListViewSearchLine *m_listViewSearchLine;

	ClassInfo *m_currentClass;
};

#endif
