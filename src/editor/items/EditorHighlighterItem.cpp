
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

#include <QKeyEvent>

#include "EditorHighlighterItem.h"
#include "HandgripItem.h"
#include "EditorScene.h"

const int widthFactor = 8;
const QColor defaultColor = QColor(255,255,0);
EditorHighlighterItem::EditorHighlighterItem(QGraphicsItem *parent)
    : BaseItem(parent),
    m_compositionMode(QPainter::CompositionMode_Multiply),
    m_highlightColor(defaultColor)
{
    m_type = metaObject()->className();
    m_item = new QGraphicsLineItem(this);
    m_item->setFlag( QGraphicsItem::ItemClipsToShape,true);
    m_item->setGraphicsEffect(m_dropShadowFx);

    m_handgrips << new HandgripItem(this,this,0);
    m_handgrips << new HandgripItem(this,this,1);

    updateHandgrips();

    /*
       connect( m_handgrips[BEGIN], SIGNAL( moved(const QPointF &) ),this, SLOT( updatePoint(const QPointF &) ) );
       connect( m_handgrips[END], SIGNAL( moved(const QPointF &) ),this, SLOT( updatePoint(const QPointF &) ) );

       hideHandgrips();
     */

    //setFlag(ItemIsFocusable,true); // for keypressevent

    // init with 2 dummy points
    m_line << QPointF();
    m_line << QPointF();

    m_highlightColor = getItemSetting("HighlightColor",defaultColor).value<QColor>();
}

EditorHighlighterItem::~EditorHighlighterItem()
{ }

QPen EditorHighlighterItem::pen() const
{
    return m_item->pen();
}

void EditorHighlighterItem::setPen(const QPen &p)
{
    m_pen = p;

    QPen realPen(p);

    // multiply pen width !!
    realPen.setWidth(p.width() * widthFactor);
    realPen.setColor(defaultColor);
    realPen.setColor( QColor(0,0,0,0) );
    realPen.setCapStyle(Qt::FlatCap);
    realPen.setJoinStyle(Qt::MiterJoin);

    // must have a valid to extract a valid clip path
    m_item->setPen(realPen);
}

void EditorHighlighterItem::paint(QPainter *                      painter,
                                  const QStyleOptionGraphicsItem *option,
                                  QWidget *                       widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    QRectF rect( boundingRect() );

    EditorScene *scn = qobject_cast<EditorScene *>( scene() );
    if (rect.isValid()  && scn)
    {
        painter->setClipPath( m_item->clipPath() );
        painter->setClipping(true);

        QRectF extractRect( rect.translated( pos() - scn->getUnderlayOffset() ) );
        QImage background = scn->getUnderlayImage().copy( extractRect.toRect() );

        painter->drawImage(rect,background);

        painter->setPen(Qt::NoPen);
        painter->setRenderHint(QPainter::Antialiasing);
        painter->setCompositionMode(QPainter::CompositionMode_Multiply);
        painter->setBrush( QBrush(m_highlightColor) );
        painter->drawRect( boundingRect() );
    }
}

QRectF EditorHighlighterItem::boundingRect() const
{
    return m_item->boundingRect();
}

void EditorHighlighterItem::mousePress ( QGraphicsSceneMouseEvent * mouseEvent )
{
    if (m_initialized) // to avoid move by post selection of item
        return;

    m_line[BEGIN] = mouseEvent->scenePos();
    m_handgrips[BEGIN]->setPos( mapFromScene(m_line[BEGIN]) );
    m_initialized = true;
}

void EditorHighlighterItem::mouseMove ( QGraphicsSceneMouseEvent * mouseEvent )
{
    m_line[END] = mouseEvent->scenePos();
    m_item->setLine( QLineF( mapFromScene(m_line[BEGIN]),mapFromScene(m_line[END]) ) );

    m_handgrips[END]->setPos( mapFromScene(m_line[END]) );
}

BaseItem::typModeEdition EditorHighlighterItem::mouseRelease ( QGraphicsSceneMouseEvent * mouseEvent )
{
    Q_UNUSED(mouseEvent);
    return BaseItem::EDITION_NEXTITEM;
}

void EditorHighlighterItem::updatePoint(const QPointF &pt)
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

        //hg->setPos(pt);
        m_handgrips[BEGIN]->setPos( mapFromScene(m_line[BEGIN]) );
        m_handgrips[END]->setPos( mapFromScene(m_line[END]) );
    }
}

void EditorHighlighterItem::endEdition()
{
    // check validity
    QPointF dist = m_line[BEGIN] - m_line[END];
    if (dist.manhattanLength() < 3)
        deleteLater();
}

void EditorHighlighterItem::toXml(QDomElement &elt) const
{
    elt.setAttribute( "start", IOHelper::variantToString( mapToScene(m_line[BEGIN]) ) );
    elt.setAttribute( "end", IOHelper::variantToString( mapToScene(m_line[END] ) ) );
    elt.setAttribute( "color", IOHelper::variantToString( m_highlightColor ) );
    elt.setAttribute( "width", IOHelper::variantToString( m_item->pen().width() / widthFactor ) );
}

void EditorHighlighterItem::fromXml(const QDomElement &elt)
{
    bool ok = true;
    m_initialized = true;
    setPos( QPointF(0,0) );

    m_line[BEGIN] = IOHelper::stringToVariant(elt.attribute("start"),QVariant::PointF,ok).toPointF();
    m_line[END] = IOHelper::stringToVariant(elt.attribute("end"),QVariant::PointF,ok).toPointF();

    m_highlightColor = IOHelper::stringToVariant(elt.attribute("color"),QVariant::Color,ok).value<QColor>();
    int width = elt.attribute("width").toInt();
    m_pen = QPen(QColor(Qt::black),width);
    setPen(m_pen);
    setBrush( QBrush() );
    m_item->setLine( QLineF( (m_line[BEGIN]),(m_line[END]) ) );
    m_handgrips[BEGIN]->setPos( (m_line[BEGIN]) );
    m_handgrips[END]->setPos( (m_line[END]) );
}

// copy constructor
BaseItem * EditorHighlighterItem::clone()
{
    EditorHighlighterItem *copiedItem = new  EditorHighlighterItem();
    copyDataTo(copiedItem);
    return copiedItem;
}
