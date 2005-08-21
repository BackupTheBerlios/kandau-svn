## Makefile.am for kandau

# set the include path for X, qt and KDE
INCLUDES = $(all_includes)

# these are the headers for your project


# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO

messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/kdedcoptest.pot

# this is the program that gets installed.  it's name is used for all
# of the other Makefile.am variables

# the application source, library search path, and link libraries

lib_LTLIBRARIES = libkandau.la
libkandau_la_LDFLAGS = $(all_libraries)
libkandau_la_SOURCES = collection.cpp dbbackendiface.cpp manager.cpp object.cpp \
		oidtype.cpp labels.cpp sqldbbackend.cpp seqtype.cpp xmldbbackend.cpp classes.cpp \
		inmemorysqldbbackend.cpp testbackend.cpp dynamicobject.cpp
include_HEADERS = collection.h dbbackendiface.h defs.h manager.h \
			object.h	oidtype.h testbackend.h xmldbbackend.h classes.h labels.h seqtype.h \
		sqldbbackend.h inmemorysqldbbackend.h



noinst_HEADERS = dynamicobject.h
