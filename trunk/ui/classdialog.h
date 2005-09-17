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
#ifndef CLASSDIALOG_H
#define CLASSDIALOG_H

#include <kdialogbase.h>
#include <object.h>


/**
	@author Albert Cervera Areny <albertca@hotpop.com>
*/
class ClassDialog : public KDialogBase
{
	Q_OBJECT
public:
	ClassDialog( Object* object, QWidget *parent = 0 );
	Object* object() const;

signals:
	void objectSelected( Object* object );

protected:
	static QWidget* createInput( QWidget* parent, const Property& property );
	static QVariant readInput( QWidget* widget );
	static void updateObjectLabel( KURLLabel *objLabel, const Object *obj );

private slots:
	void slotObjectSelected( const QString& oid );
	void slotOkClicked();
	void slotChangeClicked();
	void slotObjectModified( const ClassInfo* classInfo, const OidType& object, const PropertyInfo *property, const QVariant& newValue );

private:
	QMap<QString,QWidget*> m_mapProperties;
	QMap<QString,KURLLabel*> m_mapObjects;
	QMap<const QWidget*,RelatedObject*> m_mapChangeButtons;
	ObjectRef<Object> m_object;
};

#endif
