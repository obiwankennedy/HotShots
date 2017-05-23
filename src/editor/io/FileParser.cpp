
/******************************************************************************
   HotShot: Screenshot utility
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

#include <QtCore/QDir>
#include <QtCore/QTemporaryFile>
#include <QtCore/QTextStream>
#include <QtCore/QFileInfo>
#include <QtCore/QRegExp>
#include <QtCore/QtDebug>

#include <QtXml/QDomNode>

#include "FileParser.h"
#include "EditorScene.h"
#include "BaseItem.h"
#include "ItemFactory.h"
#include "LogHandler.h"
#include "IOHelper.h"

const QString sTagProjectRoot = "HotShotsData";
const QString currentFormatVersion = "1.2";
const QString sTagScene = "scene";
const QString sTagBackground = "background";
const QString sTagItems = "items";
const QString sTagItem = "item";

const QString sTagAttName = "name";
const QString sTagAttRect = "rect";
const QString sTagAttData = "data";
const QString sTagAttType = "type";
const QString sTagAttVersion = "version";

//------------------------------------------------------------------------------
FileParser::FileParser()
{}

//------------------------------------------------------------------------------
FileParser::~FileParser()
{
}

bool FileParser::load(const QString &filePath, EditorScene *scn)
{
    m_file.setFileName(filePath);
    if ( !m_file.open(QFile::ReadOnly | QFile::Text) )
    {
        LogHandler::getInstance()->reportError( tr("Cannot read file %1: (%2)").arg(filePath).arg( m_file.errorString() ) );
        return false;
    }

    bool loaded = read(&m_file, scn);
    m_file.close();

    scn->setMode("Select");

    return loaded;
}

bool FileParser::save(const QString &filePath, const EditorScene *scn)
{
    if (!scn)
        return false;

    QFile fileToWrite(filePath);
    bool success = fileToWrite.open(QFile::ReadWrite | QFile::Truncate | QFile::Text);

    if ( success )
    {
        // 1. Create DomDocument from model
        QDomDocument doc;
        fillDomDoc(doc, scn);

        // 2. Save the DOM
        QTextStream textStream(&fileToWrite);
        textStream.setCodec("ISO 8859-1");
        const int IndentSize = 4;
        doc.save(textStream, IndentSize, QDomNode::EncodingFromDocument);

        fileToWrite.close();

        if ( !success )
        {
            // Failed : something wrong in model
            LogHandler::getInstance()->reportError( tr("Saving failed for project %1: %2").arg(filePath).arg( fileToWrite.errorString() ) );
        }
    }
    else
    {
        // Failed : IO error
        LogHandler::getInstance()->reportError( tr("Cannot save file %1: (%2)").arg(filePath).arg( fileToWrite.errorString() ) );
    }

    return success;
}

bool FileParser::read(QIODevice *device, EditorScene *scn)
{
    QString errorStr;
    int errorLine;
    int errorColumn;

    bool domSet = m_domDocument.setContent(device, false, &errorStr, &errorLine, &errorColumn);
    if ( !domSet )
    {
        LogHandler::getInstance()->reportError( tr("Parse error at line %1, column %2: (%3)").arg(errorLine).arg(errorColumn).arg(errorStr) );
        return false;
    }

    QDomElement root = m_domDocument.documentElement();
    if ( root.tagName() != sTagProjectRoot )
    {
        LogHandler::getInstance()->reportError( tr("The file is not a \"Hotshots\" file!") );
        return false;
    }

    if ( updateVersion() )
        LogHandler::getInstance()->reportInfo( tr("Configuration has been automatically updated!") );

    parse( m_domDocument.firstChildElement(), scn );

    return true;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void FileParser::parse(const QDomNode &node, EditorScene *scn)
{
    if ( node.isNull() )
        return;

    if ( node.hasChildNodes() )
    {
        //qDebug( "Parsing file %s : %d children", node.nodeName().toStdString().c_str(), node.childNodes().count() );

        QDomElement scnNode = node.firstChildElement(sTagScene);
        QDomElement bgNode = scnNode.firstChildElement(sTagBackground);
        QDomElement bgItems = scnNode.firstChildElement(sTagItems);

        if ( !bgNode.isNull() )
        {
            //qDebug() << "bgNode !!";
            bool ok = true;
            QImage image( IOHelper::stringToVariant(bgNode.text(),QVariant::Image,ok).value<QImage>() );
            QPointF pos = IOHelper::stringToVariant(bgNode.attribute("pos"),QVariant::PointF,ok).toPointF();
            scn->setUnderlayImage(image,pos);
        }

        if ( !scnNode.isNull() )
        {
            bool ok = true;
            QRectF rect = IOHelper::stringToVariant(scnNode.attributeNode(sTagAttRect).value(),QVariant::RectF,ok).toRectF();
            scn->setSceneRect(rect);
        }

        if ( !bgItems.isNull() )
        {
            for( QDomNode n = bgItems.firstChild(); !n.isNull(); n = n.nextSibling() )
            {
                //qDebug() << n.toElement().tagName();
                BaseItem *item = scn->getItemFactory()->create( n.toElement().tagName() );
                item->fromXml( n.toElement() );
                scn->addOverlayItem(item);
            }
        }
    }
    else
    {
        //qDebug( "Parsing data source %s : no children nodes", node.nodeName().toStdString().c_str() );
    }
}

//------------------------------------------------------------------------------
bool FileParser::updateVersion()
{
    bool updated = false;
    QDomElement root = m_domDocument.documentElement();
    QString version = root.attribute(sTagAttVersion);

    if ( version.toFloat() > currentFormatVersion.toFloat() )
    {
        LogHandler::getInstance()->reportInfo( tr("Unknown version of hotshots file (%1).").arg(version) );
    }

    // TODO: do something if older release detected !!!
    if (version == "1.0")
    {
        // image format has changed !!
        QDomElement bgNode = root.firstChildElement().firstChildElement(sTagBackground);
        if ( !bgNode.isNull() )
        {
            QImage img;
            img.loadFromData(bgNode.text().toLatin1(),"XPM");
            QDomText t = bgNode.firstChild().toText();
            if ( !t.isNull() )
                t.setData( IOHelper::variantToString( img ) );
        }
        updated = true;
    }

    return updated;
}

//------------------------------------------------------------------------------

void FileParser::fillDomDoc(QDomDocument& doc, const EditorScene *scn)
{
    // File header
    QDomProcessingInstruction instr = doc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"utf-8\"");
    doc.appendChild(instr);

    // Root node
    QDomElement rootNode = IOHelper::addElement(doc, doc, sTagProjectRoot);
    rootNode.setAttribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
    rootNode.setAttribute("xmlns:xsd", "http://www.w3.org/2001/XMLSchema");
    rootNode.setAttribute(sTagAttVersion, currentFormatVersion);

    // scene data
    QDomElement sceneNode = IOHelper::addElement(doc, rootNode, sTagScene);

    // scene rect
    sceneNode.setAttribute( sTagAttRect,IOHelper::variantToString( scn->sceneRect() ) );

    // background
    QDomElement bgNode = IOHelper::addElement(doc, sceneNode, sTagBackground);
    QDomText newNodeText = doc.createTextNode( IOHelper::variantToString( scn->getUnderlayImage() ) );
    bgNode.setAttribute( "pos", IOHelper::variantToString( scn->getUnderlayOffset() ) );
    bgNode.appendChild(newNodeText);

    QDomElement itemsNode = IOHelper::addElement(doc, sceneNode, sTagItems);

    QList<QGraphicsItem *> lisItem( scn->getOverlayItems() );
    foreach (QGraphicsItem * i, lisItem)
    {
        BaseItem *item = dynamic_cast<BaseItem *>(i);
        if (item)
        {
            QDomElement itemNode = doc.createElement( item->getType() );
            item->toXml(itemNode);
            itemsNode.appendChild( itemNode );
        }
    }
}
