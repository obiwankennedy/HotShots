
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

#ifndef _EDITORARROWITEM_H_
#define _EDITORARROWITEM_H_

#include "BaseItem.h"

class QPointF;
class QGraphicsPathItem;
class QPainterPath;
class QGraphicsRectItem;
class QGraphicsItem;

class EditorArrowItem : public BaseItem
{
    Q_OBJECT

public:

    enum typPoint
    {
        TAIL = 0,
        HEAD
    };

    EditorArrowItem(QGraphicsItem *parent = 0);
    virtual ~EditorArrowItem();

    virtual void setPen(const QPen &);
    virtual QPen pen() const;

    virtual QRectF boundingRect() const;

    virtual void mouseMove ( QGraphicsSceneMouseEvent * event );
    virtual void mousePress ( QGraphicsSceneMouseEvent * event );
    virtual typModeEdition mouseRelease ( QGraphicsSceneMouseEvent * event );

    virtual void toXml(QDomElement &) const;
    virtual void fromXml(const QDomElement &e);

    virtual void endEdition();

    // copy constructor
    virtual BaseItem * clone();

private slots:

    void updatePoint(const QPointF &);

private:

    virtual void createShape(const QPointF &start, const QPointF &end);

    void setArrow(const QPainterPath &arrow, const QPointF &pos, qreal angle);
    QPainterPath createArrow(const QPointF &tail,
                             const QPointF &head,
                             qreal          scaleFactor) const;
    qreal calculateAngle(const QPointF &start, const QPointF &end) const;
    qreal convert(qreal o_value, qreal o_min, qreal o_max, qreal n_min, qreal n_max) const;
    qreal convert(qreal o_value,qreal o_min,qreal o_max,qreal o_pivot,qreal n_min,qreal n_max,qreal n_pivot) const;
    void redrawArrow();

    qreal m_scaleFactor;
    qreal m_scaleMin;
    qreal m_scaleMax;
    qreal m_scalePivot;

    QGraphicsPathItem *m_item;
    QList<QPointF> m_line;
};

#endif // _EDITORARROWITEM_H_
