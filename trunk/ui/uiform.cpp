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
#include <qwidgetfactory.h>
#include <qobjectlist.h>
#include <qlayout.h>
#include <qwhatsthis.h>

#include <qlineedit.h>
#include <qtextedit.h>
#include <qcheckbox.h>
#include <qdatetimeedit.h>
#include <qslider.h>
#include <qscrollbar.h>
#include <qdial.h>
#include <qspinbox.h>
#include <qlabel.h>
#include <qprogressbar.h>
#include <qlcdnumber.h>
#include <qtable.h>
#include <qpopupmenu.h>
#include <kdatewidget.h>
#include <kcolorcombo.h>
#include <kcolorbutton.h>
#include <klocale.h>

#include "uiform.h"
#include "widgethandler.h"
#include "tablehandler.h"
#include "defaultpropertymetainfo.h"


QMap<QString,QString> UiForm::m_properties;
QMap<QString,WidgetHandlerFactory*> UiForm::m_relationHandlerFactories;
QMap<QString,WidgetHandlerFactory*> UiForm::m_collectionHandlerFactories;

UiForm::UiForm(QWidget *parent, const char *name)
 : QWidget(parent, name)
{
	m_widget = 0;
	QHBoxLayout *layout = new QHBoxLayout( this );
	layout->setAutoAdd( true );
	if ( m_properties.isEmpty() ) {
		m_properties[ "KColorButton" ] = "color";
		m_properties[ "KColorCombo" ] = "color";
		m_properties[ "QLineEdit" ] = "text";
		m_properties[ "QTextEdit" ] = "text";
		m_properties[ "QCheckBox" ] = "checked";
		m_properties[ "QDateEdit" ] = "date";
		m_properties[ "QTimeEdit" ] = "time";
		m_properties[ "QDateTimeEdit" ] = "dateTime";
		m_properties[ "QSlider" ] = "value";
		m_properties[ "QScrollBar" ] = "value";
		m_properties[ "QDial" ] = "value";
		m_properties[ "QSpinBox" ] = "value";
		m_properties[ "QLabel" ] = "text";
		m_properties[ "QProgressBar" ] = "value";
		m_properties[ "QLCDNumber" ] = "value";
		m_properties[ "KDateWidget" ] = "date";
	}
	if ( m_relationHandlerFactories.isEmpty() ) {
	}
	if ( m_collectionHandlerFactories.isEmpty() ) {
		m_collectionHandlerFactories[ "QTable" ] = new TableHandlerFactory();
	}
}

void UiForm::setUiFile( const QString & fileName )
{
	kdDebug() << k_funcinfo << endl;
	m_uiFileName = fileName;
	if ( m_widget ) {
		delete m_widget;
		m_widget = 0;
	}
	m_widget = QWidgetFactory::create( fileName, 0, this );
	fillForm();
}

const QString & UiForm::uiFile( ) const
{
	return m_uiFileName;
}

void UiForm::setObject( Object * object )
{
	kdDebug() << k_funcinfo << endl;
	m_object = object;
	fillForm();
}

Object * UiForm::object() const
{
	return m_object;
}

void UiForm::fillForm( )
{
	if ( ! m_widget || m_object.isNull() )
		return;

	if ( ! m_object.isNull() ) {
		PropertiesIterator it( m_object->propertiesBegin() );
		PropertiesIterator end( m_object->propertiesEnd() );
		Property p;
		for ( ; it != end; ++it ) {
			p = *it;
			kdDebug() << k_funcinfo << " P: " << p.name() << " V: " << p.value() << endl;
		}
		kdDebug() << k_funcinfo << "Oid: " << oidToString( m_object->oid() ) << endl;
	}

	QObjectList* widgets = m_widget->queryList();
	QObjectListIt it( *widgets );
	QWidget *w;
	QMetaObject *m;
	QVariant value;
	for ( ; w = static_cast<QWidget*>( it.current() ); ++it ) {
		m = w->metaObject();

		if ( existsProperty( QWhatsThis::textFor( w ) ) ) {

			value = m_object->property( QWhatsThis::textFor( w ) ).value();
			if ( m_object->property( QWhatsThis::textFor( w ) ).readOnly() )
				w->setEnabled( false );

			QMapConstIterator<QString,QString> it( m_properties.constBegin() );
			QMapConstIterator<QString,QString> end( m_properties.constEnd() );
			for ( ; it != end; ++it ) {
				if ( m->inherits( it.key() ) ) {
					w->setProperty( it.data(), value );
					break;
				}
			}
			if ( it == end ) {
				kdDebug() << k_funcinfo << "Widget type: '" << QString( m->className() ) << "', inherits '" << QString( m->superClassName() ) << "'" << endl;
			}
		} else if ( existsRelation( QWhatsThis::textFor( w ) ) ) {
			if ( m->inherits( "QComboBox" ) ) {
				Object *rel = relation( QWhatsThis::textFor( w ) );
				QComboBox *e = static_cast<QComboBox*>( w );
				e->clear();
				if ( rel ) {
					DefaultPropertyMetaInfo *defaultProperty = dynamic_cast<DefaultPropertyMetaInfo*>( rel->classInfo()->metaInfo( "defaultProperty" ) );

					Collection col( rel->classInfo()->name() );
					CollectionIterator it( col.begin() );
					CollectionIterator end( col.end() );
					Object *obj;
					int item;
					for ( ; it != end; ++it ) {
						obj = *it;
						if ( defaultProperty )
							e->insertItem( defaultProperty->defaultPropertyValue( obj ) );
						else
							e->insertItem( oidToString( obj->oid() ) );

						if ( obj->oid() == rel->oid() )
							item = e->count() - 1;
					}
					e->setCurrentItem( item );
				}
			}
		} else if ( existsCollection( QWhatsThis::textFor( w ) ) ) {
			QMapConstIterator<QString,WidgetHandlerFactory*> it( m_collectionHandlerFactories.constBegin() );
			QMapConstIterator<QString,WidgetHandlerFactory*> end( m_collectionHandlerFactories.constEnd() );
			for ( ; it != end; ++it ) {
				if ( m->inherits( it.key() ) ) {
					WidgetHandler* handler = it.data()->create( w );
					m_collectionHandlers[ w ] = handler;
					handler->setWidget( w );
					handler->setObject( m_object );
					handler->load();
					connect( handler, SIGNAL(destroyed(QObject*)), SLOT(handlerDestroyed(QObject*)) );
					break;
				}
			}
			if ( it == end ) {
				kdDebug() << k_funcinfo << "Widget type: '" << QString( m->className() ) << "', inherits '" << QString( m->superClassName() ) << "'" << endl;
			}
		}
	}
	delete widgets;
}

void UiForm::save( )
{
	if ( ! m_widget || m_object.isNull() )
		return;


	QObjectList* widgets = m_widget->queryList();
	QObjectListIt it( *widgets );
	QWidget *w;
	QMetaObject *m;
	for ( ; w = static_cast<QWidget*>( it.current() ); ++it ) {
		m = w->metaObject();

		if ( existsProperty( QWhatsThis::textFor( w ) ) ) {
			Property p = m_object->property( QWhatsThis::textFor( w ) );
			if ( m_object->property( QWhatsThis::textFor( w ) ).readOnly() )
				continue;

			QMapConstIterator<QString,QString> it( m_properties.constBegin() );
			QMapConstIterator<QString,QString> end( m_properties.constEnd() );
			for ( ; it != end; ++it ) {
				if ( m->inherits( it.key() ) ) {
					p.setValue( w->property( it.data() ) );
					break;
				}
			}
			if ( it == end ) {
				kdDebug() << k_funcinfo << "Widget type: '" << QString( m->className() ) << "', inherits '" << QString( m->superClassName() ) << "'" << endl;
			}
		} else if ( existsRelation( QWhatsThis::textFor( w ) ) ) {
			if ( m->inherits( "QComboBox" ) ) {
				Object *rel = relation( QWhatsThis::textFor( w ) );
				QComboBox *e = static_cast<QComboBox*>( w );
				e->clear();
				if ( rel ) {
					DefaultPropertyMetaInfo *defaultProperty = dynamic_cast<DefaultPropertyMetaInfo*>( rel->classInfo()->metaInfo( "defaultProperty" ) );

					Collection col( rel->classInfo()->name() );
					CollectionIterator it( col.begin() );
					CollectionIterator end( col.end() );
					Object *obj;
					int item;
					for ( ; it != end; ++it ) {
						obj = *it;
						if ( defaultProperty )
							e->insertItem( defaultProperty->defaultPropertyValue( obj ) );
						else
							e->insertItem( oidToString( obj->oid() ) );

						if ( obj->oid() == rel->oid() )
							item = e->count() - 1;
					}
					e->setCurrentItem( item );
				}
			}
		} else if ( existsCollection( QWhatsThis::textFor( w ) ) ) {
			if ( m_collectionHandlers.contains( w ) ) {
				WidgetHandler *handler = m_collectionHandlers[ w ];
				handler->save();
			} else {
				kdDebug() << k_funcinfo << "Widget type: '" << QString( m->className() ) << "', inherits '" << QString( m->superClassName() ) << "'" << endl;
			}
		}

	}
	delete widgets;
}

Property UiForm::property( const QString & path )
{
	if ( m_object.isNull() )
		return Property();

	if ( m_object->containsProperty( path ) )
		return m_object->property( path );

	// TODO: Analyze path

	return Property();
}

bool UiForm::existsProperty( const QString & path )
{
	if ( m_object.isNull() )
		return false;
	if ( m_object->containsProperty( path )	)
		return true;
	// TODO: Analyze path

	return false;
}

Object * UiForm::relation( const QString & path )
{
	if ( m_object.isNull() )
		return 0;
	if ( m_object->containsObject( path ) )
		return m_object->object( path );
	// TODO: Analyze path

	return 0;
}

bool UiForm::existsRelation( const QString & path )
{
	if ( m_object.isNull() )
		return false;
	if ( m_object->containsObject( path ) )
		return true;
	// TODO: Analyze path

	return false;
}

Collection * UiForm::collection( const QString & path )
{
	if ( m_object.isNull() )
		return 0;
	if ( m_object->containsCollection( path ) )
		return m_object->collection( path );
	// TODO: Analyze path

	return 0;
}

bool UiForm::existsCollection( const QString & path )
{
	if ( m_object.isNull() )
		return false;
	if ( m_object->containsCollection( path ) )
		return true;
	// TODO: Analyze path

	return false;
}

void UiForm::addRelationHandler( const QString & widgetClassName, WidgetHandlerFactory * factory )
{
	m_relationHandlerFactories[ widgetClassName ] = factory;
}

void UiForm::removeRelationHandler( const QString & widgetClassName )
{
	m_relationHandlerFactories.remove( widgetClassName );
}

void UiForm::addCollectionHandler( const QString & widgetClassName, WidgetHandlerFactory * factory )
{
	m_collectionHandlerFactories[ widgetClassName ] = factory;
}

void UiForm::removeCollectionHandler( const QString & widgetClassName )
{
	m_collectionHandlerFactories.remove( widgetClassName );
}

void UiForm::handlerDestroyed( QObject * object )
{
	QWidget *widget = static_cast<QWidget*>( object );
	if ( m_collectionHandlers.contains( widget ) ) {
		m_collectionHandlers.remove( widget );
	} else if ( m_relationHandlers.contains( widget ) ) {
		m_relationHandlers.remove( widget );
	}
}


/*
void UiForm::tablePopup( int row, int col, const QPoint & pos )
{
	if ( ! sender()->inherits( "QTable" ) )
		return;
	QPopupMenu *menu = new QPopupMenu( this );
	menu->insertItem( i18n( "&Add row" ), this, SLOT(tableAddRow()) );
	menu->insertItem( i18n( "&Remove row" ), this, SLOT(tableRemoveRow()) );
	menu->popup( pos );

	currentTable = (QTable*)( sender() );
}

void UiForm::tableAddRow( )
{
	currentTable->setNumRows( currentTable->numRows() + 1 );
}

void UiForm::tableRemoveRow( )
{
	currentTable->removeRow( currentTable->currentRow() );
}
*/

#include "uiform.moc"
