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
#include <qvariant.h>

#include "classes.h"
#include "object.h"
#include "notifier.h"

using namespace Kandau;

PossibleEvents::PossibleEvents()
{
	m_classInfo = 0;
	m_object = 0;
	m_property = QString::null;
}

PossibleEvents::PossibleEvents( const PossibleEvents& p )
{
	m_classInfo = p.m_classInfo;
	m_object = p.m_object;
	m_property = p.m_property;
}

PossibleEvents::PossibleEvents( const ClassInfo *classInfo, const OidType& object, const QString &property )
{
	m_classInfo = classInfo;
	m_object = object;
	m_property = property;
}

const ClassInfo* PossibleEvents::classInfo() const
{
	return m_classInfo;
}

const OidType& PossibleEvents::object() const
{
	return m_object;
}

const QString& PossibleEvents::property() const
{
	return m_property;
}

ObjectAndSlot::ObjectAndSlot()
{
	m_object = 0;
	m_slot = QString::null;
}

ObjectAndSlot::ObjectAndSlot( const ObjectAndSlot& p )
{
	m_object = p.m_object;
	m_slot = p.m_slot;
}

ObjectAndSlot::ObjectAndSlot( const QObject* object, const QString& slot )
{
	m_object = object;
	m_slot = slot;
}

bool ObjectAndSlot::operator<( const ObjectAndSlot& p ) const
{
	return m_object < p.m_object;
}

bool ObjectAndSlot::operator==( const ObjectAndSlot& p ) const
{
	return m_object == p.m_object && m_slot == p.m_slot;
}

const QObject* ObjectAndSlot::object() const
{
	return m_object;
}

const QString& ObjectAndSlot::slot() const
{
	return m_slot;
}

void Notifier::registerSlot( const QObject *dstObject, const char* slot, const ClassInfo* classInfo, const OidType& object, const QString& property )
{
	m_slotEvents[ ObjectAndSlot( dstObject, slot ) ].append( PossibleEvents( classInfo, object, property ) );
	m_eventSlots[ classInfo ][ object ][ property ].append( ObjectAndSlot( dstObject, slot ) );
}

void Notifier::unregisterSlot( const QObject *object, const char* slot )
{
	QValueList<PossibleEvents> list = m_slotEvents[ ObjectAndSlot( object, slot ) ];
	QValueListIterator<PossibleEvents> it( list.begin() );
	QValueListIterator<PossibleEvents> end( list.end() );
	for ( ; it != end; ++it ) {
		PossibleEvents e( *it );
		m_eventSlots[ e.classInfo() ][ e.object() ][ e.property() ].remove( ObjectAndSlot( object, slot ) );
	}
	m_slotEvents.remove( ObjectAndSlot( object, slot ) );
}

bool Notifier::propertyModified( const ClassInfo* classInfo, const OidType& object, const QString& property, const QVariant& newValue )
{
	MapSlotEventsConstIterator it( m_slotEvents.constBegin() );
	MapSlotEventsConstIterator end( m_slotEvents.constEnd() );
	QValueList<PossibleEvents> list;
	for ( ; it != end; ++it ) {
		list = *it;
		QValueListIterator<PossibleEvents> lit( list.begin() );
		QValueListIterator<PossibleEvents> lend( list.end() );
		for ( ; lit != lend; ++lit ) {
			const PossibleEvents &e = (*lit);
			if ( e.classInfo() != 0 && e.classInfo() != classInfo )
				continue;
			if ( e.object() != 0 && e.object() != object )
				continue;
			if ( ! e.property().isNull() && e.property() != property )
				continue;
			const ObjectAndSlot &s = it.key();
			connect( this, SIGNAL(modified(const ClassInfo*,const OidType&,const PropertyInfo*,const QVariant&)), s.object(), s.slot() );
		}
	}
	emit modified( classInfo, object, classInfo->property( property ), newValue );
	disconnect( SIGNAL( modified(const ClassInfo*,const OidType&,const PropertyInfo*,const QVariant&)) );
	return true;
}

#include "notifier.moc"
