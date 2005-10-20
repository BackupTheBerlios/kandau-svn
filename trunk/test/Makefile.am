INCLUDES = -I$(top_srcdir)/src -I$(top_srcdir)/ui $(all_includes)

check_PROGRAMS = kunittest

METASOURCES = AUTO

kunittest_SOURCES = main.cpp kunittest.cpp qtester.cpp article.cpp customer.cpp \
	testbackendtest.cpp sqlbackendtest.cpp xmlbackendtest.cpp customerorder.cpp \
	inmemorysqlbackendtest.cpp multiplebackendstest.cpp dynamicobjectstest.cpp metainfotest.cpp
kunittest_LDFLAGS = $(all_libraries) $(KDE_RPATH)
kunittest_LDADD = $(top_builddir)/src/libkandau.la $(LIB_KDECORE)

check: kunittest
	@./kunittest 2>&1 | grep "tests:"
noinst_HEADERS = article.h customer.h testbackendtest.h sqlbackendtest.h \
	xmlbackendtest.h dcopiface.h objectaccess.h customerorder.h inmemorysqlbackendtest.h \
	multiplebackendstest.h dynamicobjectstest.h metainfotest.h


bin_PROGRAMS = dcoptester uitester
dcoptester_LDFLAGS = $(all_libraries) $(KDE_RPATH)
dcoptester_LDADD = $(top_builddir)/src/libkandau.la $(LIB_KDECORE) $(LIB_KDEUI)
dcoptester_SOURCES = dcoptester.cpp objectaccess.cpp dcopiface.skel article.cpp \
	customer.cpp customerorder.cpp
uitester_LDFLAGS = $(all_libraries) $(KDE_RPATH)
uitester_LDADD = $(top_builddir)/ui/libkandauui.la \
	$(top_builddir)/src/libkandau.la $(LIB_KDECORE) $(LIB_KDEUI)
uitester_SOURCES = article.cpp customer.cpp customerorder.cpp uitester.cpp
