/******************************************************************************
   HotShots: Screenshot utility
   Copyright(C) 2011-2014  xbee@xbee.net
   2017-2018

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *******************************************************************************/

#include <QtCore/QTextStream>

#include <QApplication>

#include <QDesktopWidget>
#include <QMessageBox>
#include <QCommandLineOption>
#include <QCommandLineParser>

#include "MainWindow.h"
#include "SingleApplication.h"
#include "AppSettings.h"
#include "MiscFunctions.h"
#include "SplashScreen.h"

#ifdef Q_OS_WIN
#include <windows.h> // for Sleep
#endif

#ifdef Q_OS_UNIX
#include <time.h>
#endif

// extract from QTest ...
void qSleep(int ms)
{
#ifdef Q_OS_WIN
    Sleep( uint(ms) );
#else
    struct timespec ts = {
        ms / 1000, (ms % 1000) * 1000 * 1000
    };
    nanosleep(&ts, NULL);
#endif
}



int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(hotshots);

    qSleep(1000); // waiting sometimes for restart case ...

    SingleApplication app(argc, argv, PACKAGE_NAME);
    app.setQuitOnLastWindowClosed(false); // because of systray
    app.setApplicationVersion(QStringLiteral("2.2.0"));

    // add possible image plugin path
#ifdef Q_OS_MAC
    app.addLibraryPath(QApplication::applicationDirPath() + "/../plugins");
#endif
    app.addLibraryPath( QApplication::applicationDirPath() + "./plugins");

    // update some application infos (use by some platform for temp storage, ...)
    MiscFunctions::updateApplicationIdentity();

    // set default language for application (computed or saved)
    MiscFunctions::setDefaultLanguage();

    // check for special argument
    bool forceResetConfig = false;
    bool ignoreSingleInstance = false;
    bool editorOnly = false;
    QCommandLineOption resetOpt(QStringList() << "r" << "reset-config", "clear the saved preference parameters");
    QCommandLineOption portablOpt(QStringList() << "p" << "portable", "clear the saved preference parameters");
    QCommandLineOption nosingleOpt(QStringList() << "n" << "no-singleinstance", "clear the saved preference parameters");
    QCommandLineOption fileOpt(QStringList() << "f" << "file", "clear the saved preference parameters");
    QCommandLineOption editorOpt(QStringList() << "e" << "edit", "Start editor only");


    QString fileToLoad;
    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addOption(resetOpt);
    parser.addOption(portablOpt);
    parser.addOption(nosingleOpt);
    parser.addOption(editorOpt);
    parser.addOption(fileOpt);

    parser.parse(app.arguments());

    if(parser.isSet(resetOpt))
    {
        forceResetConfig = true;
    }
    else if(parser.isSet(portablOpt))
    {
        QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, QApplication::applicationDirPath());
        QSettings::setPath(QSettings::IniFormat, QSettings::SystemScope, QApplication::applicationDirPath());
        QSettings::setDefaultFormat(QSettings::IniFormat);
    }
    else if(parser.isSet(nosingleOpt))
    {
        ignoreSingleInstance = true;
    }
    else if(parser.isSet(editorOpt))
    {
        editorOnly = true;
        ignoreSingleInstance = true;
    }
    else if(parser.isSet(fileOpt))
    {
        fileToLoad = parser.value(fileOpt);
        ignoreSingleInstance = true;
    }

    // check for multiple instance of program
    if (app.isRunning() && !ignoreSingleInstance)
    {
        app.sendMessage( fileToLoad );
        if ( fileToLoad.isEmpty() )
            QMessageBox::critical( 0,PACKAGE_NAME,QObject::tr("%1 is already running!!").arg(PACKAGE_NAME) );
        return 0;
    }

    AppSettings settings;

    // in order to display splashscreen on the same screen than application ...
    settings.beginGroup("MainWindow");
    int screenNumber = ( settings.value( "screenNumber",QApplication::desktop()->primaryScreen() ).toInt() ) % ( QApplication::desktop()->screenCount() ); // in order to be sure to display on a right screen
    settings.endGroup();

    settings.beginGroup("Application");

    // reset the saved configuration if needed
    bool resetConfig = settings.value("resetConfig",false).toBool();
    if (resetConfig || forceResetConfig)
    {
        settings.clear();
        settings.sync();
    }

    bool splashscreenAtStartup = settings.value("splashscreenAtStartup",true).toBool();
    bool splashscreenTransparentBackground = settings.value("splashscreenTransparentBackground",true).toBool();
    bool startInTray = settings.value("startInTray",false).toBool();

    settings.endGroup();

    SplashScreen *sScreen = NULL;

    // splash screen
    if (splashscreenAtStartup)
    {
        sScreen = new SplashScreen(QPixmap(":/hotshots/splashscreen.png"), 3000, screenNumber,splashscreenTransparentBackground);
        sScreen->show();
    }

    MainWindow w;
    QObject::connect( &app, SIGNAL( messageAvailable(const QString &) ), &w, SLOT( wakeUp(const QString &) ) );

    // splash screen
    if (splashscreenAtStartup)
    {
        if (!startInTray)
            sScreen->delayedFinish(&w);
    }

    {
#ifdef _DEBUG // ugly hack for debugging
        if (startInTray)
            w.show();
#endif
        if (!startInTray)
            w.show();
    }

    if ( !fileToLoad.isEmpty() )
        w.openEditor(fileToLoad);

    return app.exec();
}
