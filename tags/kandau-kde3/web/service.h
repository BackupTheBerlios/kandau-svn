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
#ifndef SERVICE_H
#define SERVICE_H

#include <qstring.h>
#include <qmap.h>
#include <qobject.h>
#include <qvariant.h>

/**
	@author Albert Cervera Areny <albertca@hotpop.com>
*/
class Service : public QObject 
{
	Q_OBJECT
public:
	QVariant process( const QMap<QString,QString>& parameters );

//	const QString& answer() const;
	const QVariant& answer() const;

signals:
	void run();

protected:
//	void setAnswer( const QString& text );
	void setAnswer( const QVariant& val );

private:
	QMap<QString, QString> m_parameters;
	QVariant m_answer;
	//QString m_answer;
	
};

#endif
