
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

#include <QPolygonF>
#include <QPen>
#include <QBrush>

#include <QPainter>
#include <QGraphicsDropShadowEffect>
#include <QColor>
#include <QGraphicsPathItem>
#include <QGraphicsSceneMouseEvent>

#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#include "EditorEncircleItem.h"
#include "HandgripItem.h"

const int widthFactor = 2;

EditorEncircleItem::EditorEncircleItem(QGraphicsItem *parent)
    : BaseItem(parent),
    m_currentRotation(0.0),
    m_rotation(0.0),
    m_relativeRotation(0.0),
    m_currentSx(0.0),
    m_currentSy(0.0)
{
    m_type = metaObject()->className();
    m_item = new QGraphicsPathItem(this);
    m_item->setGraphicsEffect(m_dropShadowFx);

    // init with 2 dummy points
    m_line << QPointF();
    m_line << QPointF();

    // prepare handgrips
    m_handgrips << new HandgripItem(this,m_item,HandgripItem::TopLeft,HandgripItem::ROTATOR);
    m_handgrips << new HandgripItem(this,m_item,HandgripItem::TopCenter,HandgripItem::TRANSLATOR_Y);
    m_handgrips << new HandgripItem(this,m_item,HandgripItem::TopRight,HandgripItem::ROTATOR);
    m_handgrips << new HandgripItem(this,m_item,HandgripItem::MiddleLeft,HandgripItem::TRANSLATOR_X);
    m_handgrips << new HandgripItem(this,m_item,HandgripItem::MiddleRight,HandgripItem::TRANSLATOR_X);
    m_handgrips << new HandgripItem(this,m_item,HandgripItem::BottomLeft,HandgripItem::ROTATOR);
    m_handgrips << new HandgripItem(this,m_item,HandgripItem::BottomCenter,HandgripItem::TRANSLATOR_Y);
    m_handgrips << new HandgripItem(this,m_item,HandgripItem::BottomRight,HandgripItem::ROTATOR);

    updateHandgrips();
}

EditorEncircleItem::~EditorEncircleItem()
{ }

void EditorEncircleItem::setPen(const QPen &p)
{
    m_pen = p;

    // double pen width !!
    QPen realPen(p);
    realPen.setWidth(p.width() * widthFactor);
    m_item->setPen(realPen);

    QBrush br(m_item->pen().color(),Qt::NoBrush);
    m_item->setBrush(br);
}

QRectF EditorEncircleItem::boundingRect() const
{
    return mapFromScene( m_item->sceneBoundingRect() ).boundingRect();
}

QPainterPath EditorEncircleItem::shape() const
{
    return mapFromItem( m_item,m_item->shape() );
}

void EditorEncircleItem::mousePress ( QGraphicsSceneMouseEvent * mouseEvent )
{
    if (m_initialized) // to avoid move by post selection of item
        return;

    m_line[BEGIN] = mouseEvent->scenePos();

    m_initialized = true;
}

void EditorEncircleItem::mouseMove ( QGraphicsSceneMouseEvent * mouseEvent )
{
    m_line[END] = mouseEvent->scenePos();
    m_item->setPath( createPath( QRectF(m_line[BEGIN],m_line[END]).normalized() ) );
    HandgripItem::syncHandgrips( m_item->path().boundingRect(), m_handgrips );
}

BaseItem::typModeEdition EditorEncircleItem::mouseRelease ( QGraphicsSceneMouseEvent * mouseEvent )
{
    Q_UNUSED(mouseEvent);
    return BaseItem::EDITION_NEXTITEM;
}

void EditorEncircleItem::updatePoint(const QPointF &pt)
{
    HandgripItem *hg = qobject_cast<HandgripItem *>( sender() );

    if (!hg)
        return;

    QRectF rect = QRectF(m_line[BEGIN],m_line[END]).normalized();
    prepareGeometryChange();

    if (hg->getType() != HandgripItem::ROTATOR)
    {
        QRectF newRect = HandgripItem::applyTranslation(hg,rect,pt, m_item);

        m_line[BEGIN] = newRect.topLeft();
        m_line[END] = newRect.bottomRight();
        m_item->setPath( createPath( QRectF(m_line[BEGIN],m_line[END]).normalized() ) );
        HandgripItem::syncHandgrips( m_item->path().boundingRect(), m_handgrips );
    }
    else
    {
        m_currentRotation = HandgripItem::applyRotation(hg,pt,m_item->sceneBoundingRect().center(),m_refRotationPoint);
        rotate(m_currentRotation);
    }
}

void EditorEncircleItem::endEdition()
{
    // ensure rect is "normalized"
    QRectF norm = QRectF(m_line[BEGIN],m_line[END]).normalized();
    m_line[BEGIN] = norm.topLeft();
    m_line[END] = norm.bottomRight();

    // check validity
    if ( !QRectF(m_line[BEGIN],m_line[END]).isValid() )
        deleteLater();
}

void EditorEncircleItem::initRotationReferencePoint(const QPointF &p)
{
    m_refRotationPoint = p;
}

void EditorEncircleItem::validRotationReferencePoint(const QPointF &p)
{
    Q_UNUSED(p);
    prepareUndo();
    m_rotation += m_relativeRotation;
    saveUndo();
}

void EditorEncircleItem::rotate(double r)
{
    QTransform m;
    QPointF anchor = m_item->path().boundingRect().center();

    m.translate( anchor.x(),anchor.y() );
    m.rotate(-m_rotation - r);
    m.translate( -anchor.x(),-anchor.y() );

    m_item->setTransform(m,false);

    m_relativeRotation = r;
}

QPainterPath EditorEncircleItem::createPath(const QRectF &rect)
{
    double r(rect.height() * 0.01); //this alters the distance between different tracks of spiral
    const double pi = 3.14159265; //more or less
    double maxRadius = 8.1 * pi;
    double minRadius = 5.9 * pi;
    QPointF center( rect.center() );

    QPolygonF spiral;

    for(double d = maxRadius; d > minRadius; d -= 0.05)
    {
        double x = -(sinf(d) * d) * r;
        double y = -( sinf( d + (pi / 2.0) ) ) * ( d + (pi / 2.0) ) * r;

        spiral << QPointF(x,y);
    }

    QRectF maxSpiral = spiral.boundingRect();

    QPointF offset(maxSpiral.center().x(),
        maxSpiral.center().y()
        );

    QPainterPath p(center - offset +
        QPointF( spiral[0].x() * ( rect.width() * 0.9 / maxSpiral.width() ),
                 spiral[0].y() * ( rect.height() * 0.9 / maxSpiral.height() ) )
        );

    for(int i = 1; i < spiral.size(); i++)
        p.lineTo(
            center - offset +
            QPointF( spiral[i].x() * ( rect.width() * 0.9 / maxSpiral.width() ),
                     spiral[i].y() * ( rect.height() * 0.9 / maxSpiral.height() ) )
            );

    return p;
}

void EditorEncircleItem::toXml(QDomElement &elt) const
{
    elt.setAttribute( "color", IOHelper::variantToString( m_item->pen().color() ) );
    elt.setAttribute( "rotation", QString::number( m_rotation,'f',1) );
    elt.setAttribute( "width", IOHelper::variantToString( m_item->pen().width() / widthFactor ) );
    elt.setAttribute( "rect", IOHelper::variantToString( QRectF(m_line[BEGIN],m_line[END]).normalized() ) );
    elt.setAttribute( "pos", IOHelper::variantToString( pos() ) );
 //   elt.setAttribute( "style", IOHelper::variantToString( m_item->pen().style() ) );
    elt.setAttribute( "shadow", IOHelper::variantToString( isShadowEnabled() ) );
}

void EditorEncircleItem::fromXml(const QDomElement &elt)
{
    m_line.clear();
    m_initialized = true;
    bool ok = true;
    QRectF rect = IOHelper::stringToVariant(elt.attribute("rect"),QVariant::RectF,ok).toRectF();
    m_line << rect.topLeft();
    m_line << rect.bottomRight();

    setEnableShadow( IOHelper::stringToVariant(elt.attribute("shadow","true"),QVariant::Bool,ok).toBool() );
    QColor color = IOHelper::stringToVariant(elt.attribute("color"),QVariant::Color,ok).value<QColor>();
    int width = elt.attribute("width").toInt();
    int style = elt.attribute("style").toInt();
    m_rotation = elt.attribute("rotation").toDouble();
    QPointF pos = IOHelper::stringToVariant(elt.attribute("pos"),QVariant::PointF,ok).toPointF();
    setPos(pos);

    QPen pen(color,width,(Qt::PenStyle)style);
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);

    setPen(pen);
    m_brush = QBrush(color,Qt::NoBrush);
    setBrush(m_brush);

    m_item->setPath( createPath(rect) );
    HandgripItem::syncHandgrips( m_item->path().boundingRect(), m_handgrips );
    rotate(0.0);
}

// copy constructor
BaseItem * EditorEncircleItem::clone()
{
    EditorEncircleItem *copiedItem = new  EditorEncircleItem();
    copyDataTo(copiedItem);
    return copiedItem;
}
