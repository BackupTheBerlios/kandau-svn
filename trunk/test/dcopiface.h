/***************************************************************************
 *   Copyright (C) 2004 by Albert Cervera Areny                            *
 *   albertca.com                                                   *
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

#include <qstringlist.h>
#include <qvariant.h>

#include <dcopobject.h>

 class DCOPIface : virtual public DCOPObject
{
K_DCOP
k_dcop:
	virtual QStringList classes() const = 0;
	virtual QStringList properties( const QString& className ) const = 0;
	virtual QStringList relatedObjects( const QString& className ) const = 0;
	virtual QStringList relatedCollections( const QString& className ) const = 0;
	virtual QString create( const QString& className ) const = 0;
	virtual QStringList objects() const = 0;
	virtual bool commit() = 0;
	virtual bool rollback() = 0;
	virtual void reset() = 0;

	virtual QString property( const QString& className, const QString& oid, const QString& property ) = 0;
	virtual void setProperty( const QString& className, const QString& oid, const QString& property, const QString& value ) = 0;
	
	virtual QString object( const QString& className, const QString& oid, const QString& object ) = 0;
	virtual void setObject( const QString& className, const QString& oid, const QString& object, const QString& value ) = 0;

	virtual QStringList collection( const QString& className, const QString& oid, const QString& collection ) = 0;
	virtual void add( const QString& className, const QString& oid, const QString& collection, const QString& value ) = 0;
	virtual void remove( const QString& className, const QString& oid, const QString& collection, const QString& value ) = 0;

};
