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
#include "dialoggenerator.h"
#include "classdialog.h"

KDialog* DialogGenerator::generateDialog( QWidget* parent, Object* object )
{
	//KDialogBase *dialog = new KDialogBase( parent );
	ClassDialog *dialog = new ClassDialog( object, parent );
	return dialog;
}

/*
QWidget* DialogGenerator::createInput( QWidget* parent, const Property& property )
{
	QWidget *widget;
	switch ( property.type() ) {
		case QVariant::String: {
			KLineEdit *line = new KLineEdit( parent );
			line->setText( property.value().toString() );
			widget = line;
			break;	
		}
		case QVariant::LongLong:
		case QVariant::ULongLong:
		case QVariant::Int:
		case QVariant::UInt: {
			KIntNumInput *input = new KIntNumInput( parent );
			input->setValue( property.value().toLongLong() );
			widget = input;
			break;
		}
		case QVariant::Date: {
			KDateWidget *date = new KDateWidget( parent );
			date->setDate( property.value().toDate() );
			widget = date;
			break;
		}
		case QVariant::Time: {
			KTimeWidget *time = new KTimeWidget( parent );
			time->setTime( property.value().toTime() );
			widget = time;
			break;
		}
		case QVariant::DateTime: {
			break;
		}
		case QVariant::Double: {
			break;
		}
		default: {
			break;
		}
	}
	return widget;
}
*/
