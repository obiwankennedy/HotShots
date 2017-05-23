
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
#include <QtCore/QDebug>

#include <QPen>
#include <QBrush>

#include <QPainter>
#include <QGraphicsDropShadowEffect>
#include <QColor>
#include <QGraphicsPolygonItem>
#include <QGraphicsSceneMouseEvent>

#include <cmath>

#include "EditorPolygonItem.h"
#include "HandgripItem.h"

EditorPolygonItem::EditorPolygonItem(QGraphicsItem *parent)
    : BaseItem(parent)
{
    m_type = metaObject()->className();
    m_item = new QGraphicsPolygonItem(this);
    m_item->setGraphicsEffect(m_dropShadowFx);
}

EditorPolygonItem::~EditorPolygonItem()
{
}

void EditorPolygonItem::setPen(const QPen &p)
{
    m_pen = p;
    m_item->setPen(p);
}

void EditorPolygonItem::setBrush(const QBrush &b)
{
    m_brush = b;
    m_item->setBrush(b);
}

QRectF EditorPolygonItem::boundingRect() const
{
    return m_item->boundingRect();
}

QPainterPath EditorPolygonItem::shape() const
{
    return m_item->shape();
}

void EditorPolygonItem::mousePress ( QGraphicsSceneMouseEvent * mouseEvent )
{
    if ( (!m_initialized && mouseEvent->button() == Qt::RightButton) ||
         (m_initialized && mouseEvent->button() == Qt::LeftButton) ) // to avoid move by post selection of item
        return;

    QPointF newPos = mapFromScene( mouseEvent->scenePos() );

    if (m_initialized)
    {
        // check if point of line selected
        int index = -1;
        if ( mouseEvent->button() == Qt::MiddleButton &&
             isPointSelected(newPos,m_line,index,5.0f) )
        {
            deletePoint(index,m_line);
            m_item->setPolygon( createPolygon() );
            return;
        }

        if ( mouseEvent->button() == Qt::RightButton &&
             !isPointSelected(newPos,m_line,index,10.0f) &&
             createPointIfNeeded(newPos,m_line) )
            m_item->setPolygon( createPolygon() );
        return;
    }

    if (mouseEvent->button() == Qt::MiddleButton)
    {
        deletePoint(m_line.size() - 1,m_line);
        mouseMove(mouseEvent); // simulate
        return;
    }

    insertPoint(m_line.size(),newPos,m_line);
    m_item->setPolygon( createPolygon() );
}

void EditorPolygonItem::mouseMove ( QGraphicsSceneMouseEvent * mouseEvent )
{
    // create a temp last point
    QPolygonF p = createPolygon();
    p << mouseEvent->scenePos();
    m_item->setPolygon(p);
}

BaseItem::typModeEdition EditorPolygonItem::mouseRelease ( QGraphicsSceneMouseEvent * mouseEvent )
{
    Q_UNUSED(mouseEvent);
    if (mouseEvent->button() != Qt::RightButton)
        return BaseItem::EDITION_CONTINUEITEM;
    else
        return BaseItem::EDITION_NEXTITEM;
}

void EditorPolygonItem::updatePoint(const QPointF &pt)
{
    HandgripItem *hg = qobject_cast<HandgripItem *>( sender() );

    if (hg)
    {
        // to avoid scene relative move
        for (int i = 0; i < m_line.size(); i++)
            m_line[i] = m_handgrips.at(i)->scenePos();
        m_line[hg->getId()] = pt;

        setPos( QPointF(0,0) );

        for (int i = 0; i < m_line.size(); i++)
            m_handgrips[i]->setPos( mapFromScene(m_line[i]) );

        m_item->setPolygon( createPolygon() );
    }
}

void EditorPolygonItem::endEdition()
{
    m_initialized = true;

    // redraw path with only stored points
    m_item->setPolygon( createPolygon() );

    // check if valid
    if (m_line.size() < 3)
        deleteLater();
}

QPolygonF EditorPolygonItem::createPolygon()
{
    QVector<QPointF> poly;
    for (int i = 0; i < m_line.size(); i++)
    {
        m_line[i] = m_handgrips.at(i)->scenePos();
        poly << mapFromScene( m_line.at(i) );
    }

    return QPolygonF(poly);
}

void EditorPolygonItem::toXml(QDomElement &elt) const
{
    QString data;
    for (int i = 0; i < m_line.size(); i++)
        data += IOHelper::variantToString( mapToScene(m_line[i]) ) + ";";
    elt.setAttribute( "coords", data);
    elt.setAttribute( "color", IOHelper::variantToString( m_item->pen().color() ) );
    elt.setAttribute( "bgColor", IOHelper::variantToString( m_item->brush().color() ) );
    elt.setAttribute( "width", IOHelper::variantToString( m_item->pen().width() ) );
   // elt.setAttribute( "style", IOHelper::variantToString( m_item->pen().style() ) );
    elt.setAttribute( "shadow", IOHelper::variantToString( isShadowEnabled() ) );

    if ( m_item->brush().style() == Qt::NoBrush)
        elt.setAttribute( "alpha", "0");
    else
        elt.setAttribute( "alpha", IOHelper::variantToString( m_item->brush().color().alpha() ) );
}

void EditorPolygonItem::fromXml(const QDomElement &elt)
{
    setPos( QPointF(0,0) );
    QStringList coords = elt.attribute("coords").split(";",QString::SkipEmptyParts);
    m_line.clear();

    //qDeleteAll(m_handgrips);
    foreach (HandgripItem * item, m_handgrips)
    {
        item->deleteLater();
    }
    m_handgrips.clear();
    bool ok = true;
    m_initialized = true;
    foreach (const QString &coord, coords)
    {
        m_line << IOHelper::stringToVariant(coord,QVariant::PointF,ok).toPointF();
        m_handgrips << new HandgripItem(this,this,m_line.size() - 1);
        m_handgrips.last()->hide();
        connect( m_handgrips.last(), SIGNAL( moved(const QPointF &) ),this, SLOT( updatePoint(const QPointF &) ) );
        m_handgrips.last()->setPos( m_line.last() );

        // detect press/release point for undo translation
        connect( m_handgrips.last(), SIGNAL( initPos(const QPointF &) ),this, SLOT( pressPoint(const QPointF &) ) );
        connect( m_handgrips.last(), SIGNAL( releasePos(const QPointF &) ),this, SLOT( releasePoint(const QPointF &) ) );
    }

    setEnableShadow( IOHelper::stringToVariant(elt.attribute("shadow","true"),QVariant::Bool,ok).toBool() );
    QColor color = IOHelper::stringToVariant(elt.attribute("color"),QVariant::Color,ok).value<QColor>();
    int width = elt.attribute("width").toInt();
    int style = elt.attribute("style").toInt();
    int alpha = elt.attribute("alpha").toInt();
    m_pen = QPen(color,width,(Qt::PenStyle)style);
    m_pen.setCapStyle(Qt::RoundCap);
    m_pen.setJoinStyle(Qt::RoundJoin);

    setPen(m_pen);
    color.setAlpha(alpha);
    m_brush = QBrush(color,Qt::SolidPattern); // just in case old format (no bg color)

    // read the bg if available
    if ( elt.hasAttribute("bgColor") )
    {
        QColor bgColor = IOHelper::stringToVariant(elt.attribute("bgColor"),QVariant::Color,ok).value<QColor>();
        bgColor.setAlpha(alpha);
        m_brush = QBrush(bgColor,Qt::SolidPattern);
    }

    if (alpha == 0)
        m_brush.setStyle(Qt::NoBrush);
    setBrush(m_brush);

    m_item->setPolygon( createPolygon() );
}

// copy constructor
BaseItem * EditorPolygonItem::clone()
{
    EditorPolygonItem *copiedItem = new  EditorPolygonItem();
    copyDataTo(copiedItem);
    return copiedItem;
}
