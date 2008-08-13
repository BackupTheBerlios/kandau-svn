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
#ifndef TABLEHANDLER_H
#define TABLEHANDLER_H

#include <qobject.h>

#include "widgethandler.h"

namespace Kandau {
	namespace Ui {

		class TableHandler : public WidgetHandler
		{
			Q_OBJECT
		public:
			TableHandler( QObject* parent = 0, const char* name = 0 );
		
			QTable* table() const;
		
			void load();
			void save();
		
		protected slots:
			void tablePopup( int row, int col, const QPoint& pos );
			void tableAddRow();
			void tableRemoveRow();
		private:
			QValueVector<OidType> m_removed;
		};
		
		class TableHandlerFactory : public WidgetHandlerFactory
		{
		public:
			TableHandler *create( QWidget* widget ) const;
		};

	}
}
#endif
