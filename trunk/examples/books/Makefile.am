INCLUDES = -I$(top_srcdir)/src $(all_includes)
METASOURCES = AUTO
bin_PROGRAMS = books
books_LDFLAGS = $(all_libraries)
books_SOURCES = book.cpp author.cpp books.cpp
noinst_HEADERS = author.h
books_LDADD = $(top_builddir)/src/libkandau.la -lkdecore -lqt-mt
books_DEPENDENCIES = ../../src
