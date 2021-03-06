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
#include <qtimer.h>
#include <qtoolbox.h>
#include <qlayout.h>
#include <qfile.h>
#include <qpopupmenu.h>

#include <klistviewsearchline.h>
#include <kstdaction.h>
#include <klocale.h>

#include <collection.h>
#include <labelsmetainfo.h>

#include "collectionlistview.h"
#include "classmainwindow.h"
#include "classdialog.h"
#include "uiclassdialog.h"
#include "classchooser.h"

ClassMainWindow::ClassMainWindow(QWidget *parent, const char *name)
 : KMainWindow(parent, name)
{
	QTimer::singleShot( 0, this, SLOT( slotSetup() ) );
	initGUI();
}

void ClassMainWindow::initGUI()
{
	m_centralWidget = new QWidget( this );
	setCentralWidget( m_centralWidget );
	QHBoxLayout *layout = new QHBoxLayout( m_centralWidget );

	m_classChooser = new ClassChooser( m_centralWidget );
	m_classChooser->setMaximumWidth( 200 );
	m_classChooser->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
	layout->addWidget( m_classChooser );

	QVBoxLayout *vlayout = new QVBoxLayout( layout );
	m_listView = new CollectionListView( 0, m_centralWidget );
	m_listView->setClassInfo( m_classChooser->currentClass() );


	m_classChooser->load();
	m_listView->setClassInfo( m_classChooser->currentClass() );

	connect( m_listView, SIGNAL(doubleClicked(QListViewItem*,const QPoint&, int)), SLOT(slotDoubleClicked(QListViewItem*,const QPoint&, int)));
	connect( m_listView, SIGNAL(rightButtonClicked(QListViewItem*,const QPoint&,int)), SLOT(slotRightClick(QListViewItem*,const QPoint &,int)) );

	m_listViewSearchLine = new KListViewSearchLine( m_centralWidget );
	m_listViewSearchLine->setListView( m_listView );

	vlayout->addWidget( m_listViewSearchLine );
	vlayout->addWidget( m_listView );

	KStdAction::save( this, SLOT(slotSave()), actionCollection() );

	setupGUI();

	connect( m_classChooser, SIGNAL(classSelected(const ClassInfo*)), SLOT(slotCurrentClassChanged(const ClassInfo*)) );
}

void ClassMainWindow::slotSetup()
{
	m_listView->fill();
}

void ClassMainWindow::slotSave()
{
	Manager::self()->commit();
}

void ClassMainWindow::slotDoubleClicked ( QListViewItem *item, const QPoint &, int )
{
	slotObjectSelected( m_listView->classInfo()->create( stringToOid( item->text( 0 ) ) ) );
}

void ClassMainWindow::slotObjectSelected( Object *object )
{
	if ( m_mapDialogs.contains( object->oid() ) ) {
		m_mapDialogs[ object->oid() ]->setFocus();
	} else {
		QFile f( QString( object->classInfo()->name() ).lower() + ".ui" );
		if ( f.exists() ) {
			UiClassDialog *dialog = new UiClassDialog( this );
			dialog->setObject( object );
			dialog->setUiFile( f.name() );
			dialog->show();
			connect( dialog, SIGNAL(finished()), SLOT(slotDialogFinished()) );
			m_mapDialogs.insert( object->oid(), dialog );
		} else {
			ClassDialog *dialog = new ClassDialog( object, this );
			dialog->show();
			//connect( dialog, SIGNAL(destroyed(QObject*)), SLOT(slotDialogDestroyed(QObject*)) );
			connect( dialog, SIGNAL(finished()), SLOT(slotDialogFinished()) );
			connect( dialog, SIGNAL(objectSelected(Object*)), SLOT(slotObjectSelected(Object*)) );
			m_mapDialogs.insert( object->oid(), dialog );
		}
	}
}

void ClassMainWindow::slotDialogFinished()
{
	const ClassDialog *dialog = dynamic_cast<const ClassDialog*>( sender() );
	if ( dialog ) {
		m_mapDialogs.remove( dialog->object()->oid() );
		if ( dialog->object()->classInfo() == m_classChooser->currentClass() ) {
			m_listView->fill();
		}
	} else {
		const UiClassDialog *dialog = dynamic_cast<const UiClassDialog*>( sender() );
		if ( dialog ) {
			m_mapDialogs.remove( dialog->object()->oid() );
			if ( dialog->object()->classInfo() == m_classChooser->currentClass() ) {
				m_listView->fill();
			}
		}
	}
}

void ClassMainWindow::slotCurrentClassChanged( const ClassInfo *classInfo )
{
	m_listView->setClassInfo( classInfo );
	m_listView->fill();
}

void ClassMainWindow::slotRightClick( QListViewItem *, const QPoint &pos, int )
{
	QPopupMenu *menu = new QPopupMenu( this );
	menu->insertItem( i18n( "&Add" ), this, SLOT(slotAdd()) );
	menu->insertItem( i18n( "&Remove" ), this, SLOT(slotRemove()) );
	menu->popup( pos );
}

void ClassMainWindow::slotAdd( )
{
	slotObjectSelected( m_listView->classInfo()->create() );
}

void ClassMainWindow::slotRemove()
{
	kdDebug() << k_funcinfo << m_listView->currentItem()->text( 0 ) << endl;
	Manager::self()->remove( m_listView->classInfo()->create( stringToOid( m_listView->currentItem()->text( 0 ) ) ) );
	m_listView->fill();
}

#include "classmainwindow.moc"
