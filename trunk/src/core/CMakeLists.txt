project (kandaucore)

include_directories ( ${KDE4_INCLUDES} )

set(kandaucore_SRCS
object.cpp
property.cpp
relation.cpp
classes.cpp
collection.cpp
dbbackendiface.cpp
defaultpropertymetainfo.cpp
desktopconfigdbbackend.cpp
dynamicobject.cpp
inmemorysqldbbackend.cpp
labelsmetainfo.cpp
manager.cpp
notificationhandler.cpp
notifier.cpp
oidtype.cpp
seqtype.cpp
sqldbbackend.cpp
testbackend.cpp
tokenizer.cpp
xmldbbackend.cpp
)

#kde4_add_library(kandaucore SHARED ${kandaucore_SRCS})
kde4_add_library(kandaucore STATIC ${kandaucore_SRCS})

target_link_libraries(kandaucore ${KDE4_KDECORE_LIBS} ${KDE4_KDEUI_LIBS} ${KDE4_KFILE_LIBS} ${KDE4_KIO_LIBS} ${KDE4_KDE3SUPPORT_LIBS} ${QT_QTSQL_LIBRARY})

install(TARGETS kandaucore DESTINATION ${LIB_INSTALL_DIR} )
