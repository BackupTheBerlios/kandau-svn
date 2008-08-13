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
#ifndef FASTCGI_H
#define FASTCGI_H

#include <qstring.h>
#include <qstringlist.h>

#include <fcgiapp.h>

#include "fastcgidevice.h"

class QTextStream;

/**
	@author Albert Cervera Areny <albertca@hotpop.com>
*/
class FastCgi 
{
public:
	FastCgi() {};
	virtual ~FastCgi() {};

	void run();

protected:
	virtual void handleRequest() = 0;

	QString parameter( const QString& name ) const;
	QStringList parameters() const;

	void setContentType( const QString& type );
	const QString& contentType() const;

	QTextStream& input();
	QTextStream& output();

private:
	FCGX_Stream *m_in, *m_out, *m_error;
	
	FCGX_ParamArray m_parameters;
	QTextStream *m_outputStream;
	QTextStream *m_inputStream;
	QString m_output;
	QString m_input;
	QString m_contentType;
};

#endif
