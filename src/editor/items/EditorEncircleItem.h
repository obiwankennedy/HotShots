
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

#ifndef _EDITORENCIRCLEITEM_H_
#define _EDITORENCIRCLEITEM_H_

#include "BaseItem.h"

class QGraphicsPathItem;

class EditorEncircleItem : public BaseItem
{
    Q_OBJECT

public:

    enum typPoint
    {
        BEGIN = 0,
        END
    };

    EditorEncircleItem(QGraphicsItem *parent = 0);
    virtual ~EditorEncircleItem();

    virtual QRectF boundingRect() const;
    virtual QPainterPath shape() const;

    virtual void setPen(const QPen &);
    QPen pen() const;

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
    void initRotationReferencePoint(const QPointF &);
    void validRotationReferencePoint(const QPointF &);

private:

    void rotate(double a);
    QPainterPath createPath(const QRectF &);

    QGraphicsPathItem *m_item;
    QList<QPointF> m_line;
    QRectF m_rect;
    double m_currentRotation;
    double m_rotation;
    double m_relativeRotation;
    double m_currentSx;
    double m_currentSy;
    QPointF m_refRotationPoint;
};

#endif // _EDITORENCIRCLEITEM_H_
