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
#include <qregexp.h>
#include <qstringlist.h>

#include <kdebug.h>

#include "tokenizer.h"


MTokenizer::MTokenizer( const QString& text, const QStringList& symbols )
{
	m_text = text;
	m_symbols = QRegExp::escape( symbols.join( QString::null ) ); //\s=><\(\)
	m_index = 0;
}

QString MTokenizer::nextToken()
{
	QString token;
	int nextIndex;
	kdDebug() << k_funcinfo << ": m_text = " << m_text << endl;
	kdDebug() << k_funcinfo << ": m_symbols = " << m_symbols << endl;
	nextIndex = m_text.find( QRegExp( "[" + m_symbols + "]" ), m_index );
	kdDebug() << k_funcinfo << ": nextIndex = " << nextIndex << endl;
	if ( nextIndex == -1 )
		return QString::null;

	token = m_text.mid( m_index, nextIndex - m_index );
	kdDebug() << k_funcinfo << ": token = " << token << endl;
	m_index = nextIndex + 1;
	kdDebug() << k_funcinfo << ": m_index = " << m_index << endl;
	return token;
}

QString MTokenizer::head()
{
	return m_text.left( m_index );
}

QString MTokenizer::tail()
{
	return m_text.right( m_text.length() - m_index );
}

int MTokenizer::currentIndex()
{
	return m_index;
}
