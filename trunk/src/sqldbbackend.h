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
#ifndef SQLDBBACKEND_H
#define SQLDBBACKEND_H

#include "dbbackendiface.h"
#include "oidtype.h"
#include "seqtype.h"

class QSqlCursor;
class QSqlDatabase;
class RelatedCollection;

/**
@author Albert Cervera Areny
*/

//#define SQL( name ) _#name_
#define ERROR( msg ) { kdDebug() << msg << endl; return false; }

/*!
	This class implements a backend for accessing/storing objects in a SQL database. It uses the database access classes provided by Qt.
*/
class SqlDbBackend : public DbBackendIface
{
public:

	/*!
	La constructora SQLDataObjetManager espera com a paràmetre un apuntador a una
	base de dades oberta.
	*/
	SqlDbBackend( QSqlDatabase *db );
	virtual ~SqlDbBackend();

	QSqlDatabase *database();


	void setup();
	void shutdown();

	/*!
	This function loads an object from the database from the oid of the
	object and a pointer to an Object.
	*/
	virtual bool load( const OidType& oid, Object *object );

	/*
	(Heredada de SQLObjectManagerIface)
	*/
//	virtual bool load( Collection *collection , QString criteria = "");
	virtual bool load( Collection *collection);

	/*!
	This function saves an object to the database using a pointer to an Object.
	*/
	virtual bool save( Object *object );

	/*!
	This function deletes an object from the database from a pointer to the Object
	*/
	virtual bool remove( Object *object );

	/*!
	This function inserts an object to a collection
	(Heredada de SQLObjectManagerIface)
	*/
	virtual bool add( Collection *collection, Object *object );

	/*!
	This function removes an object form a collection
	(Heredada de SQLObjectManagerIface)
	*/
	virtual bool remove( Collection *collection, const OidType& oid );

	/*!
	Creates and executes all CREATE TABLE / SEQUENCE.. necessari for creating
	the SQL Database.
	(Heredada de SQLObjectManagerIface)
	*/
	virtual bool createSchema();

	void setRelation( const OidType& oid, const QString& relation, const OidType& oidRelated, const OidType& oldOid );

	/*! Decides whether the object changed in the database since last load */
	virtual bool hasChanged( Object * object );

	/*!
	Aquesta funció retorna el nom de la taula corresponent a un objecte, actualment
	retorna: className() + "_", però podria variar. La funció és pública per permetre
	a aquelles aplicacions que necessitin accés directe a les taules de la base de dades
	accedir als noms correctes de les taules.
	*/
	virtual QString tableName( const Object *object ) const;


	/*!
	Starts a transaction
	 */
	bool start();

	/*!
	Commits the current transaction
	 */
	bool commit();

	/*!
	Aborts the current transaction
	 */
	bool rollback();

	/*!
	Té el mateix comportament que la funció anterior però a partir del nom d'una classe.
	*/
	virtual QString tableName( const QString &className ) const;

	/*!
	Genera un nou Oid a partir d'una seqüència de la base de dades.
	*/
	virtual OidType newOid();

	virtual SqlDbBackend* SqlBackend( );

	/*!
	Aquesta funció no s'hereda de ObjectManagerIface. Hauria de ser protegida o privada,
	però es fa pública per permetre hagilitzar aquelles funcions que treballen directament sobre
	la base de dades i volen carregar objectes segons un criteri de consulta.
	*/
	virtual bool load( const QSqlCursor &cursor, Object *object );

	virtual void reset() {};
protected:
	/*!
	Retorna un nou número de seqüència. NO necessàriament ha de ser nou,
	newSeq pot gestionar els números de seqüència com li doni la gana.
	*/
	virtual SeqType newSeq();
	virtual QString tableName( RelatedCollection *collection ) const;
	virtual QString filterFieldName( RelatedCollection *collection ) const;
	virtual OidType filterValue( Collection *collection ) const;
	virtual QString idFieldName( RelatedCollection *collection ) const;
	virtual QString sqlType( QVariant::Type type );
	QSqlDatabase *m_db;

	// The first OidType (key) stores the Object that references the second OidType
	QMap<OidType, QMap<QString,OidType> > m_removedRelations;
};


#endif
