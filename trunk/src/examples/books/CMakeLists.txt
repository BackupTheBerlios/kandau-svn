project ( kandauexample )

include_directories ( ${KDE4_INCLUDES} ${QT_INCLUDES} ../../core ../../ui)

#set ( kandauexample_SRCS books.cpp book.cpp author.cpp)
set ( kandauexample_SRCS main.cpp)

kde4_add_executable ( kandauexample ${kandauexample_SRCS} )

target_link_libraries ( kandauexample ${KDE4_KDECORE_LIBS} ${KDE4_KDEUI_LIBS} ${KDE4_KFILE_LIBS} ${KDE4_KIO_LIBS} ${KDE4_KDE3SUPPORT_LIBS} ${QT_QTSQL_LIBRARY} kandauui kandaucore)
