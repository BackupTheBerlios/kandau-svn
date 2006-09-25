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
#include <qlayout.h>
#include <qlabel.h>
#include <qdragobject.h>

#include <kstdguiitem.h>
#include <kpushbutton.h>
#include <klocale.h>

#include "collectionchooser.h"
#include "collectionlistview.h"

using namespace Kandau;
using namespace Kandau::Ui;

CollectionChooser::CollectionChooser(QWidget *parent, const char *name)
 : QWidget(parent, name)
{
	QLabel *lblAvailable = new QLabel( this );
	lblAvailable->setText( i18n( "Available:" ) );

	m_available = new CollectionListView( 0, this );
	m_available->setDragEnabled( true );
	connect( m_available, SIGNAL(doubleClicked(QListViewItem *, const QPoint &, int)), SLOT(addClicked()) );
	connect( m_available, SIGNAL(dropped(QDropEvent*)), SLOT(droppedAvailable(QDropEvent*)) );

	m_add = new KPushButton( this );
	m_add->setText( i18n( "&Add" ) );
	//m_add->setGuiItem( KStdGuiItem::add() );
	connect( m_add, SIGNAL(clicked()), SLOT(addClicked()) );

	m_addAll = new KPushButton( this );
	//m_addAll->setIconSet( KStdGuiItem::add().iconSet() );
	m_addAll->setText( i18n( "Add A&ll" ) );
	connect( m_addAll, SIGNAL(clicked()), SLOT(addAllClicked()) );

	m_remove = new KPushButton( this );
	m_remove->setText( i18n( "&Remove" ) );
	//m_remove->setGuiItem( KStdGuiItem::remove() );
	connect( m_remove, SIGNAL(clicked()), SLOT(removeClicked()) );

	m_removeAll = new KPushButton( this );
	//m_removeAll->setIconSet( KStdGuiItem::remove().iconSet() );
	m_removeAll->setText( i18n( "R&emove All" ) );
	connect( m_removeAll, SIGNAL(clicked()), SLOT(removeAllClicked()) );

	QLabel *lblChoosen = new QLabel( this );
	lblChoosen->setText( i18n( "Selected:" ) );

	m_choosen = new CollectionListView( 0, this );
	m_choosen->setDragEnabled( true );
	connect( m_choosen, SIGNAL(doubleClicked(QListViewItem *, const QPoint &, int)), SLOT(removeClicked()) );
	connect( m_choosen, SIGNAL(dropped(QDropEvent*)), SLOT(droppedChoosen(QDropEvent*)) );

	QVBoxLayout *layAvailable = new QVBoxLayout();
	layAvailable->addSpacing( 10 );
	layAvailable->add( lblAvailable );
	layAvailable->addSpacing( 10 );
	layAvailable->add( m_available );

	QVBoxLayout *layButtons = new QVBoxLayout();
	layButtons->addSpacing( 10 );
	layButtons->add( m_add );
	layButtons->addSpacing( 10 );
	layButtons->add( m_addAll );
	layButtons->addSpacing( 30 );
	layButtons->add( m_remove );
	layButtons->addSpacing( 10 );
	layButtons->add( m_removeAll );
	layButtons->addStretch();

	QVBoxLayout *layChoosen = new QVBoxLayout();
	layChoosen->addSpacing( 10 );
	layChoosen->add( lblChoosen );
	layChoosen->addSpacing( 10 );
	layChoosen->add( m_choosen );

	QHBoxLayout *layout = new QHBoxLayout( this );
	layout->addLayout( layAvailable );
	layout->addSpacing( 10 );
	layout->addLayout( layButtons );
	layout->addSpacing( 10 );
	layout->addLayout( layChoosen );
}

void CollectionChooser::setObject( Object * object )
{
	m_object = object;
}

Object * CollectionChooser::object( ) const
{
	return m_object;
}

void CollectionChooser::setCollection( Collection * collection )
{
	m_collection = collection;
}

Collection * CollectionChooser::collection() const
{
	return m_collection;
}

void CollectionChooser::setCollectionName( const QString & collectionName )
{
	if ( ! m_object.isNull() && m_object->containsCollection( collectionName ) ) {
		m_available->setClassInfo( m_object->classInfo()->collection( collectionName )->childrenClassInfo() );
		m_available->fill();
		m_choosen->setClassInfo( m_object->classInfo()->collection( collectionName )->childrenClassInfo() );
	}
	m_collectionName = collectionName;
}

const QString & CollectionChooser::collectionName( ) const
{
	return m_collectionName;
}

void CollectionChooser::addClicked()
{
	if ( m_object.isNull() )
		return;

	if ( m_choosen->findItem( oidToString( m_available->currentOid() ), 0 ) == 0 )
		m_choosen->add( m_available->currentObject() );
}

void CollectionChooser::addAllClicked()
{
	if ( m_object.isNull() )
		return;
}

void CollectionChooser::removeClicked()
{
	if ( m_object.isNull() )
		return;
	m_choosen->remove( m_choosen->currentOid() );
}

void CollectionChooser::removeAllClicked()
{
	if ( m_object.isNull() )
		return;
}

void CollectionChooser::droppedAvailable( QDropEvent* event )
{
	QString text;
	if ( QTextDrag::decode( event, text ) )
		m_choosen->remove( stringToOid( text ) );
}

void CollectionChooser::droppedChoosen( QDropEvent* event )
{
	QString text;
	if ( ! QTextDrag::decode( event, text ) )
		return;

	if ( m_choosen->findItem( text, 0 ) ) {
		m_available->setCurrentItem( m_available->findItem( text, 0 ) );
		m_choosen->add( m_available->currentObject() );
	}
}

#include "collectionchooser.moc"
