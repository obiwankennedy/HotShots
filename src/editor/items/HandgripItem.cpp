
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

#include <QtCore/QPropertyAnimation>
#include <QtCore/QDebug>

#include <QGraphicsSceneMouseEvent>
#include <QBrush>
#include <QPen>
#include <QCursor>

#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

const int sizeMark = 8;
const int cornerSize = 2;

#include "HandgripItem.h"
#include "BaseItem.h"

HandgripItem::HandgripItem(QObject *parentObject, QGraphicsItem *parentItem, int id, int type)
    : QObject(parentObject),
    QGraphicsPathItem(parentItem),
    m_opacityAnimation( new QPropertyAnimation(this, "opacity", this) ),
    m_moving(false),
    m_id(id),
    m_type(type)
{
    setAcceptHoverEvents(true);
    setFlag(QGraphicsItem::ItemIgnoresTransformations,true);

    QRadialGradient radialGrad(0,0,sizeMark,0,0);

    radialGrad.setColorAt( 0.0, QColor(200, 255, 255) );
    if (m_type == ROTATOR)
    {
        radialGrad.setColorAt( 0.0, QColor(255, 50, 0) );
        radialGrad.setColorAt( 1.0, QColor(255, 190, 0) );
    }
    else
    {
        radialGrad.setColorAt( 0.0, QColor(200, 255, 255) );
        radialGrad.setColorAt( 1.0, QColor(31, 197, 193) );
    }

    QBrush b(radialGrad);
    b.setStyle(Qt::RadialGradientPattern);

    QRectF r(0, 0, sizeMark, sizeMark);
    r.moveCenter( QPoint(0, 0) );
    QPainterPath p;
    p.addRoundedRect(r, cornerSize, cornerSize);

    setPath(p);
    setBrush(b);
    setPen( QPen(Qt::black) );

    QPen p2(Qt::red, 1);
    p2.setCosmetic(true);
    m_outerRing.setParentItem(this);
    m_outerRing.setRect( r.adjusted(-sizeMark / 2, -sizeMark / 2, sizeMark / 2, sizeMark / 2) );
    m_outerRing.setPen(p2 );
    m_outerRing.hide();

    // in order to
    connect( m_opacityAnimation,SIGNAL( finished () ),this,SLOT( checkVisibility() ) );

    // select a correct cursor from type
    if (m_type == TRANSLATOR_XY)
        QGraphicsItem::setCursor( QCursor(Qt::SizeAllCursor ) );
    else if (m_type == TRANSLATOR_Y)
        QGraphicsItem::setCursor( QCursor(Qt::SizeVerCursor ) );
    else if (m_type == TRANSLATOR_X)
        QGraphicsItem::setCursor( QCursor(Qt::SizeHorCursor ) );
    else
        QGraphicsItem::setCursor( QCursor( QPixmap(":/editor/editor/cursor_rotate.png") ) );
}

void HandgripItem::fadeIn(int duration)
{
    if ( !isVisible() )
    {
        show();
    }
    m_opacityAnimation->setDuration(duration);
    m_opacityAnimation->setStartValue(0);
    m_opacityAnimation->setEndValue(1);
    m_opacityAnimation->start();
}

void HandgripItem::fadeOut(int duration)
{
    m_opacityAnimation->setDuration(duration);
    m_opacityAnimation->setStartValue(1);
    m_opacityAnimation->setEndValue(0);
    m_opacityAnimation->start();
}

void HandgripItem::checkVisibility()
{
    if (opacity() == 0.0)
        hide();
    else
        show();
}

void HandgripItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event);
    m_outerRing.show();
}

void HandgripItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event);
    m_outerRing.hide();
}

void HandgripItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        m_moving = true;
        emit initPos( event->scenePos() );
    }
    else
    {
        QGraphicsPathItem::mousePressEvent(event);
    }
}

void HandgripItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        m_moving = false;
        emit releasePos( event->scenePos() );
        parentItem()->setFocus(); // not very beautiful but sometime parent must keep the focus
    }
    else
    {
        QGraphicsPathItem::mouseReleaseEvent(event);
    }
}

void HandgripItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (m_moving)
    {
        emit moved( event->scenePos() );
    }
    else
    {
        QGraphicsPathItem::mouseMoveEvent(event);
    }
}

void HandgripItem::syncHandgrips(const QRectF &sbr, const QList<HandgripItem*> &list)
{
    foreach (HandgripItem * hg, list)
    {
        switch( hg->getId() )
        {
            case HandgripItem::TopLeft:
            {
                hg->setPos( sbr.topLeft() );
                break;
            }
            case HandgripItem::TopCenter:
            {
                hg->setPos( sbr.topLeft() + QPointF(sbr.width() / 2.0f,0) );
                break;
            }
            case HandgripItem::TopRight:
            {
                hg->setPos( sbr.topRight() );
                break;
            }
            case HandgripItem::MiddleLeft:
            {
                hg->setPos( sbr.topLeft() + QPointF(0,sbr.height() / 2.0f) );
                break;
            }
            case HandgripItem::MiddleRight:
            {
                hg->setPos( sbr.topRight() + QPointF(0,sbr.height() / 2.0f) );
                break;
            }
            case HandgripItem::BottomRight:
            {
                hg->setPos( sbr.bottomRight() );
                break;
            }
            case HandgripItem::BottomCenter:
            {
                hg->setPos( sbr.bottomLeft() + QPointF(sbr.width() / 2,0) );
                break;
            }
            case HandgripItem::BottomLeft:
            {
                hg->setPos( sbr.bottomLeft() );
                break;
            }
            default:
                break;
        }
    }
}

QRectF HandgripItem::applyTranslation(const HandgripItem *hg, const QRectF & r, const QPointF &newPos, const QGraphicsItem *item)
{
    QRectF rect(r);

    if(hg && hg->getType() != HandgripItem::ROTATOR)
    {
        switch( hg->getId() )
        {
            case TopLeft:
            {
                rect.setTopLeft( item->mapFromScene(newPos) );
                break;
            }

            case TopRight:
            {
                rect.setTopRight( item->mapFromScene(newPos) );
                break;
            }
            case BottomRight:
            {
                rect.setBottomRight( item->mapFromScene(newPos) );
                break;
            }
            case BottomLeft:
            {
                rect.setBottomLeft( item->mapFromScene(newPos) );
                break;
            }
            case TopCenter:
            {
                rect.setTop( item->mapFromScene(newPos).y() );
                break;
            }
            case BottomCenter:
            {
                rect.setBottom( item->mapFromScene(newPos).y() );
                break;
            }
            case MiddleLeft:
            {
                rect.setLeft( item->mapFromScene(newPos).x() );
                break;
            }
            case MiddleRight:
            {
                rect.setRight( item->mapFromScene(newPos).x() );
                break;
            }
            default:
            {
                break;
            }
        }
    }
    return rect;
}

double HandgripItem::applyRotation(const HandgripItem *hg, const QPointF &newPos, const QPointF &anchor, const QPointF &refPt)
{
    double angle = 0.0;

    if(hg && hg->getType() == HandgripItem::ROTATOR)
    {
        double offset = ( 180.0 * HandgripItem::angleForPos( refPt, ( anchor ) ) ) / M_PI;

        double a = ( 180.0 * HandgripItem::angleForPos( newPos, ( anchor ) ) ) / M_PI;

        angle = a - offset;
    }
    return angle;
}

double HandgripItem::distanceToPoint(const QPointF &pos)
{
    return std::sqrt( pos.x() * pos.x() + pos.y() * pos.y() );
}

double HandgripItem::angleForPos(const QPointF &pos, const QPointF &anchor )
{
    double dtp = HandgripItem::distanceToPoint(pos - anchor);

    qreal angle = std::acos( ( pos.x() - anchor.x() ) / dtp );

    if(pos.y() - anchor.y() > 0)
        angle = M_PI * 2.0 - angle;

    return angle;
}
