
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

#ifndef _EDITORTEXTITEM_H_
#define _EDITORTEXTITEM_H_

#include <QPen>
#include <QTextCursor>

#include "BaseItem.h"

class QFocusEvent;
class QGraphicsItem;
class QGraphicsScene;
class QGraphicsSceneMouseEvent;
class SimpleTextItem;

class EditorTextItem :    public BaseItem
{
    Q_OBJECT

public:

    enum typPoint
    {
        BEGIN = 0,
        END
    };

    explicit EditorTextItem(QGraphicsItem *parent = 0);
    virtual ~EditorTextItem();

    QPointF centerPoint(){
        return m_centerPoint;
    }

    virtual void setPen(const QPen &p);
    virtual void setBrush(const QBrush &b);
    virtual void setFont(const QFont &);

    void setCenterPoint(const QPointF &point);
    void activateEditor();

    virtual QRectF boundingRect() const;
    virtual QPainterPath shape() const;

    QString text();

    virtual void toXml(QDomElement &) const;
    virtual void fromXml(const QDomElement &e);

    // copy constructor
    virtual BaseItem * clone();

protected:

    virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value);

    virtual void focusInEvent(QFocusEvent *event);

    virtual void mouseMove ( QGraphicsSceneMouseEvent * event );
    virtual void mousePress ( QGraphicsSceneMouseEvent * event );
    virtual typModeEdition mouseRelease ( QGraphicsSceneMouseEvent * event );

protected slots:

    void textChanged();
    void focusOut();

private slots:

    void updatePoint(const QPointF &);
    void initRotationReferencePoint(const QPointF &);
    void validRotationReferencePoint(const QPointF &);

private:

    void rotate(double a);

    SimpleTextItem *m_item;
    QGraphicsRectItem *m_bgItem;

    // QList<QPointF> m_line;
    QPointF m_centerPoint;
    bool m_adapt;
    double m_currentRotation;
    double m_rotation;
    double m_relativeRotation;
    QPointF m_refRotationPoint;
};

#endif // _EDITORTEXTITEM_H_
