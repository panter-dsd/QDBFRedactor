project(QDBFRedactor)
cmake_minimum_required( VERSION 2.6 )

#SET (CMAKE_BUILD_TYPE Release)
SET (CMAKE_BUILD_TYPE Debug)

INCLUDE(FindQt4)
find_package( Qt4 REQUIRED )
SET(QT_USE_QTXML 1)
SET(QT_USE_QTNETWORK 1)
include( ${QT_USE_FILE})
add_definitions(-DUNICODE)

include_directories(
src
src/core
src/preferences
src/widgets
src/dialogs
src/3dparty/qtsingleapplication/src
)

################################################################
# src
################################################################

set (src_SRC
src/main.cpp
#src/dbfredactormainwindow.cpp
#src/dbfredactordelegate.cpp
src/translationmanager.cpp
)

set (src_HEADERS
#src/dbfredactorpage.h
#src/dbfredactormainwindow.h
#src/dbfredactordelegate.h
#src/filterdelegate.h
src/translationmanager.h
)

################################################################
# core
################################################################
set (core_SRC
src/core/dbffield.cpp
src/core/dbfheader.cpp
#src/core/dbfredactor.cpp
#src/core/dbfredactormodel.cpp
#src/core/dbfredactorsortfilterproxymodel.cpp
)

set (core_HEADERS
src/core/dbffield.h
src/core/dbfheader.h
#src/core/dbfredactor.h
#src/core/dbfredactormodel.h
#src/core/dbfredactorsortfilterproxymodel.h
)

################################################################
# dialogs
################################################################

set (dialogs_SRC
#src/dialogs/sortdialog.cpp
#src/dialogs/filterdialog.cpp
#src/dialogs/aboutdialog.cpp
#src/dialogs/structdialog.cpp
)

set (dialogs_HEADERS
#src/dialogs/sortdialog.h
#src/dialogs/filterdialog.h
#src/dialogs/aboutdialog.h
#src/dialogs/structdialog.h
)

################################################################
# preferences
################################################################
set (preferences_SRC
#src/preferences/preferencesdialog.cpp
#src/preferences/languagepreferences.cpp
#src/preferences/displaypreferences.cpp
#src/preferences/globalpreferences.cpp
)

set (preferences_HEADERS
#src/preferences/preferencesdialog.h
#src/preferences/abstractpreferencespage.h
#src/preferences/languagepreferences.h
#src/preferences/displaypreferences.h
#src/preferences/globalpreferences.h
)

################################################################
# widgets
################################################################
set (widgets_SRC
#src/widgets/qtcolorbutton.cpp
#src/widgets/qtfontbutton.cpp
)

set (widgets_HEADERS
#src/widgets/qtcolorbutton.h
#src/widgets/qtfontbutton.h
)

################################################################
# qtsingleapplication
################################################################
set(qtsingleapplication_SRC
src/3dparty/qtsingleapplication/src/qtlocalpeer.cpp
src/3dparty/qtsingleapplication/src/qtsingleapplication.cpp
)

set(qtsingleapplication_HEADERS
src/3dparty/qtsingleapplication/src/qtlocalpeer.h
src/3dparty/qtsingleapplication/src/qtsingleapplication.h
)


################################################################
# all
################################################################
set(
SOURCES
${core_SRC}
${src_SRC}
${dialogs_SRC}
${preferences_SRC}
${widgets_SRC}
${qtsingleapplication_SRC}
)

set(
HEADERS
${core_HEADERS}
${src_HEADERS}
${dialogs_HEADERS}
${preferences_HEADERS}
${widgets_HEADERS}
${qtsingleapplication_HEADERS}
)

qt4_wrap_cpp( MOC_SOURCES ${HEADERS} )

set( RESOURCES resource.qrc )

qt4_add_resources( QRC_SOURCES ${RESOURCES} )

if(MINGW)
  ADD_CUSTOM_COMMAND(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/qdbfredactor_rc.o
  COMMAND windres.exe -I${CMAKE_CURRENT_SOURCE_DIR}
  -i${CMAKE_CURRENT_SOURCE_DIR}/qdbfredactor.rc
   -o ${CMAKE_CURRENT_BINARY_DIR}/qdbfredactor_rc.o)
	SET(qdbfredactor_RC ${qdbfredactor_RC} ${CMAKE_CURRENT_BINARY_DIR}/qdbfredactor_rc.o)
else(MINGW)
	SET(qdbfredactor_RC ${qdbfredactor_RC} qdbfredactor.rc)
endif(MINGW)


if(${CMAKE_BUILD_TYPE} STREQUAL Debug)
	add_executable( QDBFRedactor ${SOURCES} ${MOC_SOURCES} ${QRC_SOURCES} ${qdbfredactor_RC})
else()
	add_executable( QDBFRedactor WIN32 ${SOURCES} ${MOC_SOURCES} ${QRC_SOURCES} ${qdbfredactor_RC})
endif()

target_link_libraries( QDBFRedactor ${QT_LIBRARIES} )
