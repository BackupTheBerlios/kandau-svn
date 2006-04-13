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
#ifndef CLASSDIALOG_H
#define CLASSDIALOG_H

#include <kdialogbase.h>
#include <object.h>

class PropertyWidget;

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
	void okClicked( Object* object );
	void cancelClicked( Object* object );

protected:
	static void updateObjectLabel( KURLLabel *objLabel, const Object *obj );
	void slotOk();
	void slotCancel();

private slots:
	void slotObjectSelected( const QString& oid );
	void slotChangeClicked();
	void slotObjectModified( const ClassInfo* classInfo, const OidType& object, const PropertyInfo *property, const QVariant& newValue );

private:
	QMap<QString,PropertyWidget*> m_mapProperties;
	QMap<QString,KURLLabel*> m_mapObjects;
	QMap<const QWidget*,RelationInfo*> m_mapChangeButtons;
	ObjectRef<Object> m_object;
};

#endif
