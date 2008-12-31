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
#ifndef LABELSMETAINFO_H
#define LABELSMETAINFO_H

#include <qobject.h>

/**
	@author Albert Cervera Areny <albertca@hotpop.com>
*/

struct LabelDescription
{
	const char *name;
	const char *description;
};

#define LabelDescriptionLast { 0, 0 }

class LabelsMetaInfo : public QObject
{
	Q_OBJECT
public:
	LabelsMetaInfo( const LabelDescription *labels );
	QString label( const QString& name );
private:
	QMap<QString,QString> m_labels;
};

#endif
