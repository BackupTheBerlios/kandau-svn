project (kandauui)

include_directories ( ${KDE4_INCLUDES} ${QT_INCLUDES} ../core)

set(kandauui_SRCS
collectionmodel.cpp
#collectionview.cpp
#dialoggenerator.cpp
#classdialog.cpp
#classmainwindow.cpp
#chooseobjectdialog.cpp
#propertywidget.cpp
#collectionchooser.cpp
#uiform.cpp
#uiclassdialog.cpp
#classchooser.cpp
#widgethandler.cpp
#tablehandler.cpp
#comboboxhandler.cpp
#comboboxpropertyhandler.cpp
#genericpropertyhandler.cpp
)

#kde4_add_library(kandauui SHARED ${kandauui_SRCS})
kde4_add_library(kandauui STATIC ${kandauui_SRCS})

target_link_libraries(kandauui ${KDE4_KDECORE_LIBS} ${KDE4_KDEUI_LIBS} ${KDE4_KFILE_LIBS} ${KDE4_KIO_LIBS} ${KDE4_KDE3SUPPORT_LIBS} ${QT_QTSQL_LIBRARY} kandaucore)

install(TARGETS kandauui DESTINATION ${LIB_INSTALL_DIR} )
