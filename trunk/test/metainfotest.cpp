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
#include <labelsmetainfo.h>
#include <object.h>
#include <classes.h>

#include "metainfotest.h"

void MetaInfoTest::labels()
{
	QObject *meta = Classes::classInfo( "Article" )->metaInfo( "labels" );
	assert( meta );
	assert( meta->inherits( "LabelsMetaInfo" ) );
	LabelsMetaInfo *labels = static_cast<LabelsMetaInfo*>( meta );
	CHECK( labels->label( "code" ), QString( "Code" ) );
	CHECK( labels->label( "label" ), QString( "Label" ) );
	CHECK( labels->label( "description" ), QString( "Description" ) );
	CHECK( labels->label( "Unknown" ), QString::null );
}

void MetaInfoTest::allTests()
{
	labels();
}
