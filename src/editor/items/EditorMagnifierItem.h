
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

#ifndef _EDITORMAGNIFIERITEM_H_
#define _EDITORMAGNIFIERITEM_H_

#include "BaseItem.h"

class EditorMagnifierItem : public BaseItem
{
    Q_OBJECT

public:

    enum typPoint
    {
        BEGIN = 0,
        END
    };

    EditorMagnifierItem(QGraphicsItem *parent = 0);
    virtual ~EditorMagnifierItem();

    virtual void paint(QPainter *,const QStyleOptionGraphicsItem *,QWidget *);

    virtual QRectF boundingRect() const;

    virtual void mouseMove ( QGraphicsSceneMouseEvent * event );
    virtual void mousePress ( QGraphicsSceneMouseEvent * event );
    virtual typModeEdition mouseRelease ( QGraphicsSceneMouseEvent * event );

    virtual void toXml(QDomElement &) const;
    virtual void fromXml(const QDomElement &e);

    virtual void endEdition();

    // copy constructor
    virtual BaseItem * clone();

protected:

private slots:

    void updatePoint(const QPointF &);

private:

    void syncHandgrips(const QRectF &sbr);
    void updateMask();
    QRectF getRect() const;

    QList<QPointF> m_line;
    bool m_valid;
    QPixmap m_imgMask;
    int m_magnifyingFactor;
};

#endif // _EDITORMAGNIFIERITEM_H_
