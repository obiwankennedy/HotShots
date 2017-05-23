
QT += core gui xml network widgets multimedia printsupport x11extras

CONFIG += ordered debug_and_release


TARGET = hotshots
TEMPLATE = app

# directories setup
isEmpty( INSTALL_PREFIX )  : INSTALL_PREFIX  = /usr/local

isEmpty( INSTALL_BINDIR )  : INSTALL_BINDIR  = $$INSTALL_PREFIX/bin
isEmpty( INSTALL_LIBDIR )  {
    INSTALL_LIBDIR  = $$INSTALL_PREFIX/lib
    #installing to lib64 on 64-bit platforms:
    contains (QMAKE_HOST.arch, x86_64) : INSTALL_LIBDIR = $$INSTALL_PREFIX/lib64
}

isEmpty( INSTALL_MANDIR )  : INSTALL_MANDIR  = $$INSTALL_PREFIX/share/man
isEmpty( INSTALL_DATADIR ) : INSTALL_DATADIR = $$INSTALL_PREFIX/share

isEmpty( MYAPP_INSTALL_MIMETYPE ) : MYAPP_INSTALL_MIMETYPE = $$INSTALL_DATADIR/mime/packages
isEmpty( MYAPP_INSTALL_DESKTOP ) : MYAPP_INSTALL_DESKTOP = $$INSTALL_DATADIR/applications
isEmpty( MYAPP_INSTALL_PIXMAPS ) : MYAPP_INSTALL_PIXMAPS = $$INSTALL_DATADIR/pixmaps
isEmpty( MYAPP_INSTALL_DATA )    : MYAPP_INSTALL_DATA    = $$INSTALL_DATADIR/$$TARGET
isEmpty( MYAPP_INSTALL_DIR )     : MYAPP_INSTALL_DIR     = $$INSTALL_LIBDIR/$$TARGET
isEmpty( MYAPP_INSTALL_BINDIR )  : MYAPP_INSTALL_BINDIR  = $$INSTALL_BINDIR
isEmpty( MYAPP_INSTALL_MAN )     : MYAPP_INSTALL_MAN     = $$INSTALL_MANDIR/man1
isEmpty( MYAPP_INSTALL_TRANS )   : MYAPP_INSTALL_TRANS   = $$INSTALL_DATADIR/$$TARGET/locale


SOURCES += src/*.cpp\
           src/3rdparty/qkeysequencewidget/qkeysequencewidget.cpp \
           src/3rdparty/qt-json/json.cpp \
           src/3rdparty/QSgml/QSgml.cpp \
           src/3rdparty/QSgml/qsgmlhighliter.cpp \
           src/3rdparty/QSgml/QSgmlTag.cpp \
           src/3rdparty/wwWidgets/qwwtwocolorindicator.cpp \
           src/3rdparty/wwWidgets/wwglobal_p.cpp \
           src/uploaders/*.cpp \
           src/editor/*.cpp \
           src/editor/widgets/*.cpp \
           src/editor/items/*.cpp \
           src/editor/io/*.cpp

HEADERS  += src/*.h \
            src/3rdparty/qkeysequencewidget/qkeysequencewidget.h \
            src/3rdparty/qkeysequencewidget/qkeysequencewidget_p.h \
            src/3rdparty/qt-json/json.h \
            src/3rdparty/QSgml/QSgml.h \
            src/3rdparty/QSgml/qsgmlhighliter.h \
            src/3rdparty/QSgml/QSgmlTag.h \
            src/3rdparty/wwWidgets/qwwtwocolorindicator.h \
            src/3rdparty/wwWidgets/wwglobal.h \
            src/3rdparty/wwWidgets/wwglobal_p.h \
            src/uploaders/*.h \
            src/editor/*.h \
            src/editor/widgets/*.h \
            src/editor/items/*.h \
            src/editor/io/*.h

win32:SOURCES += \
    src/3rdparty/qxt/qxtglobal.cpp \
    src/3rdparty/qxt/qxtglobalshortcut.cpp \
    src/3rdparty/qxt/qxtglobalshortcut_win.cpp \
    src/3rdparty/qxt/qxtwindowsystem.cpp \
    src/3rdparty/qxt/qxtwindowsystem_win.cpp

macx:SOURCES += \
    src/3rdparty/qxt/qxtglobal.cpp \
    src/3rdparty/qxt/qxtglobalshortcut.cpp \
    src/3rdparty/qxt/qxtglobalshortcut_mac.cpp \
    src/3rdparty/qxt/qxtwindowsystem.cpp \
    src/3rdparty/qxt/qxtwindowsystem_mac.cpp
    
!isEmpty(BUNDLE):SOURCES += \
    src/3rdparty/qxt/qxtglobal.cpp \
    src/3rdparty/qxt/qxtglobalshortcut.cpp \
    src/3rdparty/qxt/qxtglobalshortcut_x11.cpp \
    src/3rdparty/qxt/qxtwindowsystem.cpp \
    src/3rdparty/qxt/qxtwindowsystem_x11.cpp

	
win32:HEADERS += \
    src/3rdparty/qxt/qxtglobal.h \
    src/3rdparty/qxt/qxtglobalshortcut.h \
    src/3rdparty/qxt/qxtwindowsystem.h
    

macx:HEADERS += \
    src/3rdparty/qxt/qxtglobal.h \
    src/3rdparty/qxt/qxtglobalshortcut.h \
    src/3rdparty/qxt/qxtwindowsystem.h
    
!isEmpty(BUNDLE):HEADERS += \
    src/3rdparty/qxt/qxtglobal.h \
    src/3rdparty/qxt/qxtglobalshortcut.h \
    src/3rdparty/qxt/qxtwindowsystem.h


FORMS    += ui/AboutDialog.ui \
   ui/DefaultUploaderSettings.ui \
   ui/EditorWidget.ui \
   ui/FtpUploaderSettings.ui \
   ui/MainWindow.ui \
   ui/RescaleDialog.ui \


TRANSLATIONS =  lang/hotshots_fr.ts # french
TRANSLATIONS += lang/hotshots_lt.ts # Lithuanian
TRANSLATIONS += lang/hotshots_it.ts #Italian
TRANSLATIONS += lang/hotshots_ja.ts # Japanese
TRANSLATIONS += lang/hotshots_pl.ts # Polish
TRANSLATIONS += lang/hotshots_sk.ts # Slovak
TRANSLATIONS += lang/hotshots_sr.ts # Serbian
TRANSLATIONS += lang/hotshots_si.ts # Sinhala (Sri Lanka)
TRANSLATIONS += lang/hotshots_tr.ts # Turkish
TRANSLATIONS += lang/hotshots_cs.ts # Czech
TRANSLATIONS += lang/hotshots_ru.ts # Russian
TRANSLATIONS += lang/hotshots_el.ts # Greek
TRANSLATIONS += lang/hotshots_vi.ts # Vietnamese
TRANSLATIONS += lang/hotshots_es.ts # Spain
TRANSLATIONS += lang/hotshots_eu.ts # Basque
TRANSLATIONS += lang/hotshots_zh.ts # Chinese
TRANSLATIONS += lang/hotshots_ro.ts # Romanian
TRANSLATIONS += lang/hotshots_pt.ts # Portuguese
TRANSLATIONS += lang/hotshots_de.ts # German
TRANSLATIONS += lang/hotshots_uk.ts # Ukrainian
TRANSLATIONS += lang/hotshots_gl.ts # Galician


macx:ICON = res/hotshots.icns
macx:QMAKE_INFO_PLIST =  ./Info.plist
macx: INCLUDEPATH += /usr/local/include
macx:QMAKE_POST_LINK += ./postbuild-macx.sh

win32:RC_FILE = res/hotshots.rc

macx:RESOURCES += res/osx/hotshots.qrc
!macx:RESOURCES += res/hotshots.qrc

win32:LIBS += -lgdi32
macx:LIBS += -framework  Carbon
unix:!macx:LIBS+= -lX11 -lXfixes
#isEmpty(BUNDLE):unix:!macx:LIBS+= -lQxtGui


INCLUDEPATH += \
    $${DESTDIR} \
    src/3rdparty/qkeysequencewidget \
    src/3rdparty/QSgml \
    src/3rdparty/wwWidgets \
    src/3rdparty/qt-json \
    src/3rdparty/qxt/ \
    src/uploaders \
    src/editor \
    src/editor/items \
    src/editor/widgets \
    src/editor/io \
    src

win32:INCLUDEPATH += src/3rdparty/qxt
macx:INCLUDEPATH += src/3rdparty/qxt
!isEmpty(BUNDLE):INCLUDEPATH += src/3rdparty/qxt
unix:!macx:INCLUDEPATH += /usr/include/qxt/QxtGui /usr/include/qxt/QxtCore /usr/include/QxtGui /usr/include/QxtCore /usr/local/Qxt/include/QxtGui /usr/local/Qxt/include/QxtCore

win32|os2:DEFINES += QXT_STATIC Q_WW_STATIC

win32-msvc*:DEFINES += _CRT_NONSTDC_NO_DEPRECATE _CRT_SECURE_NO_WARNINGS

Release:DESTDIR = release
Release:OBJECTS_DIR = release
Release:MOC_DIR = release
Release:RCC_DIR = release
Release:UI_DIR = release

Debug:DESTDIR = debug
Debug:OBJECTS_DIR = debug
Debug:MOC_DIR = debug
Debug:RCC_DIR = debug
Debug:UI_DIR = debug

macx {
    # add auto creation of info.plist file
    VERSION = $$system(grep VERSION ../src/AppSettings.h | awk \' { print $NF }\' | sed   \'s/\"//g\')
    createplist.commands = $$PWD/Info.plist.sh $$VERSION > $$PWD/Info.plist
    createplist.target = Info.plist1
    createplist.depends = FORCE
    PRE_TARGETDEPS += Info.plist1
    QMAKE_EXTRA_TARGETS += createplist
}

# .qm file for mingw and before setup.nsi
win32-g++ {
    # add auto compilation of .ts files
    QMAKE_EXTRA_COMPILERS += copyQtTrans
    for(TRANS,TRANSLATIONS){
    lang = $$replace(TRANS, .*_([^/]*)\\.ts, \\1)
    GLOBALTRANSLATIONS += $$[QT_INSTALL_TRANSLATIONS]/qt_$${lang}.qm
    }
    copyQtTrans.input         =  GLOBALTRANSLATIONS
    copyQtTrans.output        = ${QMAKE_FILE_BASE}.qm
    copyQtTrans.commands      = cp ${QMAKE_FILE_IN} ../lang/
    copyQtTrans.CONFIG       += no_link target_predeps
    
    # add auto compilation of .ts files
    QMAKE_EXTRA_COMPILERS += lrelease 
    lrelease.input         = TRANSLATIONS
    lrelease.output        = ${QMAKE_FILE_PATH}/${QMAKE_FILE_BASE}.qm
    lrelease.commands      = $$[QT_INSTALL_BINS]/lrelease ${QMAKE_FILE_IN} -qm ../lang/${QMAKE_FILE_BASE}.qm
    lrelease.CONFIG       += no_link target_predeps
}

unix:!macx {

    # add auto compilation of .ts files
    QMAKE_EXTRA_COMPILERS += lrelease
    lrelease.input         = TRANSLATIONS
    lrelease.output        = ${QMAKE_FILE_BASE}.qm
    lrelease.commands      = $$[QT_INSTALL_BINS]/lrelease ${QMAKE_FILE_IN} -qm ../lang/${QMAKE_FILE_BASE}.qm
    lrelease.CONFIG       += no_link target_predeps

    # to copy executable to /usr/local/bin directory
    starter.files = ./release/hotshots
    starter.path = $$MYAPP_INSTALL_BINDIR

    transl.files = lang/*.qm
    transl.path = $$MYAPP_INSTALL_TRANS
    
    data.files += CREDITS.txt README.txt AUTHORS.txt Changelog.txt
    data.path = $$MYAPP_INSTALL_DATA

    # generate desktop file
    VERSION = $$system(grep VERSION ../src/AppSettings.h | awk \' { print $NF }\' | sed   \'s/\"//g\')
    system( sh  hotshots-desktop.sh  $$INSTALL_PREFIX $$VERSION > hotshots.desktop )
    desktop.files += hotshots.desktop
    desktop.path = $$MYAPP_INSTALL_DESKTOP

    icons.files += res/hotshots.png
    icons.path = $$MYAPP_INSTALL_PIXMAPS
    
    mimetype.files += hotshots.xml
    mimetype.path = $$MYAPP_INSTALL_MIMETYPE
    
    # generate manpage
    system( sh  hotshots-manpage.sh > hotshots.1 )
    system( gzip -9 -f hotshots.1  )
    manual.files += hotshots.1.gz
    manual.path = $$MYAPP_INSTALL_MAN

    INSTALLS += starter transl data desktop icons manual mimetype
}

