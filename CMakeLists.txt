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
)

################################################################
# src
################################################################

set (src_SRC
src/main.cpp
src/dbfredactor.cpp
src/dbfredactormodel.cpp
src/dbfredactormainwindow.cpp
)

set (src_HEADERS
src/dbfredactor.h
src/dbfredactormodel.h
src/dbfredactorpage.h
src/dbfredactormainwindow.h
)

set(
SOURCES
${src_SRC}
)

set(
HEADERS
${src_HEADERS}
)

qt4_wrap_cpp( MOC_SOURCES ${HEADERS} )

set( RESOURCES resource.qrc )

qt4_add_resources( QRC_SOURCES ${RESOURCES} )

#add_executable( QDBFRedactor WIN32 ${SOURCES} ${MOC_SOURCES} ${QRC_SOURCES})
add_executable( QDBFRedactor ${SOURCES} ${MOC_SOURCES} ${QRC_SOURCES})

target_link_libraries( QDBFRedactor ${QT_LIBRARIES} )


