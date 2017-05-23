cmake_minimum_required(VERSION 2.6)

PROJECT(qxt)

find_package(Qt4 REQUIRED)

INCLUDE(${QT_USE_FILE})

set(QXT-SOURCES
  qxtglobal.cpp
  qxtglobalshortcut.cpp
  qxtwindowsystem.cpp
)

set(QXT-MOC-HEADERS
  qxtglobalshortcut.h qxtwindowsystem.h
)

if(WIN32)
  set(QXT-SOURCES ${QXT-SOURCES} qxtglobalshortcut_win.cpp qxtwindowsystem_win.cpp)
elseif(APPLE)
  set(QXT-SOURCES ${QXT-SOURCES} qxtglobalshortcut_mac.cpp qxtwindowsystem_mac.cpp)
else(WIN32)
  set(QXT-SOURCES ${QXT-SOURCES} qxtglobalshortcut_x11.cpp qxtwindowsystem_x11.cpp)
endif(WIN32)

QT4_WRAP_CPP(QXT-SOURCES-MOC ${QXT-MOC-HEADERS})

# add special flags for compilation
ADD_DEFINITIONS( -DQXT_STATIC )

ADD_LIBRARY(qxt STATIC
  ${QXT-SOURCES}
  ${QXT-SOURCES-MOC}
)
