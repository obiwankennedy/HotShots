
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

#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QLibraryInfo>
#include <QtCore/QLocale>
#include <QtCore/QTranslator>

#include <QApplication>
#include <QImage>
#include <QImageReader>
#include <QImageWriter>
#include <QPainter>
#include <QPixmap>
#include <QStandardPaths>

#include "AppSettings.h"
#include "LogHandler.h"
#include "MiscFunctions.h"

QString MiscFunctions::getValidStorage()
{
#ifdef Q_OS_UNIX // on unix => write to the user home
    return QDir::homePath() + "/." + PACKAGE_NAME;

#else // other case
    return QStandardPaths::writableLocation(QStandardPaths::DataLocation);

#endif
}

QStringList MiscFunctions::getAvailablesImageFormatsListForWriting()
{
    QList<QByteArray> supportedFormats= QImageWriter::supportedImageFormats();
    QStringList result;
    QListIterator<QByteArray> supportedFormat(supportedFormats);
    QByteArray format;

    while(supportedFormat.hasNext())
    {
        format= supportedFormat.next().toLower();

        if(!result.contains(format))
            result << format;
    }

    return result;
}

QStringList MiscFunctions::getAvailablesImageFormatsList()
{
    QList<QByteArray> supportedFormats= QImageReader::supportedImageFormats();
    QStringList result;
    QListIterator<QByteArray> supportedFormat(supportedFormats);
    QByteArray format;

    while(supportedFormat.hasNext())
    {
        format= supportedFormat.next().toLower();

        if(!result.contains(format))
            result << format;
    }

    return result;
}

/*
 * Extract from Wally - Qt4 wallpaper/background changer
 * Copyright (C) 2009  Antonio Di Monaco <tony@becrux.com>
 */

QHash<QString, QString> MiscFunctions::getLongImageFormats()
{
    QHash<QString, QString> formats;

    /*
       BMP files [reading, writing]
       Dr. Halo CUT files [reading] *
       DDS files [reading]
       EXR files [reading, writing]
       Raw Fax G3 files [reading]
       GIF files [reading, writing]
       HDR files [reading, writing]
       ICO files [reading, writing]
       IFF files [reading]
       JBIG [reading, writing] **
       JNG files [reading]
       JPEG/JIF files [reading, writing]
       JPEG-2000 File Format [reading, writing]
       JPEG-2000 codestream [reading, writing]
       KOALA files [reading]
       Kodak PhotoCD files [reading]
       MNG files [reading]
       PCX files [reading]
       PBM/PGM/PPM files [reading, writing]
       PFM files [reading, writing]
       PNG files [reading, writing]
       Macintosh PICT files [reading]
       Photoshop PSD files [reading]
       RAW camera files [reading]
       Sun RAS files [reading]
       SGI files [reading]
       TARGA files [reading, writing]
       TIFF files [reading, writing]
       WBMP files [reading, writing]
       XBM files [reading]
       XPM files [reading, writing]
     */

    formats["bw"]= "Black & White";
    formats["eps"]= "Encapsulated Postscript";
    formats["epsf"]= "Encapsulated PostScript";
    formats["epsi"]= "Encapsulated PostScript Interchange";
    formats["exr"]= "OpenEXR";
    formats["pcx"]= "PC Paintbrush Exchange";
    formats["psd"]= "Photoshop Document";
    formats["rgb"]= "Raw red, green, and blue samples";
    formats["rgba"]= "Raw red, green, blue, and alpha samples";
    formats["sgi"]= "Irix RGB";
    formats["tga"]= "Truevision Targa";
    formats["xcf"]= "eXperimental Computing Facility (GIMP)";
    formats["dds"]= "DirectDraw Surface";
    formats["xv"]= "Khoros Visualization";
    formats["bmp"]= "Windows Bitmap";
    formats["gif"]= "Graphic Interchange Format";
    formats["jpg"]= "Joint Photographic Experts Group";
    formats["jpeg"]= "Joint Photographic Experts Group";
    formats["jp2"]= "Joint Photographic Experts Group 2000";
    formats["mng"]= "Multiple-image Network Graphics";
    formats["png"]= "Portable Network Graphics";
    formats["pbm"]= "Portable Bitmap";
    formats["pgm"]= "Portable Graymap";
    formats["ppm"]= "Portable Pixmap";
    formats["tiff"]= "Tagged Image File Format";
    formats["tif"]= "Tagged Image File Format";
    formats["xbm"]= "X11 Bitmap";
    formats["xpm"]= "X11 Pixmap";
    formats["ico"]= "Icon Image";
    formats["svg"]= "Scalable Vector Graphics";

    return formats;
}

QString MiscFunctions::getAvailablesImageFormatsForWriting()
{
    QString imglist;
    QStringList formats= getAvailablesImageFormatsListForWriting();

    for(auto const& format : qAsConst(formats))
        imglist+= QStringLiteral("Images %1 (*.%1);;").arg(format);

    qDebug() << "imglist getAvailablesImageFormatsForWriting" << imglist;
    return imglist;
}

QString MiscFunctions::getAvailablesImageFormats()
{
    QString imglist= "Images (";
    QStringList formats= getAvailablesImageFormatsList();
    for(int i= 0; i < formats.size(); ++i)
        imglist+= " *." + formats[i];

    imglist.append(");;");

    qDebug() << "imglist" << imglist;
    return imglist;
}

QString MiscFunctions::getTranslationsFile(const QString& lang)
{
    return QString("%1_%2.qm").arg(PACKAGE_NAME).arg(lang).toLower();
}

QString MiscFunctions::getTranslationsPath(const QString& refLang)
{
    // search in application path
    QStringList ldir;
    ldir << QCoreApplication::applicationDirPath() + "/lang";
    ldir << QCoreApplication::applicationDirPath() + "/../lang";
    ldir << QCoreApplication::applicationDirPath() + "/../../lang";
    ldir << QCoreApplication::applicationDirPath() + "/../translations"; // MacOSX
    ldir << QString("/usr/share/%1/locale").arg(PACKAGE_NAME).toLower();
    ldir << QString("/usr/local/share/%1/locale").arg(PACKAGE_NAME).toLower();
    ldir << "/usr/local/share/locale";
    ldir << "/usr/share/local/locale";
    ldir << "/usr/share/locale";

    QString ext;
    if(!refLang.isEmpty())
        ext= "/" + getTranslationsFile(refLang);

    foreach(const QString& dir, ldir)
    {
        if(QFileInfo(dir + ext).exists())
            return dir;
    }

    return "";
}

QMap<QString, QString> MiscFunctions::getAvailableLanguages()
{
    QMap<QString, QString> languageMap;
    QDir dir(MiscFunctions::getTranslationsPath("fr"));
    QRegExp expr(QString("^%1_(\\w+)\\.qm$").arg(PACKAGE_NAME).toLower());
    QStringList files= dir.entryList(QDir::Files, QDir::Name);

    LogHandler::getInstance()->reportDebug(QObject::tr("Translations path %1").arg(dir.path()));

    foreach(const QString& file, files)
    {
        if(!file.contains(expr))
            continue;

        QString lang= expr.cap(1);
        QString name= QString("%1 (%2)").arg(QLocale::languageToString(QLocale(lang).language()), lang);

        if(lang.contains("_")) // detect variant
        {
            QString country= QLocale::countryToString(QLocale(lang).country());
            name= QString("%1 (%2) (%3)").arg(QLocale::languageToString(QLocale(lang).language()), country, lang);
        }

        languageMap[name]= lang;
    }
    return languageMap;
}

void MiscFunctions::setDefaultLanguage()
{
    QString lang;
    AppSettings settings;

    settings.beginGroup("Application");
    lang= settings.value("currentLanguage", "auto").toString();
    settings.endGroup();

    if(lang.isEmpty())
        lang= QLocale::system().name().left(2);

    if(!lang.isEmpty())
        setLanguage(lang);
}

void MiscFunctions::setLanguage(const QString& lang)
{
    QString language(lang);
    LogHandler::getInstance()->reportDebug(QObject::tr("Setting language to: %1").arg(language));

    // special cases
    if(language == "auto") // auto detection
    {
        language= QLocale::system().name().left(2);
    }
    else if(language == "default") // no use of translator
    {
        return;
    }

    // try load the qt translator for selected language
    QTranslator* qt= new QTranslator();
    QStringList excludedFiles;
    QString globalTranslationPath= QLibraryInfo::location(QLibraryInfo::TranslationsPath);
    if(!QFileInfo(globalTranslationPath).exists())
        globalTranslationPath= MiscFunctions::getTranslationsPath("fr");

    if(qt->load("qt_" + language, globalTranslationPath))
    {
        LogHandler::getInstance()->reportDebug(
            QObject::tr("Successfully loaded data from %1").arg(globalTranslationPath + "/qt_" + language));
        QCoreApplication::installTranslator(qt);
        excludedFiles << "qt_" + language + ".qm";
    }
    else
    {
        delete qt;
    }

    QString suff= language + ".qm";
    QDir dir(MiscFunctions::getTranslationsPath("fr"));

    foreach(const QString& s, dir.entryList(QDir::Files | QDir::Readable))
    {
        if(!s.endsWith(suff) || excludedFiles.contains(s))
            continue;
        QTranslator* t= new QTranslator();

        if(t->load(dir.filePath(s)))
        {
            QCoreApplication::installTranslator(t);
            LogHandler::getInstance()->reportDebug(
                QObject::tr("Successfully loaded data from %1").arg(dir.filePath(s)));
        }
        else
        {
            delete t;
            LogHandler::getInstance()->reportDebug(QObject::tr("Failed to load data from %1").arg(dir.filePath(s)));
        }
    }
}

void MiscFunctions::updateApplicationIdentity()
{
    QCoreApplication::setApplicationName(PACKAGE_NAME);
    QCoreApplication::setApplicationVersion(PACKAGE_VERSION);
    QCoreApplication::setOrganizationName(PACKAGE_ORGANIZATION);
}

int MiscFunctions::random(int min, int max)
{
    return qrand() % ((max + 1) - min) + min;
}

int MiscFunctions::random(int max)
{
    return qrand() % (max + 1);
}

QString MiscFunctions::base64Encode(const QString& str)
{
    QByteArray ba;
    ba.append(str);
    return ba.toBase64();
}

QString MiscFunctions::base64Decode(const QString& str)
{
    QByteArray ba;
    ba.append(str);
    return QByteArray::fromBase64(ba);
}

QString MiscFunctions::HTMLToText(const QString& t)
{
    QString text= t;

    text.replace("&quot;", "\"");
    text.replace("&apos;", "'");
    text.replace("&lt;", "<");
    text.replace("&gt;", ">");
    text.replace("&#10;", "\n");
    text.replace("&#13;", "\r");
    text.replace("&amp;", "&");
    return text;
}

QString MiscFunctions::TextToHTML(const QString& t)
{
    QString text= t;

    text.replace("\"", "&quot;");
    text.replace("'", "&apos;");
    text.replace("<", "&lt;");
    text.replace(">", "&gt;");
    text.replace("\n", "&#10;");
    text.replace("\r", "&#13;");
    text.replace("&", "&amp;");
    return text;
}

QPixmap MiscFunctions::applyAlpha(const QPixmap& pix, const QColor& bgColor)
{
    QPixmap newPix(pix.size());
    newPix.fill(bgColor);
    QPainter painter(&newPix);
    painter.drawPixmap(0, 0, pix);
    return newPix;
}

void MiscFunctions::setRunOnStartup(bool runOnStartup)
{
#ifdef Q_OS_WIN

    QSettings regSettings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run",
                          QSettings::NativeFormat);

    if(runOnStartup)
        regSettings.setValue(PACKAGE_NAME, QDir::toNativeSeparators(QCoreApplication::applicationFilePath()));
    else
        regSettings.remove(PACKAGE_NAME);
#endif
#ifdef Q_OS_LINUX
    QString desktopFileContents= QString("[Desktop Entry]\n"
                                         "Name=%1\n"
                                         "Comment=%2\n"
                                         "Icon=%3\n"
                                         "Type=Application\n"
                                         "Exec=%4\n"
                                         "Hidden=false\n"
                                         "NoDisplay=false\n"
                                         "X-GNOME-Autostart-enabled=true\n")
                                     .arg(PACKAGE_NAME)
                                     .arg(PACKAGE_DESCRIPTION)
                                     .arg(QString(PACKAGE_NAME).toLower())
                                     .arg(QDir::toNativeSeparators(QCoreApplication::applicationFilePath()));

    QString autostartLocation;
    char* xgdConfigHome= getenv("XDG_CONFIG_HOME");
    if(xgdConfigHome != NULL)
        autostartLocation= QString(QString(xgdConfigHome) + "/.config/autostart");
    else
        autostartLocation= QString(QDir::homePath() + "/.config/autostart");

    QDir d;
    d.mkpath(autostartLocation);
    QFile autostartFile(autostartLocation + QString("/%1.desktop").arg(QString(PACKAGE_NAME).toLower()));
    if(runOnStartup)
    {
        if(!autostartFile.exists())
        {
            qDebug() << "Installing autostart file to " << autostartLocation;
            autostartFile.open(QFile::WriteOnly);
            autostartFile.write(desktopFileContents.toLocal8Bit());
            autostartFile.close();
        }
        autostartFile.setPermissions(QFile::ReadOwner | QFile::WriteOwner | QFile::ExeOwner | QFile::ReadGroup
                                     | QFile::WriteGroup | QFile::ReadOther);
    }
    else
    {
        if(autostartFile.exists())
        {
            qDebug() << "Removing autostart file " << autostartFile.fileName();
            autostartFile.remove();
        }
    }
#endif
}
