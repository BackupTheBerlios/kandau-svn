/**
 * kunittest.cpp
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
#include "kunittest.h"

#include "testbackendtest.h"
#include "sqlbackendtest.h"
#include "xmlbackendtest.h"
#include "inmemorysqlbackendtest.h"
#include "multiplebackendstest.h"

#include "qtester.h"
#include "tester.h"

#include <qapplication.h>
#include <qtimer.h>

void KUnitTest::registerTests()
{
	ADD_TEST( TestBackendTest );
	ADD_TEST( SqlBackendTest );
	ADD_TEST( InMemorySqlBackendTest );
	ADD_TEST( XmlBackendTest );
	ADD_TEST( MultipleBackendsTest );
}

KUnitTest::KUnitTest()
{
    QTimer::singleShot( 0, this, SLOT(checkRun()) );

    registerTests();
}

void KUnitTest::checkRun()
{
    if ( m_qtests.isEmpty() )
        qApp->exit();
}

int KUnitTest::runTests()
{
    int result = 0;
    kdDebug()<<"# Running normal tests... #"<<endl;
    QAsciiDictIterator<Tester> it( m_tests );

    for( ; it.current(); ++it ) {
        Tester* test = it.current();
        test->allTests();
        QStringList errorList = test->errorList();
        if ( !errorList.empty() ) {
            ++result;
            kdDebug()<< it.currentKey() <<" errors:" << endl;
            for ( QStringList::Iterator itr = errorList.begin();
                  itr != errorList.end(); ++itr ) {
                kdDebug()<< "\t" << (*itr).latin1() <<endl;;
            }
        } else {
            kdDebug()<< it.currentKey() << " OK "<<endl;
        }
    }
    kdDebug()<< "# Done with normal tests #" <<endl;
    return result;
}

void KUnitTest::addTester( QTester *test )
{
    m_qtests.insert( test, test );
    connect( test, SIGNAL(destroyed(QObject*)),
             SLOT(qtesterDone(QObject* )) );
}

void KUnitTest::qtesterDone( QObject *obj )
{
    m_qtests.remove( obj );
    if ( m_qtests.isEmpty() )
        qApp->quit();
}

#include "kunittest.moc"
