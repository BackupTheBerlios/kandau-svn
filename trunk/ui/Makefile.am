INCLUDES = -I$(top_srcdir)/src $(all_includes)
METASOURCES = AUTO

lib_LTLIBRARIES = libkandauui.la
libkandauui_la_SOURCES = dialoggenerator.cpp classdialog.cpp \
	classmainwindow.cpp collectionlistview.cpp chooseobjectdialog.cpp
libkandauui_la_LDFLAGS = $(all_libraries) $(KDE_RPATH)
#libkandauui_la_LDADD = $(LIB_KDECORE)
include_HEADERS = dialoggenerator.h classdialog.h classmainwindow.h \
	chooseobjectdialog.h collectionlistview.h


libkandauui_la_LIBADD = $(top_builddir)/src/libkandau.la
libkandauui_la_DEPENDENCIES = ../src
