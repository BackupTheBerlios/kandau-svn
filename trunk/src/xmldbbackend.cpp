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
#include <qdom.h>
#include <qfile.h>

#include <kdebug.h>

#include "xmldbbackend.h"
#include "classes.h"
#include "object.h"

XmlDbBackend::XmlDbBackend( const QString& fileName )
{
	m_currentOid = 1;

	m_fileName = fileName;
	//m_document = new QDomDocument( "Database" );
/*
	QFile file( m_fileName );
	if ( file.open( IO_ReadOnly ) && m_document->setContent( &file ) ) {
		kdDebug() << "File opened correctly" << endl;
		file.close();
		return;
	} else {
		kdDebug() << "File not found" << endl;
		file.close();
		QDomElement root = doc->createElement( "Database" );
		m_document->appendChild( root );
	}
*/
}

XmlDbBackend::~XmlDbBackend()
{
	//delete m_document;
}

/* Called at the Manager constructor */
void XmlDbBackend::setup()
{
	QFile file( m_fileName );
	QDomDocument doc;
	if ( file.open( IO_ReadOnly ) && doc.setContent( &file ) ) {
		kdDebug() << "File opened correctly" << endl;
	} else {
		kdDebug() << "File not found" << endl;
		doc.appendChild( doc.createElement( "Database" ) );
	}
	file.close();

	OidType maxOid, oid;
	maxOid = 0;

	QDomElement docElem = doc.documentElement();
	QDomNode n = docElem.firstChild();

	while( !n.isNull() ) {
		QDomElement e = n.toElement();
		if( !e.isNull() ) {
			oid = elementToObject( e );
			if ( oid > maxOid )
				maxOid = oid;
		}
		n = n.nextSibling();
	}

	m_currentOid = maxOid + 1;	// m_currentOid = max oid found
}

void XmlDbBackend::shutdown()
{
	commit();
}

OidType XmlDbBackend::elementToObject( const QDomElement& e )
{
	QString name;

	if ( ! Classes::contains( e.tagName() ) )
		return 0;

	name = e.tagName();

	QDomNode n = e.firstChild();
	if ( n.isNull() )
		return 0;

	// TODO: Allow dboid to be the second or third... element?
	QDomElement el = n.toElement();
	if ( el.isNull() || el.tagName().compare( "dboid" ) != 0 )
		return 0;

	OidType oid = stringToOid( el.text() );
	// TODO: Ensure dboid is numeric?
	ObjectRef<Object> object = Classes::classInfo( name )->create( oid );

	QDomNode nx;
	QDomElement ex;
	n = n.nextSibling();
	while( !n.isNull() ) {
		el = n.toElement();
		// TODO: Test if the type of the element corresponds to the type
		// of the property?
		if( !el.isNull() ) {
			if ( el.tagName() == "Properties" ) {
				nx = el.firstChild();
				while ( ! nx.isNull() ) {
					ex = nx.toElement();
					if ( ! ex.isNull() )
						object->property( ex.tagName() ).setValue( ex.text() );
					nx = nx.nextSibling();
				}
			} else if ( el.tagName() == "Objects" ) {
				nx = el.firstChild();
				while ( ! nx.isNull() ) {
					ex = nx.toElement();
					if ( ! ex.isNull() )
						object->setObject( ex.tagName(), stringToOid( ex.text() ) );
					nx = nx.nextSibling();
				}
			} else if ( el.tagName() == "Collections" ) {
				nx = el.firstChild();
				while ( ! nx.isNull() ) {
					ex = nx.toElement();
					if ( ! ex.isNull() ) {
						Collection *collection = object->collection( ex.tagName() );
						if ( collection ) {
							QDomNode nc = nx.firstChild();
							QDomElement ec;
							while ( !nc.isNull() ) {
								ec = nc.toElement();
								if ( ! ec.isNull() ) {
									collection->simpleAdd( stringToOid( ec.text() ) );
								}
								nc = nc.nextSibling();
							}
						}
					}
					nx = nx.nextSibling();
				}
			}
		}
		n = n.nextSibling();
	}
	object->setModified( false );
	return oid;
}

void XmlDbBackend::objectToElement( Object* object, QDomDocument *doc, QDomElement *parent )
{
	assert( object );
	kdDebug() << "XmlDbBackend::objectToElement()" << endl;
	QDomElement root = doc->createElement( object->className() );
	parent->appendChild( root );

	// We don't use the sequence in this backend
	QDomElement e = doc->createElement( "dboid" );
	QDomText t = doc->createTextNode( oidToString( object->oid() ) );
	e.appendChild( t );
	root.appendChild( e );

	QDomElement properties = doc->createElement( "Properties" );
	root.appendChild( properties );
	QDomElement objects = doc->createElement( "Objects" );
	root.appendChild( objects );
	QDomElement collections = doc->createElement( "Collections" );
	root.appendChild( collections );

	PropertyIterator it( object->propertiesBegin() );
	PropertyIterator end( object->propertiesEnd() );
	for ( ; it != end; ++it ) {
		e = doc->createElement( (*it).name() );
		t = doc->createTextNode( (*it).value().toString() );
		e.appendChild( t );
		properties.appendChild( e );
	}

	ObjectIterator oIt( object->objectsBegin() );
	ObjectIterator oEnd( object->objectsEnd() );
	for ( ; oIt != oEnd; ++oIt ) {
		if (*oIt) {
			e = doc->createElement( oIt.key() );
			t = doc->createTextNode( oidToString( (*oIt)->oid() ) );
			e.appendChild( t );
			objects.appendChild( e );
		}
	}


	CollectionIterator cIt( object->collectionsBegin() );
	CollectionIterator cEnd( object->collectionsEnd() );
	kdDebug() << "Iterating collections (" << object->numCollections() << ")" << endl;
	for ( ; cIt != cEnd; ++cIt ) {
		QDomElement collection = doc->createElement( (*cIt)->collectionInfo()->childrenClassInfo()->name() );
		collections.appendChild( collection );

		oIt = (*cIt)->begin();
		oEnd = (*cIt)->end();
		for ( ; oIt != oEnd; ++oIt ) {
			e = doc->createElement( "oid" );
			t = doc->createTextNode( oidToString( (*oIt)->oid() ) );
			e.appendChild( t );
			collection.appendChild( e );
		}
	}
}


/* Object management related functions */
bool XmlDbBackend::load( const OidType& /*oid*/, Object* /*object*/ )
{
	//assert( false );
	kdDebug() << "Entering XmlDbBackend::load(): This should never happen!" << endl;
	return true;
}

bool XmlDbBackend::save( Object* /*object*/ )
{
	return true;
}

bool XmlDbBackend::remove( Object* /*object*/ )
{
	return true;
}

/* Collection management related functions */
bool XmlDbBackend::load( Collection* /*collection*/ )
{
	return true;
}

bool XmlDbBackend::add( Collection* /*collection*/, Object* /*object*/ )
{
	return true;
}

bool XmlDbBackend::remove( Collection* /*collection*/, const OidType& /*object*/ )
{
	return true;
}

/* Database Schema related functions */
bool XmlDbBackend::createSchema()
{
	return true;
}

/*! Decides whether the object changed in the database since last load */
bool XmlDbBackend::hasChanged( Object* /*object*/ )
{
	return false;
}

/*!
This function must provide a new unique Oid. Used for newly created
objects.
*/
OidType XmlDbBackend::newOid()
{
	return m_currentOid++;
}


void XmlDbBackend::setRelation( const OidType& /*oid*/, const QString& /*relation*/, const OidType& /*oidRelated*/, const OidType& /*oldOid*/ )
{
}

/*!
Starts a transaction
*/
bool XmlDbBackend::start()
{
	return true;
}

/*!
Commits the current transaction
*/
bool XmlDbBackend::commit()
{
	QDomDocument doc( "Database" );
	QDomElement root = doc.createElement( "Database" );
	doc.appendChild( root );

	ManagerObjectIterator it( Manager::self()->begin() );
	ManagerObjectIterator end( Manager::self()->end() );
	kdDebug() << "Starting manager loop(" << Manager::self()->count() << " objects)" << endl;
	for ( ; it != end; ++it ) {
		//root.appendChild( objectToElement( *it, &doc, &root ) );
		objectToElement( *it, &doc, &root );
		//kdDebug() << "XmlDbBackend::objectToElement() ==> count = " << QString::number( root.childNodes().count() ) << endl;

	}
	//doc.appendChild( root );
	QFile file( m_fileName );
	if ( ! file.open( IO_WriteOnly ) ) {
		kdDebug() << "Error opening file for writing." << endl;
		return false;
	}
	QTextStream ts( &file );
	doc.documentElement().save( ts, 2 );
	file.close();

	return true;
}

/*!
Aborts the current transaction
*/
bool XmlDbBackend::rollback()
{
	setup();
	return true;
}

