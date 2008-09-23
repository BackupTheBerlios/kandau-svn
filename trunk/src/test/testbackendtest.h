/**
 * sampletest.h
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
#ifndef SAMPLETEST_H
#define SAMPLETEST_H

#include "tester.h"

class Manager;

class TestBackendTest : public Tester
{
public:
	TestBackendTest();
	void testBrowsing();

	/*!
	This function should test all the capabilities of browsing
	the properties of an object. Mainly iterators.
	*/
	void browseObjectProperties();

	/*!
	This function should test all the capabilities of browsing
	the objects related to an object. Mainly iterators.
	*/
	void browseObjectObjects();

	/*!
	This function should test all the capabilities of browsing
	the collections related to an object. Mainly iterators.
	*/
	void browseObjectCollections();

	/*!
	This function should test all the capabilities of browsing
	the objects in a collection. Mainly iterators.
	*/
	void browseCollectionObjects();

	/*!
	*/
	void setObjectObjects();

	/*!
	This function iterates through all available classes and checks
	that none of the "get" functions set the modified flag to the object
	and all "set" functions set the flag.
	*/
	void modified();

	/*!
	This makes a general test of functionalities.
	*/
	void general();

	/*!
	Tests if when a reference to an object is modified propagate references correctly
	*/
	void references();

	/*!
	Tests the rollback functionality. It thus looks if modified objects are removed as appropiate.
	 */
	void rollback();
	
	/*!
	Tests the commit functionality. It thus looks if modified objects are saved as appropiate.
	 */
	void commit();
		
	/*!
	Prints all the static information of relations from Classes and ClassInfo.
	*/
	void printClasses();

	void allTests();

private:
	Manager *m_manager;
};

#endif
