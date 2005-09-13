/***************************************************************************
 *   Copyright (C) 2004 by Albert Cervera Areny                            *
 *   albertca.com                                                   *
 *                                                                         *
 *   This program is free software{} you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation{} either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY{} without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
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
	{ "coutry", I18N_NOOP( "Coutry" ) }
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
	MODIFIED;
	m_code = code;
}

const QString& Customer::customerName() const
{
	return m_customerName;
}

void Customer::setCustomerName( const QString& name )
{
	MODIFIED;
	m_customerName = name;
}

const QString& Customer::address() const
{
	return m_address;
}

void Customer::setAddress( const QString& address )
{
	MODIFIED;
	m_address = address;
}

const QString& Customer::city() const
{
	return m_city;
}

void Customer::setCity( const QString& city )
{
	MODIFIED;
	m_city = city;
}

const QString& Customer::zipCode() const
{
	return m_zipCode;
}

void Customer::setZipCode( const QString& zipCode )
{
	MODIFIED;
	m_zipCode = zipCode;
}

const QString& Customer::country() const
{
	return m_country;
}

void Customer::setCountry( const QString& country )
{
	MODIFIED;
	m_country = country;
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

#include "customer.moc"
