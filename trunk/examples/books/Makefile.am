INCLUDES = -I$(top_srcdir)/src -I$(top_srcdir)/ui $(all_includes)
METASOURCES = AUTO
bin_PROGRAMS = books
books_LDFLAGS = $(all_libraries)
books_SOURCES = book.cpp author.cpp books.cpp
noinst_HEADERS = author.h
books_LDADD = $(top_builddir)/ui/libkandauui.la \
	$(top_builddir)/src/libkandau.la -lqui $(LIB_KDECORE) $(LIB_KIO) $(LIB_KDEUI)
books_DEPENDENCIES = ../../src
