
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

#include <QtCore/QPointF>

#include <QPen>
#include <QBrush>

#include <QPainter>
#include <QColor>
#include <QGraphicsLineItem>
#include <QGraphicsDropShadowEffect>
#include <QGraphicsSceneMouseEvent>

#include "EditorLineItem.h"
#include "HandgripItem.h"

EditorLineItem::EditorLineItem(QGraphicsItem *parent)
    : BaseItem(parent)
{
    m_type = metaObject()->className();
    m_item = new QGraphicsLineItem(this);
    m_item->setGraphicsEffect(m_dropShadowFx);

    m_handgrips << new HandgripItem(this,this,0);
    m_handgrips << new HandgripItem(this,this,1);

    updateHandgrips();

    /*
       connect( m_handgrips[BEGIN], SIGNAL( moved(const QPointF &) ),this, SLOT( updatePoint(const QPointF &) ) );
       connect( m_handgrips[END], SIGNAL( moved(const QPointF &) ),this, SLOT( updatePoint(const QPointF &) ) );

       hideHandgrips();
     */

    // init with 2 dummy points
    m_line << QPointF();
    m_line << QPointF();
}

EditorLineItem::~EditorLineItem()
{ }

QPen EditorLineItem::pen() const
{
    return m_item->pen();
}

void EditorLineItem::setPen(const QPen &p)
{
    m_pen = p;
    m_item->setPen(p);
}

QRectF EditorLineItem::boundingRect() const
{
    return m_item->boundingRect();
}

void EditorLineItem::mousePress ( QGraphicsSceneMouseEvent * mouseEvent )
{
    if (m_initialized) // to avoid move by post selection of item
        return;

    m_line[BEGIN] = mouseEvent->scenePos();
    m_handgrips[BEGIN]->setPos( mapFromScene(m_line[BEGIN]) );
    m_initialized = true;
}

void EditorLineItem::mouseMove ( QGraphicsSceneMouseEvent * mouseEvent )
{
    m_line[END] = mouseEvent->scenePos();
    m_item->setLine( QLineF( mapFromScene(m_line[BEGIN]),mapFromScene(m_line[END]) ) );

    m_handgrips[END]->setPos( mapFromScene(m_line[END]) );
}

BaseItem::typModeEdition EditorLineItem::mouseRelease ( QGraphicsSceneMouseEvent * mouseEvent )
{
    Q_UNUSED(mouseEvent);
    return BaseItem::EDITION_NEXTITEM;
}

void EditorLineItem::updatePoint(const QPointF &pt)
{
    HandgripItem *hg = qobject_cast<HandgripItem *>( sender() );

    if (hg)
    {
        // to avoid scene relative move
        m_line[BEGIN] = m_handgrips[BEGIN]->scenePos();
        m_line[END] = m_handgrips[END]->scenePos();
        m_line[hg->getId()] = pt;
        setPos( QPointF(0,0) );
        m_item->setLine( QLineF( mapFromScene(m_line[BEGIN]),mapFromScene(m_line[END]) ) );
        m_handgrips[BEGIN]->setPos( mapFromScene(m_line[BEGIN]) );
        m_handgrips[END]->setPos( mapFromScene(m_line[END]) );
    }
}

void EditorLineItem::endEdition()
{
    // check validity
    QPointF dist = m_line[BEGIN] - m_line[END];
    if (dist.manhattanLength() < 3)
        deleteLater();
}

void EditorLineItem::toXml(QDomElement &elt) const
{
    elt.setAttribute( "start", IOHelper::variantToString( mapToScene(m_line[BEGIN]) ) );
    elt.setAttribute( "end", IOHelper::variantToString( mapToScene(m_line[END] ) ) );
    elt.setAttribute( "color", IOHelper::variantToString( m_item->pen().color() ) );
    elt.setAttribute( "width", IOHelper::variantToString( m_item->pen().width() ) );
    //elt.setAttribute( "style", IOHelper::variantToString( m_item->pen().style() ) );
    elt.setAttribute( "shadow", IOHelper::variantToString( isShadowEnabled() ) );
}

void EditorLineItem::fromXml(const QDomElement &elt)
{
    bool ok = true;
    m_initialized = true;
    setPos( QPointF(0,0) );
    m_line[BEGIN] = IOHelper::stringToVariant(elt.attribute("start"),QVariant::PointF,ok).toPointF();
    m_line[END] = IOHelper::stringToVariant(elt.attribute("end"),QVariant::PointF,ok).toPointF();

    setEnableShadow( IOHelper::stringToVariant(elt.attribute("shadow","true"),QVariant::Bool,ok).toBool() );
    QColor color = IOHelper::stringToVariant(elt.attribute("color"),QVariant::Color,ok).value<QColor>();
    int width = elt.attribute("width").toInt();
    int style = elt.attribute("style").toInt();
    m_pen = QPen(color,width,(Qt::PenStyle)style);
    m_pen.setCapStyle(Qt::RoundCap);
    m_pen.setJoinStyle(Qt::RoundJoin);

    setPen(m_pen);
    setBrush( QBrush() );
    m_item->setLine( QLineF( (m_line[BEGIN]),(m_line[END]) ) );
    m_handgrips[BEGIN]->setPos( (m_line[BEGIN]) );
    m_handgrips[END]->setPos( (m_line[END]) );
}

// copy constructor
BaseItem * EditorLineItem::clone()
{
    EditorLineItem *copiedItem = new  EditorLineItem();
    copyDataTo(copiedItem);
    return copiedItem;
}
