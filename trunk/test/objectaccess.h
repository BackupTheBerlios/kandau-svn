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
#ifndef OBJECTACCESS_H
#define OBJECTACCESS_H

#include <kdialog.h>
#include <kapplication.h>

#include <oidtype.h>

#include "dcopiface.h"

/**
@author Albert Cervera Areny
*/
class ObjectAccess : public QObject, virtual public DCOPIface
{
Q_OBJECT

public:
	ObjectAccess( KApplication *app );

	QStringList classes() const;
	QStringList properties( const QString& className ) const;
	QStringList relatedObjects( const QString& className ) const;
	QStringList relatedCollections( const QString& className ) const;
	QString create( const QString& className ) const;
	QStringList objects() const;
	bool commit();
	bool rollback();
	void reset();

	QString property( const QString& className, const QString& oid, const QString& property );
	void setProperty( const QString& className, const QString& oid, const QString& property, const QString& value );

	QString object( const QString& className, const QString& oid, const QString& object );
	void setObject( const QString& className, const QString& oid, const QString& object, const QString& value );

	QStringList collection( const QString& className, const QString& oid, const QString& collection );
	void add( const QString& className, const QString& oid, const QString& collection, const QString& value );
	void remove( const QString& className, const QString& oid, const QString& collection, const QString& value );
};

#endif
