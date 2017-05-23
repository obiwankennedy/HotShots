
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

#ifndef _EDITORTAGITEM_H_
#define _EDITORTAGITEM_H_

#include <QtCore/QList>
#include <QFont>

#include "BaseItem.h"

class QGraphicsPathItem;
class QGraphicsEllipseItem;

class EditorTagItem :    public BaseItem
{
    Q_OBJECT

public:

    EditorTagItem(QGraphicsItem *parent = 0);
    virtual ~EditorTagItem();

    virtual void setPen(const QPen &);
    virtual void setBrush(const QBrush &);

    virtual void setFont(const QFont &font);

    void setTag(const QString &);
    const QString & tag() const;

    virtual QRectF boundingRect() const;

    virtual void mousePress ( QGraphicsSceneMouseEvent *  );
    virtual void mouseMove ( QGraphicsSceneMouseEvent *  );
    virtual typModeEdition mouseRelease ( QGraphicsSceneMouseEvent *  );

    virtual void setEnableShadow(bool);

    virtual void toXml(QDomElement &) const;
    virtual void fromXml(const QDomElement &e);

    virtual void paint(QPainter *,const QStyleOptionGraphicsItem *,QWidget *);

    // copy constructor
    virtual BaseItem * clone();

private:

    void setWidth(int w);

    void createRect(const QPointF &orig);
    void fitText(const QString &txt, const QRectF &rect);
    float fontScaleFactor(const QString &txt,
                          const QFont &  font,
                          const QRectF & rect) const;
    void drawText(const QString &txt,
                  const QFont &  font,
                  const QRectF & rect);

    qreal convert(qreal o_value, qreal o_min, qreal o_max, qreal n_min, qreal n_max) const;

    QGraphicsPathItem *m_pathItem;
    QSizeF m_size;
    QString m_numberString;
    QPointF m_pos;
    QGraphicsEllipseItem *m_item;
    QGraphicsDropShadowEffect *m_textFx;
};

#endif // _EDITORTAGITEM_H_
