INCLUDES = -I/usr/local/kde/include -I../src $(all_includes)

check_PROGRAMS = kunittest

METASOURCES = AUTO

kunittest_SOURCES = main.cpp kunittest.cpp qtester.cpp article.cpp customer.cpp \
	testbackendtest.cpp sqlbackendtest.cpp xmlbackendtest.cpp customerorder.cpp \
	inmemorysqlbackendtest.cpp multiplebackendstest.cpp dynamicobjectstest.cpp metainfotest.cpp
kunittest_LDFLAGS = $(all_libraries) $(KDE_RPATH) -lkandau \
	-L/usr/local/kde/lib -lpq
kunittest_LDADD = $(LIB_KDECORE)

check: kunittest
	@./kunittest 2>&1 | grep "tests:"
noinst_HEADERS = article.h customer.h testbackendtest.h sqlbackendtest.h \
	xmlbackendtest.h dcopiface.h objectaccess.h customerorder.h inmemorysqlbackendtest.h \
	multiplebackendstest.h dynamicobjectstest.h metainfotest.h
AM_CFLAGS = -I../src
AM_CXXFLAGS = -I../src
bin_PROGRAMS = dcoptester
dcoptester_LDFLAGS = $(all_libraries) $(KDE_RPATH) -lkandau \
	-L/usr/local/kde/lib
dcoptester_LDADD = $(LIB_KDECORE) $(LIB_KDEUI)
dcoptester_SOURCES = dcoptester.cpp objectaccess.cpp dcopiface.skel article.cpp \
	customer.cpp customerorder.cpp
