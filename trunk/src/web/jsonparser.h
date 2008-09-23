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
#ifndef JSONPARSER_H
#define JSONPARSER_H

#include <qvariant.h>
#include <qvector.h>

/**
	@author Albert Cervera Areny <albertca@hotpop.com>
*/
class JsonObject;
class JsonArray;
class JsonString;
class JsonNumber;
class JsonBoolean;

class JsonValue
{
public:
	enum Type {
		Object,
		Array,
		String,
		Number,
		Boolean,
		Null
	};
	virtual ~JsonValue() = 0;

	virtual Type type() const
	{
		return Null;
	}

	bool isNull() const
	{
		return type() == Null;
	}
	
	virtual const JsonObject* toObject() const
	{
		return NULL;
	}
	
	virtual const JsonArray* toArray() const
	{
		return NULL;
	}
	
	virtual const JsonString* toString() const
	{
		return NULL;
	}
	
	virtual const JsonNumber* toNumber() const
	{
		return NULL;
	}
	virtual const JsonBoolean* toBoolean() const
	{
		return NULL;
	}
};

class JsonObject : public JsonValue 
{
public:
	Type type() const
	{
		return Object;
	}
	const JsonObject* toObject() const
	{
		return this;
	}
	const QMap<QString,JsonValue>& properties() const;

private:
	QMap<QString,JsonValue> m_values;
};

class JsonArray : public JsonValue
{
public:
	Type type() const
	{
		return Array;
	}
	const JsonArray* toArray() const
	{
		return this;
	}
	const QVector<JsonValue>& values() const;
private:
	QVector<JsonValue> m_values;
};

class JsonNumber : public JsonValue
{
public:
	Type type() const
	{
		return Number;
	}
	const JsonNumber* toNumber() const
	{
		return this;
	}
	Q_LLONG toLongLong() const
	{
		return m_value.toLongLong();
	}
	Q_ULLONG toULongLong() const
	{
		return m_value.toULongLong();
	}
	double toDouble() const
	{
		return m_value.toDouble();
	}
	int toInt() const
	{
		return m_value.toInt();
	}
	uint toUInt() const
	{
		return m_value.toUInt();
	}
private:
	QVariant m_value;
};

class JsonString : public JsonValue
{
public:
	Type type() const
	{
		return String;
	}
	const JsonString* toString() const
	{
		return this;
	}
	const QString& value() const
	{
		return m_value;
	}
private:
	QString m_value;
};

class JsonBoolean : public JsonValue
{
public:
	Type type() const
	{
		return Boolean;
	}
	const JsonBoolean* toBoolean() const
	{
		return this;
	}
	bool value() const
	{
		return m_value;
	}
private:
	bool m_value;
};

class JsonParser
{
public:
	static JsonValue parse( const QString& text );
};

#endif
