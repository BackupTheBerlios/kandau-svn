INCLUDES = -I$(top_srcdir)/src $(all_includes)
METASOURCES = AUTO

lib_LTLIBRARIES = libkandauui.la
libkandauui_la_SOURCES = dialoggenerator.cpp classdialog.cpp \
		classmainwindow.cpp collectionlistview.cpp chooseobjectdialog.cpp propertywidget.cpp \
		collectionchooser.cpp uiform.cpp uiclassdialog.cpp classchooser.cpp widgethandler.cpp \
		tablehandler.cpp comboboxhandler.cpp comboboxpropertyhandler.cpp \
	genericpropertyhandler.cpp
libkandauui_la_LDFLAGS = $(all_libraries) $(KDE_RPATH)
#libkandauui_la_LDADD = $(LIB_KDECORE)
include_HEADERS = dialoggenerator.h classdialog.h classmainwindow.h \
			chooseobjectdialog.h collectionlistview.h collectionchooser.h propertywidget.h \
	uiclassdialog.h uiform.h classchooser.h widgethandler.h comboboxhandler.h

libkandauui_la_LIBADD = $(top_builddir)/src/libkandau.la
libkandauui_la_DEPENDENCIES = ../src

noinst_HEADERS = tablehandler.h comboboxpropertyhandler.h \
	genericpropertyhandler.h
