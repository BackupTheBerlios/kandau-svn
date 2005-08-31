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
#include <qregexp.h>
#include <qstringlist.h>

#include "tokenizer.h"


Tokenizer::Tokenizer( const QString& text, const QStringList& symbols )
{
	m_text = text;
	m_symbols = QRegExp::escape( symbols.join( QString::null ) ); //\s=><\(\)
	m_index = 0;
}

QString Tokenizer::nextToken()
{
	QString token;
	int nextIndex;
	nextIndex = m_text.find( QRegExp( "[" + m_symbols + "]" ), m_index );
	if ( nextIndex == -1 )
		return QString::null;

	token = m_text.mid( m_index, nextIndex - m_index );
	m_index = nextIndex + 1;
	return token;
}

QString Tokenizer::head()
{
	return m_text.left( m_index );
}

QString Tokenizer::tail()
{
	return m_text.right( m_text.length() - m_index );
}

int Tokenizer::currentIndex()
{
	return m_index;
}
