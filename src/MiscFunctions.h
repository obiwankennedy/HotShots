
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

#ifndef _MISCFUNCTIONS_H_
#define _MISCFUNCTIONS_H_

#include <QtCore/QString>

class MiscFunctions
{
public:

    // give a valid directory for temporary files
    static QString getValidStorage();

    // images function
    static QString getAvailablesImageFormats();
    static QString getAvailablesImageFormatsForWriting();
    static QStringList getAvailablesImageFormatsList();
    static QStringList getAvailablesImageFormatsListForWriting();
    static QMap<QString, QString> getLongImageFormats();

    // translations functions
    static QString getTranslationsPath( const QString &refLang = QString() );
    static QString getTranslationsFile(const QString &lang);
    static QMap<QString, QString> getAvailableLanguages();
    static void setDefaultLanguage();
    static void setLanguage(const QString& lang);

    static void updateApplicationIdentity();
    static int random(int max);
    static int random(int min, int max);

    static QString base64Encode(const QString &str);
    static QString base64Decode(const QString &str);
    static QString HTMLToText(const QString &t );
    static QString TextToHTML(const QString &t );

    static QPixmap applyAlpha( const QPixmap &, const QColor &bgColor = QColor(Qt::white) );

    static void setRunOnStartup(bool runOnStartup);
};

#endif // _MISCFUNCTIONS_H_
