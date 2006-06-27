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
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include <qlayout.h>

#include <knuminput.h>
#include <klineedit.h>
#include <ktimewidget.h>
#include <kdatewidget.h>
#include <kdatetimewidget.h>
#include <kurlrequester.h>
#include <kio/netaccess.h>
#include <ktempfile.h>

#include "propertywidget.h"

PropertyWidget::PropertyWidget( QWidget *parent ) : QWidget( parent )
{
	m_widget = 0;
	m_layout = new QVBoxLayout( this );
	setBackgroundOrigin( ParentOrigin );
	setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
}

PropertyWidget::PropertyWidget( const Property& property, QWidget *parent ) : QWidget( parent )
{
	m_widget = 0;
	m_layout = new QVBoxLayout( this );
	setBackgroundOrigin( ParentOrigin );
	setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
	setProperty( property );
}

/*QVariant PropertyWidget::property() const
{
//	Property p = m_property;
//	p.setValue( value() );
	return m_value;
}
*/
void PropertyWidget::setProperty( const Property& property )
{
	if ( m_value.type() != property.type() ) {
		delete m_widget;
		m_widget = 0;
	}
	m_value = property.value();
	m_readOnly = property.readOnly();
	if ( ! m_widget ) {
		m_widget = createWidget();
		m_widget->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
		m_layout->addWidget( m_widget );
	} else
		setValue( property.value() );
}

void PropertyWidget::setReadOnly( bool readOnly )
{
	m_readOnly = readOnly;
}

bool PropertyWidget::readOnly() const
{
	return m_readOnly;
}

QWidget* PropertyWidget::createWidget()
{
	QWidget *widget;
	switch ( m_value.type() ) {
		case QVariant::CString:
		case QVariant::String: {
			KLineEdit *line = new KLineEdit( this );
			line->setText( m_value.toString() );
			widget = line;
			break;
		}
		case QVariant::LongLong:
		case QVariant::ULongLong:
		case QVariant::Int:
		case QVariant::UInt: {
			KIntNumInput *input = new KIntNumInput( this );
			input->setValue( m_value.toLongLong() );
			widget = input;
			break;
		}
		case QVariant::Date: {
			KDateWidget *date = new KDateWidget( this );
			date->setDate( m_value.toDate() );
			widget = date;
			break;
		}
		case QVariant::Time: {
			KTimeWidget *time = new KTimeWidget( this );
			time->setTime( m_value.toTime() );
			widget = time;
			break;
		}
		case QVariant::DateTime: {
			KDateTimeWidget *dateTime = new KDateTimeWidget( this );
			dateTime->setDateTime( m_value.toDateTime() );
			widget = dateTime;
			break;
		}
		case QVariant::Double: {
			KDoubleNumInput *input = new KDoubleNumInput( this );
			input->setValue( m_value.toDouble() );
			widget = input;
			break;
		}
		case QVariant::ByteArray: {
			kdDebug() << k_funcinfo << "ByteArray" << endl;
			KURLRequester *input = new KURLRequester( this );
			KTempFile tmpFile( "/tmp/kandauui", ".data" );
			QFile *file = tmpFile.file();
			file->writeBlock( m_value.toByteArray().data(), m_value.toByteArray().size() );
			input->setURL( tmpFile.name() );
			widget = input;
			break;
		}
		default: {
			kdDebug() << k_funcinfo << " Can't handle QVariant type: " << m_value.typeName() << endl;
			assert( false );
			break;
		}
	}
	widget->setEnabled( ! m_readOnly );
	return widget;
}

void PropertyWidget::setValue( const QVariant& value )
{
	switch ( m_value.type() ) {
		case QVariant::CString:
		case QVariant::String: {
			KLineEdit *line = static_cast<KLineEdit*>( m_widget );
			line->setText( value.toString() );
			break;
		}
		case QVariant::LongLong:
		case QVariant::ULongLong:
		case QVariant::Int:
		case QVariant::UInt: {
			KIntNumInput *input = static_cast<KIntNumInput*>( m_widget );
			input->setValue( value.toLongLong() );
			break;
		}
		case QVariant::Date: {
			KDateWidget *date = static_cast<KDateWidget*>( m_widget );
			date->setDate( value.toDate() );
			break;
		}
		case QVariant::Time: {
			KTimeWidget *time = static_cast<KTimeWidget*>( m_widget );
			time->setTime( value.toTime() );
			break;
		}
		case QVariant::DateTime: {
			KDateTimeWidget *dateTime = static_cast<KDateTimeWidget*>( m_widget );
			dateTime->setDateTime( value.toDateTime() );
			break;
		}
		case QVariant::Double: {
			KDoubleNumInput *input = static_cast<KDoubleNumInput*>( m_widget );
			input->setValue( value.toDouble() );
			break;
		}
		case QVariant::ByteArray: {
			KURLRequester *input = static_cast<KURLRequester*>( m_widget );

			KTempFile tmpFile( "/tmp/kandauui", ".data" );
			QFile *file = tmpFile.file();
			file->writeBlock( m_value.toByteArray().data(), m_value.toByteArray().size() );
			input->setURL( tmpFile.name() );
		}
		default: {
			break;
		}
		default:
			break;
	}
}

QVariant PropertyWidget::value() const
{
	QString className = m_widget->className();
	if ( className == "KLineEdit" ) {
		return ( static_cast<KLineEdit*>( m_widget ) )->text();
	} else if ( className == "KIntNumInput" ) {
		return ( static_cast<KIntNumInput*>( m_widget ) )->value();
	} else if ( className == "KTimeWidget" ) {
		return ( static_cast<KTimeWidget*>( m_widget ) )->time();
	} else if ( className == "KDoubleNumInput" ) {
		return ( static_cast<KDoubleNumInput*>( m_widget ) )->value();
	} else if ( className == "KDateWidget" ) {
		return ( static_cast<KDateWidget*>( m_widget ) )->date();
	} else if ( className == "KDateTimeWidget" ) {
		return ( static_cast<KDateTimeWidget*>( m_widget ) )->dateTime();
       } else if ( className == "KURLRequester" ) {
		KURL url = ( static_cast<KURLRequester*>( m_widget ) )->url();
		QString fileName;
		if ( url.isLocalFile() ) {
			fileName = url.path();
		} else {
			QString tmpFile;
			if ( ! KIO::NetAccess::download( url, tmpFile, 0 ) )
				return QByteArray();
			fileName = tmpFile;
		}
		QFile file( fileName );
		file.open( IO_ReadOnly );
		QByteArray data = file.readAll();
		file.close();
		if ( ! url.isLocalFile() ) {
			KIO::NetAccess::removeTempFile( fileName );
		}
		return data;
	} else {
		return QVariant();
	}
}

QWidget* PropertyWidget::widget() const
{
	return m_widget;
}

#include "propertywidget.moc"
