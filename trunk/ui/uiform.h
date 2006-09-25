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
#ifndef UIFORM_H
#define UIFORM_H

#include <qwidget.h>

#include <object.h>

namespace Kandau {
	namespace Ui {

		class WidgetHandler;
		class WidgetHandlerFactory;
		
		class UiForm : public QWidget
		{
			Q_OBJECT
		public:
			UiForm(QWidget *parent = 0, const char *name = 0);
		
			void setUiFile( const QString& fileName );
			const QString& uiFile() const;
		
			void setObject( Object* object );
			Object* object() const;
		
			Property property( const QString& path );
			bool existsProperty( const QString& path );
		
			Object* relation( const QString& path );
			bool existsRelation( const QString& path );
		
			Collection* collection( const QString& path );
			bool existsCollection( const QString& path );
		
			static void addRelationHandler( const QString& widgetClassName, WidgetHandlerFactory* factory );
			static void removeRelationHandler( const QString& widgetClassName );
		
			static void addCollectionHandler( const QString& widgetClassName, WidgetHandlerFactory* factory );
			static void removeCollectionHandler( const QString& widgetClassName );
		
		public slots:
			void save();
		
		protected:
			void fillForm();
		
		protected slots:
			void handlerDestroyed( QObject* object );
		
		private:
			QString m_uiFileName;
			ObjectRef<Object> m_object;
			QWidget *m_widget;
			//static QMap<QString,QString> m_properties;
			static QMap<QString,WidgetHandlerFactory*> m_propertyHandlerFactories;
			static QMap<QString,WidgetHandlerFactory*> m_relationHandlerFactories;
			static QMap<QString,WidgetHandlerFactory*> m_collectionHandlerFactories;
			QMap<QWidget*,WidgetHandler*> m_propertyHandlers;
			QMap<QWidget*,WidgetHandler*> m_relationHandlers;
			QMap<QWidget*,WidgetHandler*> m_collectionHandlers;
		};
	
	}
}

#endif
