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
#ifndef NOTIFICATIONSTEST_H
#define NOTIFICATIONSTEST_H


#include <qobject.h>

#include <oidtype.h>

#include "tester.h"

class ClassInfo;
class QVariant;
class PropertyInfo;

/**
	@author Albert Cervera Areny <albertca@hotpop.com>
*/
class NotificationsTest : public QObject, public Tester
{
	Q_OBJECT
public:
	void allTests();

public slots:
	void slotSomethingModified( const ClassInfo* classInfo, const OidType& object, const PropertyInfo *property, const QVariant& newValue );
	void slotClassModified( const ClassInfo* classInfo, const OidType& object, const PropertyInfo *property, const QVariant& newValue );
	void slotObjectModified( const ClassInfo* classInfo, const OidType& object, const PropertyInfo *property, const QVariant& newValue );
	void slotPropertyModified( const ClassInfo* classInfo, const OidType& object, const PropertyInfo *property, const QVariant& newValue );

private:
	uint m_counter;
};

#endif
