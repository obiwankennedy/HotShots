
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

#include "IOHelper.h"

#include "LogHandler.h"

#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>
#include <QtXml/QDomNode>
#include <QTransform>
#include <QImage>
#include <QPixmap>
#include <QVector3D>
#include <QMatrix4x4>
#include <QtCore/QDataStream>
#include <QtCore/QBuffer>

IOHelper::IOHelper(void)
{}

IOHelper::~IOHelper(void)
{}

//------------------------------------------------------------------------------
QDomElement IOHelper::addElement(QDomDocument & doc,
                                 QDomNode &     parentNode,
                                 const QString &nodeTag,
                                 const QString &nodeValue)
{
    QDomElement elmt = doc.createElement( nodeTag );
    parentNode.appendChild( elmt );
    if ( !nodeValue.isNull() )
    {
        QDomText txt = doc.createTextNode( nodeValue );
        elmt.appendChild( txt );
    }
    return elmt;
}

//------------------------------------------------------------------------------
QDomElement IOHelper::addElement(QDomDocument & doc,
                                 QDomNode &     parentNode,
                                 const QString &nodeTag,
                                 int            nodeValue)
{
    QString qNodeValue = QString("%1").arg(nodeValue);
    return addElement(doc, parentNode, nodeTag, qNodeValue);
}

//------------------------------------------------------------------------------
QDomElement IOHelper::addElement(QDomDocument & doc,
                                 QDomNode &     parentNode,
                                 const QString &nodeTag,
                                 unsigned int   nodeValue)
{
    QString qNodeValue = QString("%1").arg(nodeValue);
    return addElement(doc, parentNode, nodeTag, qNodeValue);
}

//------------------------------------------------------------------------------
QDomElement IOHelper::addElement(QDomDocument & doc,
                                 QDomNode &     parentNode,
                                 const QString &nodeTag,
                                 double         nodeValue)
{
    QString qNodeValue = QString("%1").arg(nodeValue);
    return addElement(doc, parentNode, nodeTag, qNodeValue);
}

//------------------------------------------------------------------------------
QDomElement IOHelper::addElement(QDomDocument & doc,
                                 QDomNode &     parentNode,
                                 const QString &nodeTag,
                                 bool           nodeValue)
{
    QString qNodeValue = nodeValue ? QString("true") : QString("false");
    return addElement(doc, parentNode, nodeTag, qNodeValue);
}

//------------------------------------------------------------------------------
void IOHelper::addAttribute(QDomElement &  parentElemt,
                            const QString &attrTag,
                            const QString &attrValue)
{
    parentElemt.setAttribute(attrTag, attrValue);
}

void IOHelper::addComment(QDomDocument & doc,
                          QDomElement &  parentElemt,
                          const QString &comment)
{
    QDomComment qComment = doc.createComment(comment);
    parentElemt.appendChild(qComment);
}

//------------------------------------------------------------------------------
const QString IOHelper::GetNodeFullPath(const QDomNode &node)
{
    QString nodePath;
    if ( node.isNull() )
    {
        nodePath = QString("Null");
    }
    else
    {
        nodePath = node.nodeName();
        QDomNode loopNode = node.parentNode();
        while ( !loopNode.isNull() )
        {
            nodePath = nodePath.prepend("/");
            nodePath = nodePath.prepend( loopNode.nodeName() );
            loopNode = loopNode.parentNode();
        }
    }
    return nodePath;
}

//------------------------------------------------------------------------------
bool IOHelper::stringToBool(const QString &str)
{
    return QString::compare(str,"true",Qt::CaseInsensitive);
}

//------------------------------------------------------------------------------
QVariant IOHelper::stringToVariant(const QString& s, QVariant::Type type, bool &ok)
{
    QVariant result;
    ok = true;

    if ( s.isNull() )
    {
        return QVariant();
    }

    switch (type)
    {
        case QVariant::Invalid:
        {
            ok = false;
            break;
        }
        case QVariant::Pixmap:
        {
            QPixmap img;
            img.loadFromData(s.toLatin1(),"XPM");
            result = img;
            break;
        }
        case QVariant::Image:
        {
            /* Version 1.0
               QImage img;
               img.loadFromData(s.toLatin1(),"XPM");
               result = img;
             */
            QImage img;
            img.loadFromData( QByteArray::fromBase64( s.toLatin1() ) );
            result = img;
            break;
        }
        case QVariant::ByteArray:
        {
            const uint len = s.length();
            QByteArray ba;
            ba.resize(len / 2 + len % 2);
            for (uint i = 0; i < (len - 1); i += 2)
            {
                int c = s.mid(i, 2).toInt(&ok, 16);
                if (!ok)
                {
                    LogHandler::getInstance()->reportError( QObject::tr("stringToVariant(): Error in digit (%1).").arg(i) );
                    break;
                }
                ba[i / 2] = (char)c;
            }
            result = ba;
            break;
        }
        case QVariant::Vector3D:
        {
            QByteArray encodedData = QByteArray::fromBase64( s.toLatin1() );
            QDataStream stream(&encodedData, QIODevice::ReadOnly);
            QVector3D vec;
            stream >> vec;
            result = vec;

            break;
        }
        case QVariant::Matrix4x4:
        {
            QByteArray encodedData = QByteArray::fromBase64( s.toLatin1() );
            QDataStream stream(&encodedData, QIODevice::ReadOnly);
            QMatrix4x4 mat;
            stream >> mat;
            result = mat;
            break;
        }
        case QVariant::Rect:
        {
            QStringList args = s.split(QRegExp("\\s+|x"), QString::SkipEmptyParts);
            if (args.size() == 4)
                result = QVariant( QRect( args[0].toInt(), args[1].toInt(), args[2].toInt(), args[3].toInt() ) );
        }
        case QVariant::RectF:
        {
            QStringList args = s.split(QRegExp("\\s+|x"), QString::SkipEmptyParts);
            if (args.size() == 4)
                result = QVariant( QRectF ( args[0].toDouble(), args[1].toDouble(), args[2].toDouble(), args[3].toDouble() ) );
        }
        case QVariant::Point:
        {
            QStringList args = s.split("x");
            if (args.size() == 2)
                result = QVariant( QPoint( args[0].toInt(), args[1].toInt() ) );
            break;
        }
        case QVariant::PointF:
        {
            QStringList args = s.split("x");
            if (args.size() == 2)
                result = QVariant( QPointF( args[0].toDouble(), args[1].toDouble() ) );
            break;
        }
        default:
        {
            result = QVariant(s);
            if ( !result.convert(type) )
            {
                ok = false;
                result = QVariant();
            }
            break;
        }
    }

    return result;
}

QString IOHelper::variantToString(const QVariant &value)
{
    switch ( value.type() )
    {
        case QVariant::Image:
        case QVariant::Pixmap:
        {
            const QImage img = value.value<QImage>();
            if ( img.isNull() )
                return QObject::tr("<no image>");

            /* version 1.0 => xpm
               QByteArray block;
               QBuffer buffer(&block);
               img.save(&buffer,"XPM");
             */

            // version 1.1 =>
            QByteArray block;
            QBuffer buffer(&block);
            buffer.open(QIODevice::WriteOnly);
            img.save(&buffer, "PNG");
            buffer.close();

            return QString( block.toBase64() );
        }
        case QVariant::Line:
            return QString::fromUtf8("%1 x %2 at %3 x %4")
                       .arg( value.toLine().x1() ).arg( value.toLine().y1() )
                       .arg( value.toLine().x2() ).arg( value.toLine().y2() );

        case QVariant::LineF:
            return QString::fromUtf8("%1 x %2 at %3 x %4")
                       .arg( value.toLineF().x1() ).arg( value.toLineF().y1() )
                       .arg( value.toLineF().x2() ).arg( value.toLineF().y2() );

        case QVariant::Point:
            return QString::fromLatin1("%1x%2").
                       arg( value.toPoint().x() ).
                       arg( value.toPoint().y() );

        case QVariant::PointF:
            return QString::fromLatin1("%1x%2").
                       arg( value.toPointF().x() ).
                       arg( value.toPointF().y() );

        case QVariant::Rect:
            return QString::fromLatin1("%1x%2 %3x%4").
                       arg( value.toRect().x() ).
                       arg( value.toRect().y() ).
                       arg( value.toRect().width() ).
                       arg( value.toRect().height() );

        case QVariant::RectF:
            return QString::fromLatin1("%1x%2 %3x%4").
                       arg( value.toRectF().x() ).
                       arg( value.toRectF().y() ).
                       arg( value.toRectF().width() ).
                       arg( value.toRectF().height() );

        case QVariant::Region:
        {
            const QRegion region = value.value<QRegion>();
            if ( region.isEmpty() )
            {
                return QLatin1String("<empty>");
            }
            if (region.rectCount() == 1)
            {
                return variantToString( region.rects().first() );
            }
            else
            {
                return QString::fromLatin1("<%1 rects>").arg( region.rectCount() );
            }
        }

        case QVariant::Size:
            return QString::fromLatin1("%1x%2").
                       arg( value.toSize().width() ).
                       arg( value.toSize().height() );

        case QVariant::SizeF:
            return QString::fromLatin1("%1x%2").
                       arg( value.toSizeF().width() ).
                       arg( value.toSizeF().height() );

        case QVariant::StringList:
            return value.toStringList().join(", ");

        case QVariant::Transform:
        {
            const QTransform t = value.value<QTransform>();
            return QString::fromLatin1("[%1 %2 %3, %4 %5 %6, %7 %8 %9]").
                       arg( t.m11() ).arg( t.m12() ).arg( t.m13() ).
                       arg( t.m21() ).arg( t.m22() ).arg( t.m23() ).
                       arg( t.m31() ).arg( t.m32() ).arg( t.m33() );
        }
        case QVariant::Vector3D:
        {
            QByteArray encodedData;
            QDataStream stream(&encodedData, QIODevice::WriteOnly);
            stream << value.value<QVector3D>();

            //return QString::fromLatin1("[%1 %2 %3]").arg(t.x()).arg(t.y()).arg(t.z());
            return encodedData.toBase64();
        }
        case QVariant::Matrix4x4:
        {
            QByteArray encodedData;
            QDataStream stream(&encodedData, QIODevice::WriteOnly);
            stream << value.value<QMatrix4x4>();
            return encodedData.toBase64();
        }
        default:
            break;
    }

    return value.toString();
}
