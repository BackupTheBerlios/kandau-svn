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
#ifndef PROPERTYWIDGET_H
#define PROPERTYWIDGET_H

#include <qwidget.h>

#include <object.h>

/**
@author Albert Cervera Areny <albertca@hotpop.com>
*/
class PropertyWidget : public QWidget
{
	Q_OBJECT

public:
	PropertyWidget( QWidget *parent = 0 );
	PropertyWidget( const Property& property, QWidget *parent = 0 );

//	void setProperty( const QVariant& property );
//	QVariant property() const;

	void setProperty( const Property& property );

	void setReadOnly( bool readOnly );
	bool readOnly() const;

	virtual void setValue( const QVariant& value );
	virtual QVariant value() const;

protected:
	virtual QWidget* createWidget();
	QWidget *widget() const;

private:
	QVariant m_value;
	bool m_readOnly;
	QWidget *m_widget;
	QVBoxLayout *m_layout;
};

#endif
