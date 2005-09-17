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
#ifndef NOTIFIER_H
#define NOTIFIER_H

#include <qvaluelist.h>
#include <qobject.h>
#include <qmap.h>

#include "notificationhandler.h"
#include "oidtype.h"

class ClassInfo;
class Object;
class PropertyInfo;
class QVariant;

class PossibleEvents
{
public:
	PossibleEvents();
	PossibleEvents( const PossibleEvents &p );
	PossibleEvents( const ClassInfo *classInfo, const OidType& object, const QString &property );
	const ClassInfo* classInfo() const;
	const OidType& object() const;
	const QString& property() const;
private:
	const ClassInfo *m_classInfo;
	QString m_property;
	OidType m_object;
};

class ObjectAndSlot
{
public:
	ObjectAndSlot();
	ObjectAndSlot( const ObjectAndSlot& p );
	ObjectAndSlot( const QObject* object, const QString& slot );
	const QObject* object() const;
	const QString& slot() const;
	bool operator<( const ObjectAndSlot& p ) const;
	bool operator==( const ObjectAndSlot& p ) const;
private:
	const QObject* m_object;
	QString m_slot;
};

typedef QMap<QString,QValueList<ObjectAndSlot> > MapPropertySlots;
typedef QMapConstIterator<QString,QValueList<ObjectAndSlot> > MapConstIteratorPropertySlots;

//typedef QMap<PropertyInfo*,QStringList> MapPropertySlots;
//typedef QMapConstIterator<PropertyInfo*,QStringList> MapConstIteratorPropertySlots;

typedef QMap<OidType,MapPropertySlots> MapObjectProperty;
typedef QMapConstIterator<OidType,MapPropertySlots> MapConstIteratorObjectProperty;

typedef QMap<const ClassInfo*,MapObjectProperty> MapClassInfoObject;
typedef QMapConstIterator<const ClassInfo*,MapObjectProperty> MapConstIteratorClassInfoObject;

//typedef QMap<QString,PossibleEvents> MapSlotEvents;
//typedef QMapConstIterator<QString,PossibleEvents> MapConstIteratorSlotEvents;

typedef QMap<ObjectAndSlot,PossibleEvents> MapSlotEvents;
typedef QMapConstIterator<ObjectAndSlot,PossibleEvents> MapSlotEventsConstIterator;

/**
	@author Albert Cervera Areny <albertca@hotpop.com>
*/
class Notifier : public QObject, public NotificationHandler
{
	Q_OBJECT
public:
	void registerSlot( const QObject *dstObject, const char* slot, const ClassInfo* classInfo = 0, const OidType& object = 0, const QString& property = QString::null );
	void unregisterSlot( const QObject *object, const char* slot );
	bool propertyModified( const ClassInfo* classInfo, const OidType& object, const QString& property, const QVariant& newValue );
	
signals:
	void modified( const ClassInfo* classInfo, const OidType& object, const PropertyInfo *property, const QVariant& newValue );

private:
	MapSlotEvents m_slotEvents;
	MapClassInfoObject m_eventSlots;
//	MapClassInfoObject m_classInfoObject;
//	MapObjectProperty m_objectProperty;
//	MapPropertySlots m_propertySlot;
};

#endif
