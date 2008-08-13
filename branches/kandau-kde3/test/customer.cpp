/***************************************************************************
 *   Copyright (C) 2004 by Albert Cervera Areny                            *
 *   albertca.com                                                   *
 *                                                                         *
 *   This program is free software{} you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as published by  *
 *   the Free Software Foundation{} either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY{} without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public License     *
 *   along with this program{} if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include <klocale.h>

#include <labelsmetainfo.h>
#include <defaultpropertymetainfo.h>

#include "customer.h"
#include "article.h"
#include "customerorder.h"

ICLASS( Customer );

static const LabelDescription customerLabels[] = {
	{ "Customer", I18N_NOOP( "Customer" ) },
	{ "code", I18N_NOOP( "Code" ) },
	{ "customerName", I18N_NOOP( "Name" ) },
	{ "address", I18N_NOOP( "Address" ) },
	{ "city", I18N_NOOP( "City" ) },
	{ "zipCode", I18N_NOOP( "Zip Code" ) },
	{ "country", I18N_NOOP( "Coutry" ) },
	{ "one_to_one", I18N_NOOP( "One to one" ) },
	{ "default_article", I18N_NOOP( "Default Article" ) },
	{ "second_default_article", I18N_NOOP( "Second Default Article" ) },
	{ "discounted_articles", I18N_NOOP( "Discounted Articles" ) },
	{ "adapted_articles", I18N_NOOP( "Adapted articles" ) },
	LabelDescriptionLast
};

void Customer::createRelations()
{
	OBJECTR( CustomerOrder, "one_to_one" );
	OBJECTR( Article, "default_article" );
	OBJECTR( Article, "second_default_article" );
	COLLECTIONR( Article, "discounted_articles" );
	COLLECTIONR( Article, "adapted_articles" );
	COLLECTION( CustomerOrder );
	ADDMETAINFO( "labels", new LabelsMetaInfo( customerLabels ) );
	ADDMETAINFO( "defaultProperty", new DefaultPropertyMetaInfo( "{customerName} ({code})" ) );
}

const QString& Customer::code() const
{
	return m_code;
}

void Customer::setCode( const QString& code )
{
	m_code = code;
	MODIFIED( code );
}

const QString& Customer::customerName() const
{
	return m_customerName;
}

void Customer::setCustomerName( const QString& name )
{
	m_customerName = name;
	MODIFIED( name );
}

const QString& Customer::address() const
{
	return m_address;
}

void Customer::setAddress( const QString& address )
{
	m_address = address;
	MODIFIED( address );
}

const QString& Customer::city() const
{
	return m_city;
}

void Customer::setCity( const QString& city )
{
	m_city = city;
	MODIFIED( city );
}

const QString& Customer::zipCode() const
{
	return m_zipCode;
}

void Customer::setZipCode( const QString& zipCode )
{
	m_zipCode = zipCode;
	MODIFIED( zipCode );
}

const QString& Customer::country() const
{
	return m_country;
}

void Customer::setCountry( const QString& country )
{
	m_country = country;
	MODIFIED( country );
}

Article* Customer::defaultArticle() const
{
	return GETOBJECTR( Article, "default_article" );
}

void Customer::setDefaultArticle( Article *article )
{
	SETOBJECTR( Article, "default_article", article );
}

Article* Customer::secondDefaultArticle() const
{
	return GETOBJECTR( Article, "second_default_article" );
}

void Customer::setSecondDefaultArticle( Article *article )
{
	SETOBJECTR( Article, "second_default_article", article );
}

Collection* Customer::discountedArticles()
{
	return GETCOLLECTION( discounted_articles );
}

Collection* Customer::adaptedArticles()
{
	return GETCOLLECTION( adapted_articles );
}

int Customer::type() const
{
	return m_type;
}

void Customer::setType( int type )
{
	MODIFIED;
	m_type = type;
}

#include "customer.moc"
