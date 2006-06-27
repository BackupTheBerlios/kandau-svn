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
#ifndef CLASSABA_H
#define CLASSABA_H

#include "classab.h"

/**
	@author Albert Cervera Areny <albertca@hotpop.com>
*/
class ClassABA : public ClassAB
{
	Q_OBJECT
	Q_PROPERTY( QString stringABA READ stringABA WRITE setStringABA )
public:
	DCLASS( ClassABA );
	
	const QString& stringABA() const;
	void setStringABA( const QString& string );

private:
	QString m_stringABA;
};

#endif
