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
#ifndef UIDIALOG_H
#define UIDIALOG_H

#include <kdialogbase.h>

#include <object.h>

class UiForm;

/**
@author Albert Cervera Areny
*/
class UiClassDialog : public KDialogBase
{
Q_OBJECT
public:
	UiClassDialog(QWidget *parent = 0, const char *name = 0);

	void setObject( Object* object );
	Object* object() const;

	void setUiFile( const QString& fileName );
	const QString& uiFile() const;

protected slots:
	void slotOkClicked();

private:
	UiForm *m_form;
};

#endif
