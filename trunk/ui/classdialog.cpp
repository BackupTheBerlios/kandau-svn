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
#include <qwidget.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qlayout.h>

#include <kdialog.h>
#include <kdialogbase.h>
#include <knuminput.h>
#include <klineedit.h>
#include <ktimewidget.h>
#include <kdatewidget.h>
#include <kdatetimewidget.h>
#include <klocale.h>
#include <kurllabel.h>
#include <kmessagebox.h>

#include <object.h>
#include <labelsmetainfo.h>
#include <defaultpropertymetainfo.h>
#include <notifier.h>

#include "classdialog.h"
#include "chooseobjectdialog.h"
#include "propertywidget.h"

/**
 *
 * @param object
 * @param parent
 * @return
 */
ClassDialog::ClassDialog( Object *object, QWidget *parent) :
	KDialogBase(parent)
{
	m_object = object;
	setModal( false );
	setCaption( object->classInfo()->name() + "(" + oidToString(object->oid()) + ")" );
	showButtonOK( true );
	showButtonCancel( true );
	showButtonApply( false );

	QWidget *widget = new QWidget( this );
	setMainWidget( widget );
	connect( this, SIGNAL(okClicked()), SLOT(slotOkClicked()) );

	QVBoxLayout *layout = new QVBoxLayout( widget );

	LabelsMetaInfo *labels = dynamic_cast<LabelsMetaInfo*>( object->classInfo()->metaInfo( "labels" ) );

	QLabel *label;
	label = new QLabel( widget );
	label->setAlignment( Qt::AlignCenter );
	if ( labels )
		label->setText( labels->label( object->classInfo()->name() ) );
	else
		label->setText( object->classInfo()->name() );
	layout->addWidget( label );

	QGridLayout *gridLayout = new QGridLayout( layout, object->numProperties() + object->numObjects() + object->numCollections(), 2, 5 );

	int row = 0;
	PropertiesIterator it( object->propertiesBegin() );
	PropertiesIterator end( object->propertiesEnd() );
	for ( ; it != end; ++it, ++row ) {
		Property p = *it;
		label = new QLabel( widget );
		if ( labels )
			label->setText( labels->label( p.name() ) );
		else
			label->setText( p.name() );
		gridLayout->addWidget( label, row, 0 );
		//QWidget *tmp = createInput( widget, p );
		PropertyWidget *tmp = new PropertyWidget( p, widget );
		m_mapProperties.insert( p.name(), tmp );
		gridLayout->addWidget( tmp, row, 1 );
	}

	const ClassInfo *classInfo = object->classInfo();
	RelationInfosConstIterator it2( classInfo->relationsBegin() );
	RelationInfosConstIterator end2( classInfo->relationsEnd() );
	for ( ; it2 != end2; ++it2, ++row ) {
		RelationInfo *relObj = it2.data();
		label = new QLabel( widget );
		if ( labels )
			label->setText( labels->label( it2.data()->name() ) );
		else
			label->setText( it2.data()->name() );
		gridLayout->addWidget( label, row, 0 );

		QHBoxLayout *lay = new QHBoxLayout();
		gridLayout->addLayout( lay, row, 1 );
		KURLLabel *objLabel = new KURLLabel(widget);
		objLabel->setAlignment( Qt::AlignCenter );
		Object* obj = object->object( relObj->name() );
		updateObjectLabel( objLabel, obj );
		connect( objLabel, SIGNAL(leftClickedURL(const QString&)), SLOT(slotObjectSelected(const QString&)) );
		m_mapObjects.insert( relObj->name(), objLabel );
		QPushButton *but = new QPushButton( widget );
		but->setText( i18n( "Change" ) );
		connect( but, SIGNAL(clicked()), SLOT(slotChangeClicked()) );
		m_mapChangeButtons.insert( but, relObj );
		lay->addWidget( objLabel );
		lay->addWidget( but );

		Notifier *notifier = dynamic_cast<Notifier*>( Manager::self()->notificationHandler() );
		if ( notifier && obj )
			notifier->registerSlot( this, SLOT( slotObjectModified(const ClassInfo*,const OidType&,const PropertyInfo*,const QVariant&) ), 0, obj->oid() );
	}
}

Object* ClassDialog::object() const
{
	return m_object;
}
/*
QWidget* ClassDialog::createInput( QWidget* parent, const Property& property )
{
	QWidget *widget;
	switch ( property.type() ) {
		case QVariant::CString:
		case QVariant::String: {
			KLineEdit *line = new KLineEdit( parent );
			line->setText( property.value().toString() );
			widget = line;
			break;
		}
		case QVariant::LongLong:
		case QVariant::ULongLong:
		case QVariant::Int:
		case QVariant::UInt: {
			KIntNumInput *input = new KIntNumInput( parent );
			input->setValue( property.value().toLongLong() );
			widget = input;
			break;
		}
		case QVariant::Date: {
			KDateWidget *date = new KDateWidget( parent );
			date->setDate( property.value().toDate() );
			widget = date;
			break;
		}
		case QVariant::Time: {
			KTimeWidget *time = new KTimeWidget( parent );
			time->setTime( property.value().toTime() );
			widget = time;
			break;
		}
		case QVariant::DateTime: {
			KDateTimeWidget *dateTime = new KDateTimeWidget( parent );
			dateTime->setDateTime( property.value().toDateTime() );
			widget = dateTime;
			break;
		}
		case QVariant::Double: {
			KDoubleNumInput *input = new KDoubleNumInput( parent );
			input->setValue( property.value().toDouble() );
			break;
		}
		default: {
			break;
		}
	}
	widget->setEnabled( ! property.readOnly() );
	return widget;
}
*/

void ClassDialog::updateObjectLabel( KURLLabel *objLabel, const Object *obj )
{
	if ( obj != 0 ) {
		DefaultPropertyMetaInfo *defaultProperty = dynamic_cast<DefaultPropertyMetaInfo*>( obj->classInfo()->metaInfo( "defaultProperty" ) );
		if ( defaultProperty )
			objLabel->setText( defaultProperty->defaultPropertyValue( obj ) );
		else
			objLabel->setText( oidToString( obj->oid() ) );
		objLabel->setURL( oidToString( obj->oid() ) );
		objLabel->setEnabled( true );
	} else {
		objLabel->setText( i18n( "(not assigned)" ) );
		objLabel->setEnabled( false );
	}
}

/*
QVariant ClassDialog::readInput( QWidget* widget )
{
	QString className = widget->className();
	if ( className == "KLineEdit" ) {
		return ( static_cast<KLineEdit*>( widget ) )->text();
	} else if ( className == "KIntNumInput" ) {
		return ( static_cast<KIntNumInput*>( widget ) )->value();
	} else if ( className == "KTimeWidget" ) {
		return ( static_cast<KTimeWidget*>( widget ) )->time();
	} else if ( className == "KDoubleNumInput" ) {
		return ( static_cast<KDoubleNumInput*>( widget ) )->value();
	} else if ( className == "KDateWidget" ) {
		return ( static_cast<KDateWidget*>( widget ) )->date();
	} else if ( className == "KDateTimeWidget" ) {
		return ( static_cast<KDateTimeWidget*>( widget ) )->dateTime();
	} else if ( className == "" ) {
		return QVariant();
	} else if ( className == "" ) {
		return QVariant();
	} else {
		return QVariant();
	}
}
*/

void ClassDialog::slotObjectSelected( const QString& oid )
{
	ObjectsIterator it( m_object->objectsBegin() );
	ObjectsIterator end( m_object->objectsEnd() );
	for ( ; it != end; ++it ) {

		if ( it.data() != 0 ) {
			kdDebug() << k_funcinfo << " Oid: " << (*it)->oid() << endl;
			if ( (*it)->oid() == stringToOid(oid) ) {
				kdDebug() << k_funcinfo << " Object oid. " << oid << endl;
				emit objectSelected( it.data() );
				return;
			}
		}
	}
	kdDebug() << k_funcinfo << "Object " << oid << " not found" << endl;
}

void ClassDialog::slotOkClicked()
{
	QMapConstIterator<QString,PropertyWidget*> it( m_mapProperties.constBegin() );
	QMapConstIterator<QString,PropertyWidget*> end( m_mapProperties.constEnd() );
	for ( ; it!=end; ++it ) {
		if ( m_object->containsProperty( it.key() ) ) {
			m_object->property( it.key() ).setValue( it.data()->property() );
		}
	}
}

void ClassDialog::slotChangeClicked()
{
	const QWidget *w = dynamic_cast<const QWidget*>( sender() );
	if ( w ) {
		RelationInfo *rel = m_mapChangeButtons[ w ];
		Collection col( rel->relatedClassInfo()->name() );
		ChooseObjectDialog *c = new ChooseObjectDialog( &col, 0, this );
		if ( c->exec() == QDialog::Accepted ) {
			m_object->setObject( rel->name(), c->selectedObject() );
			updateObjectLabel( m_mapObjects[ rel->name() ], c->selectedObject() );
		}
		delete c;
	}
}

void ClassDialog::slotObjectModified( const ClassInfo* /*classInfo*/, const OidType& object, const PropertyInfo */*property*/, const QVariant& /*newValue*/ )
{
//	KMessageBox::information( this, "Class: " + classInfo->name() + ", Oid: " + oidToString( object ) + ", Property: " + property->name() + ", NewValue: " + newValue.toString(), "Object Modified" );
	ObjectsIterator it( m_object->objectsBegin() );
	ObjectsIterator end( m_object->objectsEnd() );
	for ( ; it != end; ++it ) {
		Object *obj = (*it);
		if ( obj && obj->oid() == object )
			updateObjectLabel( m_mapObjects[ it.relatedObject()->name() ], obj );
	}
}

#include "classdialog.moc"
