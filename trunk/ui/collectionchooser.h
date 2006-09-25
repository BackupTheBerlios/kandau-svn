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
#ifndef COLLECTIONCHOOSER_H
#define COLLECTIONCHOOSER_H

#include <qwidget.h>

#include <object.h>

class KPushButton;

namespace Kandau {
	namespace Ui {

		class CollectionListView;

		class CollectionChooser : public QWidget
		{
			Q_OBJECT
		public:
			CollectionChooser(QWidget *parent = 0, const char *name = 0);
		
			void setCollection( Collection* collection );
			Collection* collection() const;
			void setObject( Object* object );
			Object* object() const;
			void setCollectionName( const QString& collectionName );
			const QString& collectionName() const;
		
		protected slots:
			void addClicked();
			void addAllClicked();
			void removeClicked();
			void removeAllClicked();
			void droppedAvailable( QDropEvent* event );
			void droppedChoosen( QDropEvent* event );
		
		private:
			CollectionRef<Collection> m_collection;
			ObjectRef<Object> m_object;
			QString m_collectionName;
			CollectionListView *m_available;
			CollectionListView *m_choosen;
			KPushButton *m_add;
			KPushButton *m_addAll;
			KPushButton *m_remove;
			KPushButton *m_removeAll;
		};

	}
}

#endif
