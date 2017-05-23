TARGET = QKeySequnceWidgetPlugin
TEMPLATE = lib
CONFIG += designer \
    plugin \
    debug_and_release
include(../qkeysequencewidget/qkeysequencewidget.pri)

# install
target.path = $$[QT_INSTALL_PLUGINS]/designer
INSTALLS += target
HEADERS += qkeysequencewidgetplugin.h
SOURCES += qkeysequencewidgetplugin.cpp
