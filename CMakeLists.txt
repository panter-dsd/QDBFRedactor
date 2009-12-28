project(QDBFRedactor)
cmake_minimum_required( VERSION 2.6 )

#SET (CMAKE_BUILD_TYPE Release)
SET (CMAKE_BUILD_TYPE Debug)

INCLUDE(FindQt4)
find_package( Qt4 REQUIRED )
SET(QT_USE_QTXML 1)
include( ${QT_USE_FILE})
add_definitions(-DUNICODE)

include_directories(
src
src/preferences
)

################################################################
# src
################################################################

set (src_SRC
src/main.cpp
src/dbfredactor.cpp
src/dbfredactormodel.cpp
src/dbfredactorsortfilterproxymodel.cpp
src/dbfredactormainwindow.cpp
src/sortdialog.cpp
src/filterdialog.cpp
src/dbfredactordelegate.cpp
src/translationmanager.cpp
)

set (src_HEADERS
src/dbfredactor.h
src/dbfredactormodel.h
src/dbfredactorpage.h
src/dbfredactorsortfilterproxymodel.h
src/dbfredactormainwindow.h
src/sortdialog.h
src/filterdialog.h
src/dbfredactordelegate.h
src/filterdelegate.h
src/translationmanager.h
)

################################################################
# preferences
################################################################
set (preferences_SRC
src/preferences/preferencesdialog.cpp
src/preferences/globalpreferences.cpp
src/preferences/displaypreferences.cpp
)

set (preferences_HEADERS
src/preferences/preferencesdialog.h
src/preferences/abstractpreferencespage.h
src/preferences/globalpreferences.h
src/preferences/displaypreferences.h
)



set(
SOURCES
${src_SRC}
${preferences_SRC}
)

set(
HEADERS
${src_HEADERS}
${preferences_HEADERS}
)

qt4_wrap_cpp( MOC_SOURCES ${HEADERS} )

set( RESOURCES resource.qrc )

qt4_add_resources( QRC_SOURCES ${RESOURCES} )

#add_executable( QDBFRedactor WIN32 ${SOURCES} ${MOC_SOURCES} ${QRC_SOURCES})
add_executable( QDBFRedactor ${SOURCES} ${MOC_SOURCES} ${QRC_SOURCES})

target_link_libraries( QDBFRedactor ${QT_LIBRARIES} )


