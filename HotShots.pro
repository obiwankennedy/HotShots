
QT += core gui xml network widgets multimedia printsupport
win32:QT +=winextras
unix:!macx:QT += x11extras

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


SOURCES += src/3rdparty/qkeysequencewidget/qkeysequencewidget.cpp \
           src/3rdparty/qt-json/json.cpp \
           src/3rdparty/QSgml/QSgml.cpp \
           src/3rdparty/QSgml/qsgmlhighliter.cpp \
           src/3rdparty/QSgml/QSgmlTag.cpp \
           src/3rdparty/wwWidgets/qwwtwocolorindicator.cpp \
           src/3rdparty/wwWidgets/wwglobal_p.cpp \
            src/editor/io/IOHelper.cpp \
            src/editor/io/FileParser.cpp \
    src/editor/items/TagManager.cpp \
    src/editor/items/SimpleTextItem.cpp \
    src/editor/items/HandgripItem.cpp \
    src/editor/items/EditorUndoCommand.cpp \
    src/editor/items/EditorTextItem.cpp \
    src/editor/items/EditorTagItem.cpp \
    src/editor/items/EditorRectangleItem.cpp \
    src/editor/items/EditorPolylineItem.cpp \
    src/editor/items/EditorPolygonItem.cpp \
    src/editor/items/EditorMagnifierItem.cpp \
    src/editor/items/EditorLineItem.cpp \
    src/editor/items/EditorLineArrowItem.cpp \
    src/editor/items/EditorImageItem.cpp \
    src/editor/items/EditorHighlighterItem.cpp \
    src/editor/items/EditorEncircleItem.cpp \
    src/editor/items/EditorEllipseItem.cpp \
    src/editor/items/EditorCurveItem.cpp \
    src/editor/items/EditorCropItem.cpp \
    src/editor/items/EditorBlurItem.cpp \
    src/editor/items/EditorArrowItem.cpp \
    src/editor/items/BaseItem.cpp \
    src/editor/widgets/WidgetLineWidth.cpp \
    src/editor/widgets/WidgetFillStyle.cpp \
    src/editor/widgets/WidgetDashStyle.cpp \
    src/editor/widgets/WidgetColor.cpp \
    src/editor/widgets/WidgetBrush.cpp \
    src/editor/widgets/WidgetBase.cpp \
    src/editor/PaletteWidget.cpp \
    src/editor/PaletteBarWidget.cpp \
    src/editor/ItemRegistering.cpp \
    src/editor/GroupLayer.cpp \
    src/editor/EditorWidget.cpp \
    src/editor/EditorView.cpp \
    src/editor/EditorScene.cpp \
    src/uploaders/UploaderRegistering.cpp \
    src/uploaders/UploaderManager.cpp \
    src/uploaders/ImgurUploader.cpp \
    src/uploaders/ImageshackUploader.cpp \
    src/uploaders/FtpUploaderSettings.cpp \
    src/uploaders/FtpUploader.cpp \
    src/uploaders/FreeImageHostingUploader.cpp \
    src/uploaders/DefaultUploaderSettings.cpp \
    src/uploaders/CanardPCUploader.cpp \
    src/uploaders/BaseUploader.cpp \
    src/WindowGrabber_x11.cpp \
    src/WindowGrabber_win.cpp \
    src/WindowGrabber_os2.cpp \
    src/WindowGrabber_mac.cpp \
    src/SplashScreen.cpp \
    src/SingleApplication.cpp \
    src/RescaleDialog.cpp \
    src/RegionGrabber.cpp \
    src/QColorButton.cpp \
    src/QClickLabel.cpp \
    src/PostEffect.cpp \
    src/PaypalButton.cpp \
    src/NameManager.cpp \
    src/MiscFunctions.cpp \
    src/MainWindow.cpp \
    src/main.cpp \
    src/LogHandler.cpp \
    src/GrabManager.cpp \
    src/FreehandGrabber.cpp \
    src/AppSettings.cpp \
    src/AboutDialog.cpp


HEADERS  += src/3rdparty/qkeysequencewidget/qkeysequencewidget.h \
            src/3rdparty/qkeysequencewidget/qkeysequencewidget_p.h \
            src/3rdparty/qt-json/json.h \
            src/3rdparty/QSgml/QSgml.h \
            src/3rdparty/QSgml/qsgmlhighliter.h \
            src/3rdparty/QSgml/QSgmlTag.h \
            src/3rdparty/wwWidgets/qwwtwocolorindicator.h \
            src/3rdparty/wwWidgets/wwglobal.h \
            src/3rdparty/wwWidgets/wwglobal_p.h \
            src/editor/io/IOHelper.h \
            src/editor/io/FileParser.h \
    src/editor/items/TagManager.h \
    src/editor/items/SimpleTextItem.h \
    src/editor/items/HandgripItem.h \
    src/editor/items/EditorUndoCommand.h \
    src/editor/items/EditorTextItem.h \
    src/editor/items/EditorTagItem.h \
    src/editor/items/EditorRectangleItem.h \
    src/editor/items/EditorPolylineItem.h \
    src/editor/items/EditorPolygonItem.h \
    src/editor/items/EditorMagnifierItem.h \
    src/editor/items/EditorLineItem.h \
    src/editor/items/EditorLineArrowItem.h \
    src/editor/items/EditorImageItem.h \
    src/editor/items/EditorHighlighterItem.h \
    src/editor/items/EditorEncircleItem.h \
    src/editor/items/EditorEllipseItem.h \
    src/editor/items/EditorCurveItem.h \
    src/editor/items/EditorCropItem.h \
    src/editor/items/EditorBlurItem.h \
    src/editor/items/EditorArrowItem.h \
    src/editor/items/BaseItem.h \
    src/editor/widgets/WidgetLineWidth.h \
    src/editor/widgets/WidgetFillStyle.h \
    src/editor/widgets/WidgetDashStyle.h \
    src/editor/widgets/WidgetColor.h \
    src/editor/widgets/WidgetBrush.h \
    src/editor/widgets/WidgetBase.h \
    src/editor/PaletteWidget.h \
    src/editor/PaletteBarWidget.h \
    src/editor/ItemRegistering.h \
    src/editor/ItemFactory.h \
    src/editor/GroupLayer.h \
    src/editor/EditorWidget.h \
    src/editor/EditorView.h \
    src/editor/EditorScene.h \
    src/uploaders/UploaderRegistering.h \
    src/uploaders/UploaderManager.h \
    src/uploaders/ImgurUploader.h \
    src/uploaders/ImageshackUploader.h \
    src/uploaders/FtpUploaderSettings.h \
    src/uploaders/FtpUploader.h \
    src/uploaders/FreeImageHostingUploader.h \
    src/uploaders/DefaultUploaderSettings.h \
    src/uploaders/CanardPCUploader.h \
    src/uploaders/BaseUploader.h \
    src/WindowGrabber.h \
    src/SplashScreen.h \
    src/SingleApplication.h \
    src/RescaleDialog.h \
    src/RegionGrabber.h \
    src/QColorButton.h \
    src/QClickLabel.h \
    src/PostEffect.h \
    src/PaypalButton.h \
    src/NameManager.h \
    src/MiscFunctions.h \
    src/MainWindow.h \
    src/LogHandler.h \
    src/IMessageHandler.h \
    src/GrabManager.h \
    src/FreehandGrabber.h \
    src/AppSettings.h \
    src/AboutDialog.h


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
    VERSION = $$system(grep VERSION src/AppSettings.h | awk \' { print $NF }\' | sed   \'s/\"//g\')
    createplist.commands = $$PWD/Info.plist.sh $$VERSION > $$PWD/Info.plist
    createplist.target = Info.plist1
    createplist.depends = FORCE
    PRE_TARGETDEPS += Info.plist1
    QMAKE_EXTRA_TARGETS += createplist
}

# .qm file for mingw and before setup.nsi
#win32-g++ {
#    # add auto compilation of .ts files
#    QMAKE_EXTRA_COMPILERS += copyQtTrans
#    for(TRANS,TRANSLATIONS){
#    lang = $$replace(TRANS, .*_([^/]*)\\.ts, \\1)
#    GLOBALTRANSLATIONS += $$[QT_INSTALL_TRANSLATIONS]/qt_$${lang}.qm
#    }
#    copyQtTrans.input         =  GLOBALTRANSLATIONS
#    copyQtTrans.output        = ${QMAKE_FILE_BASE}.qm
#    copyQtTrans.commands      = cp ${QMAKE_FILE_IN} lang/
#    copyQtTrans.CONFIG       += no_link target_predeps
    
#    # add auto compilation of .ts files
#    QMAKE_EXTRA_COMPILERS += lrelease
#    lrelease.input         = TRANSLATIONS
#    lrelease.output        = ${QMAKE_FILE_PATH}/${QMAKE_FILE_BASE}.qm
#    lrelease.commands      = $$[QT_INSTALL_BINS]/lrelease ${QMAKE_FILE_IN} -qm lang/${QMAKE_FILE_BASE}.qm
#    lrelease.CONFIG       += no_link target_predeps
#}

#unix:!macx {

#    # add auto compilation of .ts files
#    QMAKE_EXTRA_COMPILERS += lrelease
#    lrelease.input         = TRANSLATIONS
#    lrelease.output        = ${QMAKE_FILE_BASE}.qm
#    lrelease.commands      = $$[QT_INSTALL_BINS]/lrelease ${QMAKE_FILE_IN} -qm lang/${QMAKE_FILE_BASE}.qm
#    lrelease.CONFIG       += no_link target_predeps

#    # to copy executable to /usr/local/bin directory
#    starter.files = ./release/hotshots
#    starter.path = $$MYAPP_INSTALL_BINDIR

#    transl.files = lang/*.qm
#    transl.path = $$MYAPP_INSTALL_TRANS
    
#    data.files += CREDITS.txt README.txt AUTHORS.txt Changelog.txt
#    data.path = $$MYAPP_INSTALL_DATA

#    # generate desktop file
#    VERSION = $$system(grep VERSION src/AppSettings.h | awk \' { print $NF }\' | sed   \'s/\"//g\')
#    system( sh  build/hotshots-desktop.sh  $$INSTALL_PREFIX $$VERSION > hotshots.desktop )
#    desktop.files += hotshots.desktop
#    desktop.path = $$MYAPP_INSTALL_DESKTOP

#    icons.files += res/hotshots.png
#    icons.path = $$MYAPP_INSTALL_PIXMAPS
    
#    mimetype.files += hotshots.xml
#    mimetype.path = $$MYAPP_INSTALL_MIMETYPE
    
#    # generate manpage
#    system( sh  build/hotshots-manpage.sh > hotshots.1 )
#    system( gzip -9 -f hotshots.1  )
#    manual.files += hotshots.1.gz
#    manual.path = $$MYAPP_INSTALL_MAN

#    INSTALLS += starter transl data desktop icons manual mimetype
#}

