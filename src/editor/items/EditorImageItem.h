
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

#ifndef _EDITORIMAGEITEM_H_
#define _EDITORIMAGEITEM_H_

#include "BaseItem.h"

class QGraphicsPixmapItem;

class EditorImageItem : public BaseItem
{
    Q_OBJECT

public:

    enum typPoint
    {
        BEGIN = 0,
        END
    };

    EditorImageItem(QGraphicsItem *parent = 0);
    virtual ~EditorImageItem();

    virtual void paint(QPainter * painter, const QStyleOptionGraphicsItem *option, QWidget * widget);
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

    virtual void processSettings();

private slots:

    void updatePoint(const QPointF &);

private:

    void setSize(const QSizeF& theValue);

    void setImage(const QString &);

    QGraphicsPixmapItem *m_item;
    QList<QPointF> m_line;
    QRectF m_imageRect;
    QString m_file;
};

#endif // _EDITORIMAGEITEM_H_
