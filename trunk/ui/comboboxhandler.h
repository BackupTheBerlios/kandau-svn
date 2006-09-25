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
#ifndef COMBOBOXHANDLER_H
#define COMBOBOXHANDLER_H

#include <widgethandler.h>

class QComboBox;

namespace Kandau {
	namespace Ui {

		class ComboBoxHandler : public WidgetHandler
		{
			Q_OBJECT
		public:
			ComboBoxHandler( QObject* parent = 0, const char* name = 0 );
		
			QComboBox* combo() const;
		
			void load();
			void save();
		private:
			QMap<int,OidType> m_oids;
		};
		
		class ComboBoxHandlerFactory : public WidgetHandlerFactory
		{
		public:
			ComboBoxHandler *create( QWidget* widget ) const;
		};

	}
}

#endif
