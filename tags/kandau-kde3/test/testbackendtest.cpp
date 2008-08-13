/**
 * TestBackendTest.cpp
 *
 * Copyright (C)  2004  Zack Rusin <zack@kde.org>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <qsqldatabase.h>

#include <kdebug.h>

#include <testbackend.h>
#include <sqldbbackend.h>
#include <xmldbbackend.h>
#include <object.h>

#include "testbackendtest.h"

#include "article.h"
#include "customer.h"
#include "customerorder.h"

TestBackendTest::TestBackendTest()
    : Tester()
{
}

/*
Requeriments bàsics de l'arquitectura:
	- Ha de ser independent de la forma d'emmagatzemament, típicament hem de tenir dos formats per la base de dades: Un servidor SQL i XML.
	- Ha de permetre la creació automàtica de l'esquema de la base de dades. Pot ser un DTD o un Schema en el cas de XML i les taules en el cas de SQL.
	- Ha de ser simple d'utilitzar i fàcil (com a mínim mecànica) la construcció de les classes persistents, assistint amb macros el que s'hagi de fer.
	- Ha de permetre la navegabilitat manual. (Poder saltar d'un objecte a un altre, coneixent el tipus de dades o la funció que ens porta a l'altre)
	- Ha de permetre la navegabilitat automàtica. Poder fer referència a les classes mitjançant strings (primer pas de la navegabilitat) i iterar per totes les propietats i altres objectes que contingui un objecte sense coneixement previ de la seva existència.
	- Les funcions en general i en particular les de navegació han de ser coherents i el més senzilles possibles.
	- Un objecte ha de poder referenciar zero, un o varis objectes i només hem de tenir una instància de cada un d'ells en memòria.
	- Seria interessant suportar i controlar la cardinalitat. D'aquesta forma, no permetem afegir més objectes dels que pot tenir com a màxim una classe ni menys del mínim (i semblava tonto!). Per exemple, podem requerir que una línia de comanda hagi de tenir exactament UN article relacionat.
*/

/*
Desitjos que difícilment es compliran:
	- Suportar control de dades introduïdes a l'estil CHECK d'SQL i al mateix temps permetre la generació automàtica de funcions get/(set).
	- En el cas del suport SQL, permetre accedir a les funcions de les classes al propi gestor. Mirar d'aprendre com incorporar C++ al PostgreSQL i veure si és factible.
	- Permetre veure les diferències entre l'estructura de la base de dades actual i el que requeriria el codi actual i facilitar l'adaptació de les dades.
*/


void TestBackendTest::testBrowsing()
{
}


void TestBackendTest::browseObjectProperties()
{
	kdDebug() << "TestBackendTest::browseObjectProperties()" << endl;

	Article *article = Article::create();

	// Test begin, end, ++it, .data()
	PropertiesIterator it( article->propertiesBegin() );
	PropertiesIterator end( article->propertiesEnd() );
	for ( ; it != end; ++it ) {
//		kdDebug() << "Property: name = " << it.data().name() << ", value = " << it.data().value().toString() << ", type = " << QVariant::typeToName( it.data().type() ) << endl;
		if ( it.data().readOnly() )
			continue;
		it.data().setValue( it.data().name() );
	}

	// Test operator=, it++, operator*
	it = article->propertiesBegin();
	for ( ; it != end; it++ ) {
//		kdDebug() << "Property: name = " << (*it).name() << ", value = " << (*it).value().toString() << ", type = " << QVariant::typeToName( (*it).type() ) << endl;
		CHECK( (*it).value().toString(), (*it).name() );
	}
	kdDebug() << endl;
}

void TestBackendTest::browseObjectObjects()
{
//	kdDebug() << "TestBackendTest::browseObjectObjects()" << endl;

	Article *article = Article::create();

	ObjectsIterator it( article->objectsBegin() );
	ObjectsIterator end( article->objectsEnd() );
	for ( ; it != end; ++it ) {
		if ( it.data() )
			kdDebug() << "Object: className = " << it.data()->className() << ", oid = " << it.data()->oid() << endl;
		else
			kdDebug() << "Object: className = " << it.key() << ", oid = NULL" << endl;
	}

	it = article->objectsBegin();
	for ( ; it != end; it++ ) {
		if ( *it )
			kdDebug() << "Object: className = " << (*it)->className() << ", oid = " << (*it)->oid() << endl;
		else
			kdDebug() << "Object: className = " << it.key() << ", oid = NULL" << endl;
	}
	//article->save();
	kdDebug() << endl;
}

void TestBackendTest::browseObjectCollections()
{
//	kdDebug() << "TestBackendTest::browseObjectCollections()" << endl;
	Article *article = Article::create();
/*
	Article *a = Article::create();
	Article *b = Article::create();
	Article *c = Article::create();
*/
/*
	article->articles()->add( a );
	article->articles()->add( b );
	article->articles()->add( c );
*/
	CollectionsIterator it( article->collectionsBegin() );
	CollectionsIterator end( article->collectionsEnd() );
	for ( ; it != end; ++it ) {
		kdDebug() << "Collection: count = " << it.data()->count() << endl;
	}

	it = article->collectionsBegin();
	for ( ; it != end; it++ ) {
		kdDebug() << "Collection: count = " << (*it)->count() << endl;
	}
	//article->save();
	kdDebug() << endl;
}

void TestBackendTest::browseCollectionObjects()
{
//	kdDebug() << "TestBackendTest::browseCollectionObjects()" << endl;
/*
	Article *article = Article::create();
	Article *a = Article::create();
	Article *b = Article::create();
	Article *c = Article::create();

	article->articles()->add( a );
	article->articles()->add( b );
	article->articles()->add( c );
	article->articles()->remove( a );

	ObjectIterator it( article->articles()->begin() );
	ObjectIterator end( article->articles()->end() );
	for ( ; it != end; ++it ) {
		kdDebug() << "Object: className = " << it.data()->className() << ", oid = " << it.data()->oid() << endl;
	}

	it = article->articles()->begin();
	for ( ; it != end; ++it ) {
		kdDebug() << "Object: className = " << (*it)->className() << ", oid = " << (*it)->oid() << endl;
	}
*/
//	kdDebug() << endl;
}

void TestBackendTest::setObjectObjects()
{
//	kdDebug() << "TestBackendTest::setObjectObjects()" << endl;

	Article *article = Article::create();
	Article *a = Article::create();

	a->setCode( "1" );
	CHECK( a->code(), QString( "1" ) );
	a->setLabel( "Article A" );
	CHECK( a->label(), QString( "Article A" ) );
	a->setDescription( "Aquest és l'article A" );
	CHECK( a->description(), QString( "Aquest és l'article A" ) );
	article->setArticle( a );
	CHECK( article->article()->code(), a->code() );
	CHECK( article->article()->label(), a->label() );
	CHECK( article->article()->description(), a->description() );

//	kdDebug() << endl;
}

void TestBackendTest::modified()
{
	kdDebug() << k_funcinfo << endl;

	QVariant value;

	ClassInfoIterator cit( Classes::begin() );
	ClassInfoIterator cend( Classes::end() );
	for ( ; cit != cend; ++cit ) {
		Object *object = (*cit)->create();

		PropertiesIterator pit( object->propertiesBegin() );
		PropertiesIterator pend( object->propertiesEnd() );
		for ( ; pit != pend; ++pit ) {
			value = (*pit).value();
			CHECK( object->isModified(), true );
		}

		pit = object->propertiesBegin();
		for ( ; pit != pend; ++pit ) {
			if ( (*pit).readOnly() )
				continue;
			value = (*pit).value();
			(*pit).setValue( value );
			if ( ! object->isModified() )
				kdDebug() << "Class: '" << cit.key() << "', Property: '" << (*pit).name() << "' misses the MODIFIED macro." << endl;
			object->setModified( false );
		}
	}
}

void TestBackendTest::general()
{
	kdDebug() << k_funcinfo << endl;

	QTime time;

	Manager::self()->reset();
	Manager::self()->setMaxObjects( 200 );

	ObjectRef<Customer> customer = Customer::create();
	customer->setCode( "C0001" );
	customer->setCustomerName( "John" );
	customer->setAddress( "John's street, no number" );
	customer->setCity( "Melbourne" );
	customer->setZipCode( "AU 2004" );
	customer->setCountry( "Australia" );
	//customer->save();

	ObjectRef<CustomerOrder> order = CustomerOrder::create();
	order->setNumber( 040001 );
	order->setDate( QDate::currentDate() );
	order->setCustomer( customer );
	//order->save();

	time.start();
	ObjectRef<Article> a, b;
	for ( int i = 0; i < 95; ++i ) {
		a = Article::create();
		a->setCode( "A" + QString::number( i ).rightJustify( 5 ) );
		a->setLabel( "Article " + QString::number( i ).rightJustify( 5 ) );
		a->setDescription( "Well, this is the long description" );
		order->articles()->add( a );
		//kdDebug() << "Count: " << QString::number( Manager::self()->count() ) << endl;
		//kdDebug() << "Oid: " << oidToString( a->oid() ) << endl;
	}
	kdDebug() << "Temps bucle: " << QString::number( time.elapsed() ) << endl;

	QValueList<OidType> list;
	CollectionIterator it( order->articles()->begin() );
	CollectionIterator end( order->articles()->end() );
	ObjectRef<Article> first;
	for ( ; it != end; ++it ) {
		Article *o = (Article*)*it;
		//kdDebug() << "(" << oidToString( o->oid() ) << "," << o->code() << "," << o->label() << "," << o->description() << ")" << endl;
		list.append( o->oid() );
		if ( ! first )
			first = o;
	}

	kdDebug() << "FIRST: '" << first->label() << "'" << endl;
	CollectionIterator it4( first->orders()->begin() );
	CollectionIterator end4( first->orders()->end() );
	for ( ; it4 != end4; ++it4 ) {
		CustomerOrder *o = (CustomerOrder*)*it4;
		kdDebug() << "=> [" << oidToString( o->oid() ) << ";" << QString::number( o->number() ) << "," << o->date().toString() << "]" << endl;
	}
	kdDebug() << "LAST" << endl;
	for ( int i = 0; i < 50; ++i )
		order->articles()->remove( list[i] );

	kdDebug() << "Articles eliminats" << endl;

	CollectionIterator it2( order->articles()->begin() );
	CollectionIterator end2( order->articles()->end() );
	for ( ; it2 != end2; ++it2 ) {
		Article *o = (Article*)*it2;
		//kdDebug() << "(" << oidToString( o->oid() ) << "," << o->code() << "," << o->label() << "," << o->description() << ")" << endl;
		list.append( o->oid() );
	}
	CHECK( order->articles()->count(), 45 );

	CollectionIterator it3( first->orders()->begin() );
	CollectionIterator end3( first->orders()->end() );
	for ( ; it3 != end3; ++it3 ) {
		CustomerOrder *o = (CustomerOrder*)*it3;
		kdDebug() << "=> [" << oidToString( o->oid() ) << ";" << QString::number( o->number() ) << "," << o->date().toString() << "]" << endl;
	}
	CHECK( order->articles()->count(), 45 );
}

void TestBackendTest::references()
{
	kdDebug() << k_funcinfo << endl;
	Manager::self()->reset();

	ObjectRef<CustomerOrder> o1 = CustomerOrder::create();
	CHECK( o1->oid(), 1 );
	ObjectRef<CustomerOrder> o2 = CustomerOrder::create();
	CHECK( o2->oid(), 2 );

	o1->setOrder( o2 );
	CHECK( o1->order()->oid(), 2 );
	CHECK( o2->order()->oid(), 1 );

	o2->setOrder( 0 );
	CHECK( o2->order(), (CustomerOrder*)0 );
	CHECK( o1->order(), (CustomerOrder*)0 );

	o2->setOrder( o1 );
	CHECK( o1->order()->oid(), 2 );
	CHECK( o2->order()->oid(), 1 );

	Manager::self()->remove( o1 );
	CHECK( o2->order(), (CustomerOrder*)0 );
	CHECK( Manager::self()->objects().count(), 1 );
	Manager::self()->remove( o2 );
	CHECK( Manager::self()->objects().count(), 0 );
}

void TestBackendTest::rollback()
{
	kdDebug() << k_funcinfo << endl;
	Manager::self()->reset();
	ObjectRef<CustomerOrder> o1 = CustomerOrder::create();
	ObjectRef<CustomerOrder> o2 = CustomerOrder::create();
	o1->setOrder( o2 );
	CHECK( Manager::self()->objects().count(), 2 );
	CHECK( Manager::self()->rollback(), true );
	CHECK( Manager::self()->objects().count(), 0 );
}

void TestBackendTest::commit()
{
	kdDebug() << k_funcinfo << endl;
	uint num = 0;
	Manager::self()->reset();
	ObjectRef<CustomerOrder> o1 = CustomerOrder::create();
	ObjectRef<CustomerOrder> o2 = CustomerOrder::create();
	o1->setOrder( o2 );
	CHECK( Manager::self()->objects().count(), num + 2 );
	CHECK( Manager::self()->commit(), true );
	CHECK( Manager::self()->objects().count(), num + 2 );
	CHECK( Manager::self()->rollback(), true );
	CHECK( Manager::self()->objects().count(), num + 2 );
}

void TestBackendTest::printClasses()
{
	kdDebug() << k_funcinfo << endl;

	ClassInfoIterator cit( Classes::begin() );
	ClassInfoIterator cend( Classes::end() );
	ClassInfo *current;
	for ( ; cit != cend; ++cit ) {
		current = (*cit);

		kdDebug() << "Class: " << current->name() << endl;
		kdDebug() << "    Objects:" << endl;
		RelationInfosIterator oit( current->relationsBegin() );
		RelationInfosIterator oend( current->relationsEnd() );
		RelationInfo *obj;
		for ( ; oit != oend; ++oit ) {
			obj = (*oit);
			QString n;
			n = obj->isOneToOne() ? "1-1" : "N-1";
			kdDebug() << "        Name: " << obj->name() << "  Class: " << obj->relatedClassInfo()->name() << "  Relation: " << n << endl;
		}

		kdDebug() << "    Collections:" << endl;
		CollectionInfosIterator lit( current->collectionsBegin() );
		CollectionInfosIterator lend( current->collectionsEnd() );
		CollectionInfo *col;
		for ( ; lit != lend; ++lit ) {
			col = (*lit);

			QString n;
			n = col->isNToOne() ? "N-1" : "N-N";
			kdDebug() << "        Name: " << col->name() << "  Class: " << col->childrenClassInfo()->name() << "  Relation: " << n << endl;
		}
	}
}

void TestBackendTest::allTests()
{
/*
	QSqlDatabase *db = QSqlDatabase::addDatabase( "QPSQL7" );
	db->setDatabaseName( "test" );
	db->setUserName( "albert" );
	db->setPassword( "" );
	db->setHostName( "localhost" );
	if ( ! db->open() ) {
		qFatal( "Failed to open sales database: " + db->lastError().text() );
	}


	DbBackendIface *backend = new SqlDbBackend( db );

	DbBackendIface *backend = new XmlDbBackend( "database.xml" );
  */
	Classes::setup();
	DbBackendIface *backend = new TestBackend();
	m_manager = new Manager( backend );
	m_manager->createSchema();
	Manager::self()->setMaxObjects( Manager::Unlimited );
	/*
	ObjectRef<Customer> customer = Customer::create( 1 );
	ObjectRef<Order> order = Order::create();
	ObjectRef<Article> a1 = Article::create();
	ObjectRef<Article> a2 = Article::create();
	ObjectRef<Article> a3 = Article::create();

	kdDebug() << "OID GENERAT: " << order->oid() << endl;
	order->setCustomer( customer );
	order->setNumber( 1000 );
	order->setDate( QDate::currentDate() );

	a1->setCode( "A" );
	a1->setLabel( "Article A" );
	a1->setDescription( "Description A" );

	a2->setCode( "B" );
	a2->setLabel( "Article B" );
	a2->setDescription( "Description B" );

	a3->setCode( "C" );
	a3->setLabel( "Article C" );
	a3->setDescription( "Description C" );

	order->articles()->add( a1 );
	order->articles()->add( a2 );
	order->articles()->add( a3 );

	kdDebug() << "Code: " << customer->code() << endl;
	kdDebug() << "Name: " << customer->customerName() << endl;
	kdDebug() << "Address: " << customer->address() << endl;
	kdDebug() << "City: " << customer->city() << endl;
	kdDebug() << "Zip Code: " << customer->zipCode() << endl;
	kdDebug() << "Country: " << customer->country() << endl;
	*/

	//general();
	/*
	browseObjectProperties();
	browseObjectObjects();
	browseObjectCollections();
	browseCollectionObjects();
	setObjectObjects();
	modified();
	*/
	modified();
	printClasses();
	references();
	rollback();
	commit();
	// Needs collections working
	general();


	delete m_manager;
}
