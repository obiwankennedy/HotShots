
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

#ifndef _IOHELPER_H_
#define _IOHELPER_H_

#include <QtCore/QString>
#include <QtCore/QVariant>

// Forward declarations
class QDomDocument;
class QDomElement;
class QDomNode;

/**
 * IOHelper class provides low level static methods for serialization.
 */
class IOHelper
{
private:

    IOHelper(void);
    virtual ~IOHelper(void);

public:

    static QDomElement addElement(QDomDocument &doc, QDomNode &parentNode, const QString &nodeTag, const QString &nodeValue = QString::null);
    static QDomElement addElement(QDomDocument &doc, QDomNode &parentNode, const QString &nodeTag, int nodeValue);
    static QDomElement addElement(QDomDocument &doc, QDomNode &parentNode, const QString &nodeTag, unsigned int nodeValue);
    static QDomElement addElement(QDomDocument &doc, QDomNode &parentNode, const QString &nodeTag, double nodeValue);
    static QDomElement addElement(QDomDocument &doc, QDomNode &parentNode, const QString &nodeTag, bool nodeValue);
    static void addAttribute(QDomElement &parentElemt, const QString &attrTag, const QString &attrValue);
    static void addComment(QDomDocument &doc, QDomElement &parentElemt, const QString &comment);
    static const QString GetNodeFullPath(const QDomNode &node);
    static bool stringToBool(const QString &str);
    static QString variantToString(const QVariant &value);
    static QVariant stringToVariant(const QString &value, QVariant::Type type, bool &ok);
};

#endif // _IOHELPER_H_
