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
#include <qdom.h>
#include <qfile.h>

#include <kdebug.h>

#include "xmldbbackend.h"
#include "classes.h"
#include "object.h"

XmlDbBackend::XmlDbBackend( const QString& fileName, bool truncate )
{
	m_currentOid = 1;
	m_fileName = fileName;
	if ( truncate )
		QFile::remove( fileName );
}

XmlDbBackend::~XmlDbBackend()
{
}

void XmlDbBackend::setup( Manager* manager )
{
	m_manager = manager;
	m_manager->setMaxObjects( Manager::Unlimited );
	init();
}

void XmlDbBackend::init()
{
	m_manager->reset();

	QFile file( m_fileName );
	QDomDocument doc;
	if ( ! file.open( IO_ReadOnly ) || ! doc.setContent( &file ) ) {
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

	// TODO: Ensure dboid is numeric?
	OidType oid = stringToOid( el.text() );
	assert( oid != 0 );
	ObjectRef<Object> object = Classes::classInfo( name )->create( oid, m_manager, true );

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
									if ( stringToOid( ec.text() ) != 0 ) {
										collection->simpleAdd( stringToOid( ec.text() ) );
									} else {
										kdDebug() << k_funcinfo << "Cannot add object with oid = 0" << endl;
									}
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

	PropertiesIterator it( object->propertiesBegin() );
	PropertiesIterator end( object->propertiesEnd() );
	for ( ; it != end; ++it ) {
		if ( (*it).readOnly() )
			continue;
		e = doc->createElement( (*it).name() );
		t = doc->createTextNode( (*it).value().toString() );
		e.appendChild( t );
		properties.appendChild( e );
	}

	ObjectsIterator oIt( object->objectsBegin() );
	ObjectsIterator oEnd( object->objectsEnd() );
	for ( ; oIt != oEnd; ++oIt ) {
		if (*oIt) {
			e = doc->createElement( oIt.key() );
			t = doc->createTextNode( oidToString( (*oIt)->oid() ) );
			e.appendChild( t );
			objects.appendChild( e );
		}
	}

	CollectionsIterator cIt( object->collectionsBegin() );
	CollectionsIterator cEnd( object->collectionsEnd() );
	for ( ; cIt != cEnd; ++cIt ) {
		Collection * tmp = (*cIt);
		assert( tmp );
		QDomElement collection = doc->createElement( tmp->collectionInfo()->name() );
		collections.appendChild( collection );

		CollectionIterator cIt2( tmp->begin() );
		CollectionIterator cEnd2( tmp->end() );
		for ( ; cIt2 != cEnd2; ++cIt2 ) {
			e = doc->createElement( "oid" );
			t = doc->createTextNode( oidToString( (*cIt2)->oid() ) );
			e.appendChild( t );
			collection.appendChild( e );
		}
	}
}

bool XmlDbBackend::load( const OidType& /*oid*/, Object* /*object*/ )
{
	kdDebug() << "Entering XmlDbBackend::load(): This should never happen!" << endl;
	return true;
}

bool XmlDbBackend::load( Collection* /*collection*/ )
{
	return true;
}

bool XmlDbBackend::createSchema()
{
	return true;
}

bool XmlDbBackend::hasChanged( Object* /*object*/ )
{
	return false;
}

OidType XmlDbBackend::newOid()
{
	return m_currentOid++;
}

bool XmlDbBackend::commit()
{
	QDomDocument doc( "Database" );
	QDomElement root = doc.createElement( "Database" );
	doc.appendChild( root );

	ManagerObjectIterator it( m_manager->begin() );
	ManagerObjectIterator end( m_manager->end() );
	for ( ; it != end; ++it )
		objectToElement( *it, &doc, &root );

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

void XmlDbBackend::afterRollback()
{
	init();
}

bool XmlDbBackend::load( Collection */*collection*/, const QString& /*query*/ )
{
	return true;
}
