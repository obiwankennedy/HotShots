
/******************************************************************************
   HotShots: Screenshot utility
   Copyright(C) 2011-2014  xbee@xbee.net

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
//#include <QPlastiqueStyle>
#include <QDesktopWidget>
#include <QMessageBox>

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

void usage()
{
    QTextStream out(stdout);
    out << endl;
    out << QObject::tr("Screenshot management and annotation tool") << endl;
    out << QObject::tr("Usage: ") << QCoreApplication::arguments().at(0) << " " << QObject::tr("[options] [file]") << endl;
    out << endl;
    out << QObject::tr("Following options are available:") << endl;
    out << QObject::tr(" --help                 : displays this help.") << endl;
    out << QObject::tr(" --reset-config         : clear the saved preference parameters.") << endl;
    out << QObject::tr(" --no-singleinstance    : enable the use of multiple instance of program (not recommended).") << endl;
    out << QObject::tr(" --portable             : use settings file location near the executable (for portable use).") << endl;
    out << QObject::tr(" file                   : file to load in the editor, can be a .hot file or an image file.") << endl;
    out << endl;
}

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(hotshots);

    qSleep(1000); // waiting sometimes for restart case ...

    SingleApplication app(argc, argv, PACKAGE_NAME);
    app.setQuitOnLastWindowClosed(false); // because of systray

    // add possible image plugin path
#ifdef Q_OS_MAC
    app.addLibraryPath(QApplication::applicationDirPath() + "/../plugins");
#endif
    app.addLibraryPath( QApplication::applicationDirPath() + "./plugins");

    // update some application infos (use by some platform for temp storage, ...)
    MiscFunctions::updateApplicationIdentity();

    // set default language for application (computed or saved)
    MiscFunctions::setDefaultLanguage();

    // modifying base look (for mac it's a workaround strange layout result ????)
/*#if defined(Q_OS_WIN) || defined(Q_OS_MAC)
    QApplication::setStyle(new QPlastiqueStyle);
#endif*/

    // check for special argument
    bool forceResetConfig = false;
    bool ignoreSingleInstance = false;
    QStringList args = QApplication::arguments();
    QString fileToLoad;
    for ( int i = 1; i < args.count(); ++i )
    {
        const QString arg = args.at(i);

        if ( arg == "--reset-config" )
        {
            forceResetConfig = true;
        }
        else if ( arg == "--no-singleinstance" )
        {
            ignoreSingleInstance = true;
        }
        else if (arg == "--help")
        {
            usage();
            return 0;
        }
        else if(arg == "--portable")
        {
            QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, QApplication::applicationDirPath());
            QSettings::setPath(QSettings::IniFormat, QSettings::SystemScope, QApplication::applicationDirPath());
            QSettings::setDefaultFormat(QSettings::IniFormat);
        }
        else
        {
            fileToLoad = arg;
        }
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
