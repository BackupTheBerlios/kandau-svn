/**
 * tester.h
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
#ifndef TESTER_H
#define TESTER_H

#include <kdebug.h>
#include <qstringlist.h>

#include <oidtype.h>

#define CHECK( x, y ) check( __FILE__, __LINE__, #x, x, y )

class Tester
{
public:
    Tester()
        : m_tests( 0 )
    {}
    virtual ~Tester() {}

public:
    virtual void allTests() = 0;

public:
    int testsFinished() const {
        return m_tests;
    }
    int testsFailed() const {
        return m_errorList.count();
    }
    QStringList errorList() const {
        return m_errorList;
    }

protected:
    void check( const char *file, int line, const char *str,
                const OidType& result, const OidType& expectedResult )
    {
        if ( result != expectedResult ) {
            QString error;
            QTextStream ts( &error, IO_WriteOnly );
            ts << file << "["<< line <<"]:"
               <<" failed on \""<<  str <<"\""
               << "\ntests:\t\t result = '"
               << oidToString( result )
               << "', expected = '"<< oidToString( expectedResult ) << "'";
            m_errorList.append( error );
        }
        ++m_tests;
    }

    void check( const char *file, int line, const char *str,
                const QByteArray& result, const QByteArray& expectedResult )
    {
        if ( result != expectedResult ) {
            QString error;
            QTextStream ts( &error, IO_WriteOnly );
            ts << file << "["<< line <<"]:"
               <<" failed on \""<<  str <<"\""
               << "\ntests:\t\t result = '"
               << "QByteArray(" << QString::number( result.size() ) << ")', expected = 'QByteArray(" << QString::number( expectedResult.size() ) << ")'";
            m_errorList.append( error );
        }
        ++m_tests;
    }

    template<typename T>
    void check( const char *file, int line, const char *str,
                const T  &result, const T &expectedResult )
    {
        if ( result != expectedResult ) {
            QString error;
            QTextStream ts( &error, IO_WriteOnly );
            ts << file << "["<< line <<"]:"
               <<" failed on \""<<  str <<"\""
               << "\ntests:\t\t result = '"
               << result
               << "', expected = '"<< expectedResult<<"'";
            m_errorList.append( error );
        }
        ++m_tests;
    }
private:
    QStringList m_errorList;
    int m_tests;
};

#endif
