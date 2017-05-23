
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

#include "EditorBlurItem.h"
#include "HandgripItem.h"
#include "EditorScene.h"

EditorBlurItem::EditorBlurItem(QGraphicsItem *parent)
    : BaseItem(parent),
    m_valid(false)
{
    m_type = metaObject()->className();

    m_handgrips << new HandgripItem(this,this,HandgripItem::TopLeft);
    m_handgrips << new HandgripItem(this,this,HandgripItem::TopRight);
    m_handgrips << new HandgripItem(this,this,HandgripItem::BottomLeft);
    m_handgrips << new HandgripItem(this,this,HandgripItem::BottomRight);

    updateHandgrips();

    // init with 2 dummy points
    m_line << QPointF();
    m_line << QPointF();
}

EditorBlurItem::~EditorBlurItem()
{ }

QRectF EditorBlurItem::boundingRect() const
{
    return QRectF(m_line[BEGIN],m_line[END]).normalized();
}

void EditorBlurItem::paint(QPainter *                      painter,
                           const QStyleOptionGraphicsItem *option,
                           QWidget *                       widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    if ( !scene() )
        return;

    QRectF rect( boundingRect() );

    EditorScene *scn = qobject_cast<EditorScene *>( scene() );
    if (rect.isValid() && m_valid && scn)
    {
        QImage background = scn->getUnderlayImage().copy( rect.translated( pos() - scn->getUnderlayOffset() ).toRect() );

        QImage blur = background.scaled(background.size() / 4,Qt::KeepAspectRatio,Qt::SmoothTransformation);
        painter->drawImage(rect,blur);
    }

    // draw a box if selected
    if( isSelected() )
    {
        painter->save();

        QPen pen;
        pen.setCosmetic(true);
        pen.setColor(Qt::black);
        pen.setWidth(1);
        pen.setStyle(Qt::DotLine);
        painter->setPen(pen);
        painter->drawRect(rect);
        painter->restore();
    }
}

void EditorBlurItem::mousePress ( QGraphicsSceneMouseEvent * mouseEvent )
{
    if (m_initialized) // to avoid move by post selection of item
        return;

    m_line[BEGIN] = mouseEvent->scenePos();
    m_line[END] = mouseEvent->scenePos(); // to insure invalid bounding rect for beginning

    m_initialized = true;
}

void EditorBlurItem::mouseMove ( QGraphicsSceneMouseEvent * mouseEvent )
{
    m_valid = true; // receive 2 points !!
    prepareGeometryChange();
    m_line[END] = mouseEvent->scenePos();

    HandgripItem::syncHandgrips( boundingRect(), m_handgrips );
}

BaseItem::typModeEdition EditorBlurItem::mouseRelease ( QGraphicsSceneMouseEvent * mouseEvent )
{
    Q_UNUSED(mouseEvent);
    return BaseItem::EDITION_NEXTITEM;
}

void EditorBlurItem::updatePoint(const QPointF & pt)
{
    HandgripItem *hg = qobject_cast<HandgripItem *>( sender() );

    if (hg)
    {
        QRectF newRect = HandgripItem::applyTranslation(hg,boundingRect(),pt, this);
        m_line[BEGIN] = newRect.topLeft();
        m_line[END] = newRect.bottomRight();

        prepareGeometryChange();

        HandgripItem::syncHandgrips( boundingRect(), m_handgrips );
    }
}

void EditorBlurItem::endEdition()
{
    // ensure rect is "normalized"
    QRectF norm = QRectF(m_line[BEGIN],m_line[END]).normalized();
    m_line[BEGIN] = norm.topLeft();
    m_line[END] = norm.bottomRight();

    // check validity
    if ( !QRectF(m_line[BEGIN],m_line[END]).isValid() )
        deleteLater();
}

void EditorBlurItem::toXml(QDomElement &elt) const
{
    elt.setAttribute( "start", IOHelper::variantToString( m_line[BEGIN] ) );
    elt.setAttribute( "end", IOHelper::variantToString( m_line[END] ) );
    elt.setAttribute( "pos", IOHelper::variantToString( pos() ) );
}

void EditorBlurItem::fromXml(const QDomElement &elt)
{
    m_initialized = true;
    bool ok = true;
    m_line[BEGIN] = IOHelper::stringToVariant(elt.attribute("start"),QVariant::PointF,ok).toPointF();
    m_line[END] = IOHelper::stringToVariant(elt.attribute("end"),QVariant::PointF,ok).toPointF();
    setPos( IOHelper::stringToVariant(elt.attribute("pos"),QVariant::PointF,ok).toPointF() );
    HandgripItem::syncHandgrips( boundingRect(), m_handgrips );

    m_valid = true;

    prepareGeometryChange();
}

// copy constructor
BaseItem * EditorBlurItem::clone()
{
    EditorBlurItem *copiedItem = new  EditorBlurItem();
    copyDataTo(copiedItem);
    return copiedItem;
}
