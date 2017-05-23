
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
#include <QColor>
#include <QGraphicsLineItem>
#include <QGraphicsDropShadowEffect>
#include <QGraphicsSceneMouseEvent>

#include <QGraphicsEllipseItem>

#include "EditorMagnifierItem.h"
#include "HandgripItem.h"
#include "EditorScene.h"

const int defaultMagnifyingFactor = 4;

EditorMagnifierItem::EditorMagnifierItem(QGraphicsItem *parent)
    : BaseItem(parent),
    m_valid(false),
    m_magnifyingFactor(defaultMagnifyingFactor)
{
    m_type = metaObject()->className();

    m_handgrips << new HandgripItem(this,this,HandgripItem::TopLeft);
    m_handgrips << new HandgripItem(this,this,HandgripItem::TopRight);
    m_handgrips << new HandgripItem(this,this,HandgripItem::BottomLeft);
    m_handgrips << new HandgripItem(this,this,HandgripItem::BottomRight);

    updateHandgrips();

    /*
       foreach (HandgripItem * item, m_handgrips)
       connect( item, SIGNAL( moved(const QPointF &) ),this, SLOT( updatePoint(const QPointF &) ) );

       hideHandgrips();
     */

    // init with 2 dummy points
    m_line << QPointF();
    m_line << QPointF();

    m_magnifyingFactor = getItemSetting("HighlightColor",defaultMagnifyingFactor).toInt();
}

EditorMagnifierItem::~EditorMagnifierItem()
{ }

QRectF EditorMagnifierItem::boundingRect() const
{
    return getRect().adjusted(-1,-1,1,1);
}

QRectF EditorMagnifierItem::getRect() const
{
    return QRectF(m_line[BEGIN],m_line[END]).normalized();
}

void EditorMagnifierItem::updateMask()
{
    QRectF current = getRect();
    QSize box = QSize( current.width(), current.width() );

    // reupdate our mask
    if (m_imgMask.size() != box)
    {
        int radius = box.width() / 2;
        int ring = radius - 10;

        m_imgMask = QPixmap(box);
        m_imgMask.fill(Qt::transparent);

        QRadialGradient g;
        g.setCenter(radius, radius);
        g.setFocalPoint(radius, radius);
        g.setRadius(radius);
        g.setColorAt( 1.0, QColor(255, 255, 255, 0) );
        g.setColorAt( 0.5, QColor(128, 128, 128, 255) );

        QPainter mask(&m_imgMask);
        mask.setRenderHint(QPainter::Antialiasing);
        mask.setCompositionMode(QPainter::CompositionMode_Source);
        mask.setBrush(g);
        mask.setPen(Qt::NoPen);
        mask.drawRect( m_imgMask.rect() );
        mask.setBrush( QColor(Qt::transparent) );
        mask.drawEllipse(g.center(), ring, ring);
        mask.end();
    }
}

void EditorMagnifierItem::paint(QPainter *                      painter,
                                const QStyleOptionGraphicsItem *option,
                                QWidget *                       widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    if ( !scene() )
        return;

    QRectF rect = getRect();

    EditorScene *scn = qobject_cast<EditorScene *>( scene() );
    if (rect.isValid() && m_valid && scn)
    {
        updateMask();
        painter->save(); // because clipping is active !!

        QPainterPath clipPath;
        clipPath.addEllipse(getRect().center(), getRect().width() / 2,getRect().width() / 2);
        painter->setClipPath(clipPath);
        painter->setClipping(true);

        // TODO add extracted image to a cache !!!
        QRectF extractRect( rect.translated( pos() - scn->getUnderlayOffset() ) );
        int extractWidth = extractRect.width() / m_magnifyingFactor;
        extractRect.adjust(extractWidth,extractWidth,-extractWidth,-extractWidth);

        QRectF bgRect = scn->getUnderlayImage().rect();

        QRectF croppedRect = extractRect.intersected(bgRect);
        if ( !croppedRect.isNull() )
        {
            QImage background = scn->getUnderlayImage().copy( croppedRect.toRect() );
            QImage mag = background.scaled(rect.size().toSize(),Qt::KeepAspectRatio,Qt::SmoothTransformation);
            painter->drawImage(rect,mag);
        }

        painter->drawPixmap(rect.topLeft(), m_imgMask);
        painter->restore();
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

void EditorMagnifierItem::mousePress ( QGraphicsSceneMouseEvent * mouseEvent )
{
    if (m_initialized) // to avoid move by post selection of item
        return;

    m_line[BEGIN] = mouseEvent->scenePos();
    m_line[END] = mouseEvent->scenePos(); // to insure invalid bounding rect for beginning

    m_initialized = true;
}

void EditorMagnifierItem::mouseMove ( QGraphicsSceneMouseEvent * mouseEvent )
{
    m_valid = true; // receive 2 points !!
    prepareGeometryChange();
    m_line[END] = mouseEvent->scenePos();

    QRectF current = QRectF(m_line[BEGIN],m_line[END]).normalized();
    int width = qMin( current.width(),current.height() );

    qreal xEnd,yEnd;
    if ( m_line[END].x() > m_line[BEGIN].x() )
        xEnd = m_line[BEGIN].x() + width;
    else
        xEnd = m_line[BEGIN].x() - width;

    if ( m_line[END].y() > m_line[BEGIN].y() )
        yEnd = m_line[BEGIN].y() + width;
    else
        yEnd = m_line[BEGIN].y() - width;

    m_line[END] = QPointF(xEnd,yEnd);

    HandgripItem::syncHandgrips( getRect(), m_handgrips );
}

BaseItem::typModeEdition EditorMagnifierItem::mouseRelease ( QGraphicsSceneMouseEvent * mouseEvent )
{
    Q_UNUSED(mouseEvent);
    return BaseItem::EDITION_NEXTITEM;
}

void EditorMagnifierItem::updatePoint(const QPointF & pt)
{
    HandgripItem *hg = qobject_cast<HandgripItem *>( sender() );

    if (hg)
    {
        QRectF newRect = HandgripItem::applyTranslation(hg,getRect(),pt, this);
        int width = qMin( newRect.width(),newRect.height() );

        QRectF square;
        if (hg->getId() == HandgripItem::TopLeft)
        {
            square = QRectF( newRect.bottomRight(),QSize(-width,-width) ).normalized();
        }
        else if (hg->getId() == HandgripItem::TopRight)
        {
            square = QRectF( newRect.bottomLeft(),QSize(width,-width) ).normalized();
        }
        else if (hg->getId() == HandgripItem::BottomLeft)
        {
            square = QRectF( newRect.topRight(),QSize(-width,width) ).normalized();
        }
        else if (hg->getId() == HandgripItem::BottomRight)
        {
            square = QRectF( newRect.topLeft(),QSize(width,width) ).normalized();
        }

        m_line[BEGIN] = square.topLeft();
        m_line[END] = square.bottomRight();

        prepareGeometryChange();

        HandgripItem::syncHandgrips( getRect(), m_handgrips );
    }
}

void EditorMagnifierItem::endEdition()
{
    // ensure rect is "normalized"
    QRectF norm = getRect();
    m_line[BEGIN] = norm.topLeft();
    m_line[END] = norm.bottomRight();

    // check validity
    if ( !getRect().isValid() )
        deleteLater();
}

void EditorMagnifierItem::toXml(QDomElement &elt) const
{
    elt.setAttribute( "start", IOHelper::variantToString( m_line[BEGIN] ) );
    elt.setAttribute( "end", IOHelper::variantToString( m_line[END] ) );
    elt.setAttribute( "pos", IOHelper::variantToString( pos() ) );
    elt.setAttribute( "magnifyingFactor", IOHelper::variantToString( m_magnifyingFactor ) );
}

void EditorMagnifierItem::fromXml(const QDomElement &elt)
{
    m_initialized = true;
    bool ok = true;
    m_line[BEGIN] = IOHelper::stringToVariant(elt.attribute("start"),QVariant::PointF,ok).toPointF();
    m_line[END] = IOHelper::stringToVariant(elt.attribute("end"),QVariant::PointF,ok).toPointF();
    setPos( IOHelper::stringToVariant(elt.attribute("pos"),QVariant::PointF,ok).toPointF() );
    HandgripItem::syncHandgrips( getRect(), m_handgrips );
    m_magnifyingFactor = IOHelper::stringToVariant(elt.attribute("magnifyingFactor"),QVariant::Int,ok).toInt();

    m_valid = true;

    prepareGeometryChange();
}

// copy constructor
BaseItem * EditorMagnifierItem::clone()
{
    EditorMagnifierItem *copiedItem = new  EditorMagnifierItem();
    copyDataTo(copiedItem);
    return copiedItem;
}
