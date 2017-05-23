#
# DiffImg CMakeLists.txt
# 
# To build DiffImg, you will need cmake 2.6
# To build the application run this commands:
# 
#	cd build
#	cmake ./
#	make
# 
# If you don't kwon cmake, here are some links for you:
# 	http://wiki.qtcentre.org/index.php?title=Compiling_Qt4_apps_with_CMake
# 	http://qtnode.net/wiki/Qt_with_cmake#cmake_for_Qt_4

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

# set project's name
PROJECT(HotShots)

# note: change this BEFORE making a release !!
SET( MYAPP_VERSION "2.2.0" )
set( PROGNAME hotshots )

IF( APPLE )
  SET( MACOSX_BUNDLE_ICON_FILE ${PROGNAME}.icns )
  SET( MACOSX_BUNDLE_SHORT_VERSION_STRING ${MYAPP_VERSION} )
  SET( MACOSX_BUNDLE_VERSION ${MYAPP_VERSION} )
  SET( MACOSX_BUNDLE_LONG_VERSION_STRING Version ${MYAPP_VERSION} )
  SET( MACOSX_BUNDLE_INFO_PLIST Version "../res/Info.plist" )
  SET( CMAKE_OSX_ARCHITECTURES ppc;i386 ) #Comment out if not universal binary
ENDIF( APPLE )

CMAKE_MINIMUM_REQUIRED(VERSION 2.4.4)
SET(CMAKE_COLOR_MAKEFILE ON)
SET(CMAKE_VERBOSE_MAKEFILE OFF)
SET(CMAKE_INCLUDE_CURRENT_DIR TRUE)

# Packages path
SET (CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

# this command finds Qt4 libraries and sets all required variables
FIND_PACKAGE(Qt4 COMPONENTS QtCore QtGui QtNetwork QtXml REQUIRED)

IF (UNIX)
    FIND_PACKAGE(Qxt COMPONENTS QxtGui REQUIRED)
    FIND_PACKAGE(Xfixes REQUIRED)
ENDIF(UNIX)

# add some useful macros and variables
# (QT_USE_FILE is a variable defined by FIND_PACKAGE( Qt4 ) that contains a path to CMake script)
INCLUDE(${QT_USE_FILE})
INCLUDE_DIRECTORIES(${QT_INCLUDE_DIR} "../src/" "../src/3rdparty/qkeysequencewidget" "../src/3rdparty/wwWidgets" "../src/3rdparty/QSgml"  "../src/3rdparty/qt-json"  "../src/uploaders" "../src/editor" "../src/editor/items" "../src/editor/io")

IF (MSVC)
INCLUDE_DIRECTORIES(${QT_INCLUDE_DIR} "../src/3rdparty/qxt")
ENDIF( MSVC )

IF (UNIX)
    INCLUDE_DIRECTORIES(${QT_INCLUDE_DIR} "/usr/include/qxt/QxtGui" "/usr/include/qxt/QxtCore" "/usr/include/QxtGui" "/usr/include/QxtCore" "/usr/local/Qxt/include/QxtGui" "/usr/local/Qxt/include/QxtCore")
ENDIF( UNIX )



FILE(GLOB include_file_list "../src/*.h"  "../src/3rdparty/qkeysequencewidget/*.h" "../src/3rdparty/wwWidgets/*.h" "../src/3rdparty/QSgml/*.h" "../src/3rdparty/qt-json/*.h" "../src/uploaders/*.h" "../src/editor/*.h" "../src/editor/io/*.h" "../src/editor/widgets/*.h" "../src/editor/items/*.h" )
FILE(GLOB cpp_file_list "../src/*.cpp"  "../src/3rdparty/QSgml/*.cpp" "../src/3rdparty/wwWidgets/*.cpp" "../src/3rdparty/qt-json/*.cpp" "../src/3rdparty/qkeysequencewidget/*.cpp" "../src/uploaders/*.cpp" "../src/editor/*.cpp" "../src/editor/io/*.cpp" "../src/editor/widgets/*.cpp" "../src/editor/items/*.cpp"  )
IF( APPLE )
FILE(GLOB resource_file_list "../res/osx/*.qrc")
ELSE (APPLE)
FILE(GLOB resource_file_list "../res/*.qrc")
ENDIF(APPLE)
FILE(GLOB ui_file_list "../ui/*.ui")
FILE(GLOB trans_file_list "../lang/*.ts")

if(WIN32)
  set(include_file_list ${include_file_list} "../src/3rdparty/qxt/qxtglobal.h" "../src/3rdparty/qxt/qxtglobalshortcut.h" "../src/3rdparty/qxt/qxtglobalshortcut_p.h")
  set(cpp_file_list ${cpp_file_list} "../src/3rdparty/qxt/qxtglobalshortcut.cpp" "../src/3rdparty/qxt/qxtglobal.cpp" "../src/3rdparty/qxt/qxtwindowsystem.cpp" "../src/3rdparty/qxt/qxtglobalshortcut_win.cpp" "../src/3rdparty/qxt/qxtwindowsystem_win.cpp")
elseif(APPLE)
  set(include_file_list ${include_file_list} "../src/3rdparty/qxt/qxtglobal.h" "../src/3rdparty/qxt/qxtglobalshortcut.h" "../src/3rdparty/qxt/qxtglobalshortcut_p.h")
  set(cpp_file_list ${cpp_file_list} "../src/3rdparty/qxt/qxtglobalshortcut.cpp" "../src/3rdparty/qxt/qxtwindowsystem.cpp" "../src/3rdparty/qxt/qxtglobal.cpp" "../src/3rdparty/qxt/qxtglobalshortcut_mac.cpp" "../src/3rdparty/qxt/qxtwindowsystem_mac.cpp")
endif(WIN32)

SET(MYAPP_UIS
    ${ui_file_list}
)

SET(MYAPP_HDRS
    ${include_file_list}
)

SET(MYAPP_SRCS
    ${cpp_file_list}
)

SET(MYAPP_TRANS
    ${trans_file_list}
)

SET(MYAPP_RES
    ${resource_file_list}
)

# add special flags for compilation
ADD_DEFINITIONS( -DQXT_STATIC )

# enable warnings for GCC compiler
IF(CMAKE_COMPILER_IS_GNUCXX )
  ADD_DEFINITIONS( -Wall -Wextra )
ENDIF(CMAKE_COMPILER_IS_GNUCXX )

# qt4 definitinos
QT4_WRAP_UI         ( MYAPP_UIS_H ${MYAPP_UIS} )
QT4_WRAP_CPP        ( MYAPP_MOC_SRCS ${MYAPP_HDRS} )
QT4_ADD_RESOURCES   ( MYAPP_RESOURCES ${MYAPP_RES} )
QT4_AUTOMOC         ( ${MYAPP_SRCS} )
QT4_ADD_TRANSLATION ( MYAPP_QM ${MYAPP_TRANS} )

IF(COMMAND CMAKE_POLICY)
  CMAKE_POLICY(SET CMP0003 NEW)
  CMAKE_POLICY(SET CMP0005 OLD)
ENDIF(COMMAND CMAKE_POLICY)

# Arrange files in folder for Visual Studio -----------------------------------
SOURCE_GROUP("Generated Files" FILES ${MYAPP_MOC_SRCS} ${MYAPP_UIS_H} ${MYAPP_RESOURCES})
SOURCE_GROUP("UI Files" FILES ${MYAPP_UIS} )

# we need this to be able to include headers produced by uic in our code
# (CMAKE_BINARY_DIR holds a path to the build directory, while INCLUDE_DIRECTORIES() works just like INCLUDEPATH from qmake)
INCLUDE_DIRECTORIES( ${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/../src/editor/io ${CMAKE_CURRENT_SOURCE_DIR}/../src/editor/widgets ${CMAKE_CURRENT_SOURCE_DIR}/../src/editor/items )

# resource compilation for MinGW
IF( MINGW )
    ADD_CUSTOM_COMMAND( OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${PROGNAME}ico.o 
        COMMAND windres.exe -I${CMAKE_CURRENT_SOURCE_DIR} -i${CMAKE_CURRENT_SOURCE_DIR}/../res/${PROGNAME}.rc -o ${CMAKE_CURRENT_BINARY_DIR}/${PROGNAME}ico.o )
    SET(MYAPP_RESOURCES ${MYAPP_RESOURCES} ${CMAKE_CURRENT_BINARY_DIR}/${PROGNAME}ico.o)
ELSE( MINGW )
    SET(MYAPP_RESOURCES ${MYAPP_RESOURCES} ../res/${PROGNAME}.rc )
ENDIF( MINGW )

# Set some Win32 Specific Settings
IF(WIN32)
    SET(GUI_TYPE WIN32)

ENDIF(WIN32)

IF (MSVC)
    # Turn off deprecation warnings
    ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS)
    ADD_DEFINITIONS(-D_CRT_NONSTDC_NO_WARNINGS)
	ADD_DEFINITIONS(-D_CRT_NONSTDC_NO_DEPRECATE)
ENDIF(MSVC)

# Set some Apple MacOS Specific settings
IF (APPLE)
    SET(GUI_TYPE MACOSX_BUNDLE)
ENDIF (APPLE)

# here we instruct CMake to build "myapp" executable from all of the source files
IF( APPLE )
    ADD_EXECUTABLE( ${PROGNAME} ${GUI_TYPE} ${MYAPP_SRCS} ${MYAPP_UIS_H} ${MYAPP_RESOURCES} ${MYAPP_MOC_SRCS} )
    ADD_CUSTOM_COMMAND( TARGET ${PROGNAME} POST_BUILD
    COMMAND mkdir ARGS ${CMAKE_CURRENT_BINARY_DIR}/${PROGNAME}.app/Contents/Resources
    COMMAND cp ARGS ${MACOSX_BUNDLE_ICON_FILE} ${CMAKE_CURRENT_BINARY_DIR}/${PROGNAME}.app/Contents/Resources
    COMMAND cp ARGS *.qm ${CMAKE_CURRENT_BINARY_DIR}/${PROGNAME}.app/Contents/Resources )
ELSE( APPLE )
    ADD_EXECUTABLE( ${PROGNAME} WIN32 ${MYAPP_SRCS} ${MYAPP_UIS_H} ${MYAPP_MOC_SRCS} ${MYAPP_RESOURCES} ${MYAPP_QM} )
ENDIF( APPLE )
  
# last thing we have to do is to tell CMake what libraries our executable needs,
# luckily FIND_PACKAGE prepared QT_LIBRARIES variable for us:
IF (UNIX)
    TARGET_LINK_LIBRARIES( ${PROGNAME} ${QT_LIBRARIES} ${XFIXES_LIBRARIES} -lQxtGui -lX11 -ldl )
ELSEIF(MINGW)
   TARGET_LINK_LIBRARIES(${PROGNAME} ${QT_LIBRARIES}  gdi32 )
ELSE (UNIX)
    TARGET_LINK_LIBRARIES( ${PROGNAME} ${QT_LIBRARIES} )
ENDIF (UNIX)

IF(WIN32)
    SET_TARGET_PROPERTIES(${PROGNAME} PROPERTIES
        LINK_FLAGS "/VERSION:${MYAPP_VERSION}"
        LINK_FLAGS_RELEASE "/ENTRY:\"mainCRTStartup\" /SUBSYSTEM:WINDOWS"
        LINK_FLAGS_DEBUG "/SUBSYSTEM:CONSOLE" )
ENDIF(WIN32)
    

# build desktop file and manpage if needed
IF( UNIX AND NOT APPLE )
    IF( NOT DESKTOP_ENTRY )
      SET( DESKTOP_ENTRY ${PROGNAME}.desktop )
    ENDIF( NOT DESKTOP_ENTRY )

    ADD_CUSTOM_COMMAND( OUTPUT ${DESKTOP_ENTRY}
      COMMAND touch ${DESKTOP_ENTRY}
      COMMAND sh ${CMAKE_CURRENT_SOURCE_DIR}/${PROGNAME}-desktop.sh ${CMAKE_INSTALL_PREFIX} ${MYAPP_VERSION} >${DESKTOP_ENTRY}
      DEPENDS ${PROGNAME}-desktop.sh
      COMMENT "Generating desktop entry file"
      )
    ADD_CUSTOM_TARGET( DESKTOP_ENTRY_FILE ALL
      DEPENDS ${DESKTOP_ENTRY}
      )

    SET( APP_ICON ../res/${PROGNAME}.png )

    #Generate manpage
    IF( NOT MANPAGE_DIRECTORY )
      set( MANPAGE_DIRECTORY /usr/share/man/man1 )
    ENDIF( NOT MANPAGE_DIRECTORY)

    ADD_CUSTOM_COMMAND( OUTPUT ${PROGNAME}.1.gz
      COMMAND touch ${PROGNAME}.1
      COMMAND sh ${CMAKE_CURRENT_SOURCE_DIR}/${PROGNAME}-manpage.sh ${MYAPP_VERSION} >${PROGNAME}.1
      COMMAND gzip -f -9 ${PROGNAME}.1
      DEPENDS ${PROGNAME}-manpage.sh
      COMMENT "Generating manpage"
      )
    ADD_CUSTOM_TARGET( MANPAGE_FILE ALL
      DEPENDS ${PROGNAME}.1.gz
      )

    SET_DIRECTORY_PROPERTIES( ADDITIONAL_MAKE_CLEAN_FILES ${PROGNAME}.1 ${PROGNAME}.1.gz )
ENDIF (UNIX AND NOT APPLE)

# install

# windows
IF (WIN32)
    ADD_CUSTOM_COMMAND( TARGET ${PROGNAME} POST_BUILD
        # there is a nsi installer !!!
        COMMAND makensis.exe /DVERSION=${MYAPP_VERSION} /DSETUP_DIR=../setup ../setup/setup.nsi
        COMMENT "Generating setup via NSI installer"
    )
ENDIF (WIN32)

# linux
if (UNIX AND NOT APPLE)
    INSTALL ( TARGETS ${PROGNAME} RUNTIME DESTINATION bin )
    INSTALL ( FILES ${MYAPP_QM}   DESTINATION share/${PROGNAME}/locale )             
    # freedesktop file
    INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROGNAME}.desktop DESTINATION "${CMAKE_INSTALL_PREFIX}/share/applications/" )
    INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROGNAME}.1.gz DESTINATION "${CMAKE_INSTALL_PREFIX}/share/man/man1/" )
    INSTALL(FILES ../res/${PROGNAME}.png DESTINATION "${CMAKE_INSTALL_PREFIX}/share/pixmaps/" )
    INSTALL(FILES hotshots.xml DESTINATION "${CMAKE_INSTALL_PREFIX}/share/mime/packages" )
    INSTALL(FILES ../CREDITS.txt ../README.txt ../AUTHORS.txt ../Changelog.txt DESTINATION share/${PROGNAME})

endif (UNIX AND NOT APPLE)






