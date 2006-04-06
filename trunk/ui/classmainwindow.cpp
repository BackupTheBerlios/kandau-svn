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

#include <klistviewsearchline.h>
#include <kstdaction.h>

#include <collection.h>
#include <labelsmetainfo.h>

#include "collectionlistview.h"
#include "classmainwindow.h"
#include "classdialog.h"

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

	m_classSelector = new QToolBox( m_centralWidget );
	m_classSelector->setMaximumWidth( 200 );
	m_classSelector->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
	layout->addWidget( m_classSelector );

	QVBoxLayout *vlayout = new QVBoxLayout( layout );
	m_listView = new CollectionListView( 0, m_centralWidget );
	m_listView->clear();

	ClassInfoIterator it( Classes::begin() );
	ClassInfoIterator end( Classes::end() );
	if ( it != end )
		m_listView->setClassInfo( it.data() );

	for ( int i = 0; it != end; ++it, ++i ) {
		ClassInfo *info = it.data();
		kdDebug() << k_funcinfo << "Adding class '" << info->name() << "'" << endl;
		LabelsMetaInfo *labels = dynamic_cast<LabelsMetaInfo*>( info->metaInfo( "labels" ) );
		m_mapClasses.insert( i, info );
		if ( labels )
			m_classSelector->addItem( new QWidget(m_classSelector), labels->label( info->name() ) );
		else
			m_classSelector->addItem( new QWidget(m_classSelector), info->name() );
	}


	connect( m_listView, SIGNAL(doubleClicked(QListViewItem*,const QPoint&, int)), SLOT(slotDoubleClicked(QListViewItem*,const QPoint&, int)));

	m_listViewSearchLine = new KListViewSearchLine( m_centralWidget );
	m_listViewSearchLine->setListView( m_listView );

	vlayout->addWidget( m_listViewSearchLine );
	vlayout->addWidget( m_listView );

	KStdAction::save( this, SLOT(slotSave()), actionCollection() );

	setupGUI();

	// Connect m_classSelector at the end to avoid receiving signals while filling it
	connect( m_classSelector, SIGNAL(currentChanged(int)), SLOT(slotCurrentClassChanged(int)) );
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
	kdDebug() << k_funcinfo << " Oid: " << item->text(0) << endl;
	slotObjectSelected( m_listView->classInfo()->create( stringToOid( item->text( 0 ) ) ) );
}

void ClassMainWindow::slotObjectSelected( Object *object )
{
	kdDebug() << k_funcinfo << "Object: " << object << endl;
	if ( m_mapDialogs.contains( object->oid() ) ) {
		m_mapDialogs[ object->oid() ]->setFocus();
	} else {
		ClassDialog *dialog = new ClassDialog( object, this );
		dialog->show();
		//connect( dialog, SIGNAL(destroyed(QObject*)), SLOT(slotDialogDestroyed(QObject*)) );
		connect( dialog, SIGNAL(finished()), SLOT(slotDialogFinished()) );
		connect( dialog, SIGNAL(objectSelected(Object*)), SLOT(slotObjectSelected(Object*)) );
		m_mapDialogs.insert( object->oid(), dialog );
	}
}

void ClassMainWindow::slotDialogFinished()
{
	const ClassDialog *dialog = dynamic_cast<const ClassDialog*>( sender() );
	if ( dialog )
		m_mapDialogs.remove( dialog->object()->oid() );
}

void ClassMainWindow::slotCurrentClassChanged( int i )
{
	m_listView->setClassInfo( m_mapClasses[ i ] );
	m_listView->fill();
}

#include "classmainwindow.moc"
