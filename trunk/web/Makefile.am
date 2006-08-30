INCLUDES = $(all_includes)
METASOURCES = AUTO

lib_LTLIBRARIES = libkandauweb.la
libkandauweb_la_LDFLAGS = $(all_libraries)
libkandauweb_la_SOURCES = httpserver.cpp service.cpp kandauservice.cpp \
	fastcgi.cpp fastcgidevice.cpp kandaufastcgi.cpp
include_HEADERS = httpserver.h service.h kandauservice.h


noinst_HEADERS = fastcgi.h fastcgidevice.h kandaufastcgi.h
libkandauweb_la_LIBADD = -lfcgi
