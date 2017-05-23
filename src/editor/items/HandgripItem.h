
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

 #ifndef _HANDGRIPITEM_H
#define _HANDGRIPITEM_H

#include <QtCore/QObject>

#include <QGraphicsRectItem>

#include <QGraphicsEllipseItem>
#include <QPainter>
#include <QGraphicsDropShadowEffect>

class QPropertyAnimation;
class BaseItem;

class HandgripItem : public QObject, public QGraphicsPathItem
{
    Q_OBJECT

    // needed for opacity animation !!
    Q_PROPERTY(qreal opacity READ opacity WRITE setOpacity)

public :

        enum typHandgrip
    {
        TopLeft  = 0,     /**< Top left node */
        TopCenter,        /**< Top center node */
        TopRight,         /**< Top right node */
        MiddleLeft,
        MiddleRight,
        BottomLeft,       /**< Bottom left node */
        BottomCenter,
        BottomRight,      /**< Bottom right node */
        Center            /**< Center node */
    };

    enum typMove
    {
        TRANSLATOR_XY = 0,
        TRANSLATOR_X,
        TRANSLATOR_Y,
        ROTATOR
    };

    HandgripItem(QObject *parentObject, QGraphicsItem *parentItem, int id, int type = TRANSLATOR_XY);

    int getId() const {return m_id; }
    void setId(int id)  {m_id = id; }
    int getType() const {return m_type; }

    void fadeIn(int duration = 150);
    void fadeOut(int duration = 150);

    static void syncHandgrips(const QRectF &sbr, const QList<HandgripItem*> &);
    static QRectF applyTranslation(const HandgripItem *hg, const QRectF & r, const QPointF &newPos, const QGraphicsItem *item);
    static double applyRotation(const HandgripItem *hg, const QPointF &newPos, const QPointF &anchor, const QPointF &refPt);
    static double distanceToPoint(const QPointF &pos);
    static double angleForPos(const QPointF &pos, const QPointF &anchor );

signals:

    void moved(const QPointF &newPos);
    void initPos(const QPointF &newPos);
    void releasePos(const QPointF &newPos);

protected:

    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

private slots:

    void checkVisibility();

private:

    QPropertyAnimation *m_opacityAnimation;
    bool m_moving;
    QGraphicsEllipseItem m_outerRing;
    int m_id;
    int m_type;
};

#endif // _HANDGRIPITEM_H
