INCLUDES = -I/usr/local/kde/include -I../src $(all_includes)
METASOURCES = AUTO

lib_LTLIBRARIES = libkandauui.la
libkandauui_la_SOURCES = dialoggenerator.cpp classdialog.cpp \
	classmainwindow.cpp collectionlistview.cpp chooseobjectdialog.cpp
libkandauui_la_LDFLAGS = $(all_libraries) $(KDE_RPATH) -lkandau -L/usr/local/kde/lib
#libkandauui_la_LDADD = $(LIB_KDECORE)
include_HEADERS = dialoggenerator.h classdialog.h classmainwindow.h \
	chooseobjectdialog.h collectionlistview.h


