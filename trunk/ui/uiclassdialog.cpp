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
#include "uiclassdialog.h"

#include <uiform.h>

using namespace Kandau;
using namespace Kandau::Ui;

UiClassDialog::UiClassDialog(QWidget *parent, const char *name)
 : KDialogBase(parent, name)
{
	showButtonApply( false );
	m_form = new UiForm( this );
	m_form->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
	m_form->setMinimumSize( m_form->sizeHint() );
	setMainWidget( m_form );
	connect( this, SIGNAL(okClicked()), SLOT(slotOkClicked()) );
}

void UiClassDialog::setObject( Object * object )
{
	m_form->setObject( object );
	if ( object )
		setCaption( object->classInfo()->name() + "(" + oidToString(object->oid()) + ")" );
	else
		setCaption( QString::null );
}

Object * UiClassDialog::object( ) const
{
	return m_form->object();
}

void UiClassDialog::setUiFile( const QString & fileName )
{
	m_form->setUiFile( fileName );
}

const QString & UiClassDialog::uiFile( ) const
{
	return m_form->uiFile();
}

void UiClassDialog::slotOkClicked()
{
	m_form->save();
}

#include "uiclassdialog.moc"
