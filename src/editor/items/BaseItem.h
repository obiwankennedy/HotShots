
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

#ifndef _BASEITEM_H_
#define _BASEITEM_H_

#include <QtCore/QTextStream>
#include <QtCore/QMap>

#include <QGraphicsObject>
#include <QPen>
#include <QBrush>
#include <QFont>

#include <QtXml/QDomElement>

#include "IOHelper.h"

class QGraphicsDropShadowEffect;
class HandgripItem;
class EditorUndoCommand;

class BaseItem :
    public QGraphicsObject
{
    Q_OBJECT

public:

    enum typModeEdition
    {
        EDITION_NONE = 0, // no current mode
        EDITION_NEXTITEM, // item finished, create a new item next mouse press
        EDITION_CONTINUEITEM, // continue same item
        EDITION_FINISHITEM // end item and change to select mode
    };

    BaseItem(QGraphicsItem *parent = 0);
    virtual ~BaseItem() {}

    virtual QRectF boundingRect() const;
    virtual void paint(QPainter *                      painter,
                       const QStyleOptionGraphicsItem *option,
                       QWidget *                       widget);

    virtual void setEnableShadow(bool);
    virtual bool isShadowEnabled() const;

    // get/set rendering parameters
    virtual void setPen(const QPen &);
    virtual void setBrush(const QBrush &);
    virtual void setFont(const QFont &);

    QPen pen() const;
    QBrush brush() const;
    QFont font() const;

    virtual const QString & getType () const;

    virtual void mouseMove ( QGraphicsSceneMouseEvent * event ) = 0;
    virtual void mousePress ( QGraphicsSceneMouseEvent * event ) = 0;
    virtual typModeEdition mouseRelease ( QGraphicsSceneMouseEvent * event ) = 0;
    virtual void mouseDoubleClick ( QGraphicsSceneMouseEvent * mouseEvent );

    virtual void toXml(QDomElement &) const = 0;
    virtual void fromXml(const QDomElement &e) = 0;

    virtual void endEdition();

    QVariant getItemSetting (const QString &key, const QVariant &defaultValue) const;
    void setItemSettings(const QMap<QString, QVariant>&);

    // copy constructor
    virtual BaseItem * clone() = 0;

    void prepareUndo();
    void saveUndo();

public slots:

    void forceSelection();

protected slots:

    virtual void pressPoint(const QPointF &);
    virtual void releasePoint(const QPointF &);

protected:

    virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value);
    virtual void processSettings();
    void showHandgrips();
    void hideHandgrips();

    bool isPointSelected(const QPointF &newPos, QList<QPointF> & line, int & index, qreal selectDist = 10.0f );
    void deletePoint(int index, QList<QPointF> & line);
    void insertPoint(int index, const QPointF &pos, QList<QPointF> & line);
    bool createPointIfNeeded(const QPointF &newPos, QList<QPointF> & line, bool loop = true );
    qreal distancePointToLine(const QPointF &p, const QPointF &a, const QPointF &b);

    void copyDataTo(BaseItem *);
    void updateHandgrips();

    QTransform makeRotation(double angle, const QRectF &rect);
    QTransform makeRotation(double angle, const QPointF &pt);

    QList<HandgripItem*> m_handgrips;

    QGraphicsDropShadowEffect *m_dropShadowFx;

    QPen m_pen;
    QBrush m_brush;
    QFont m_font;

    QString m_type;
    bool m_initialized;

    // setting data
    QMap<QString, QVariant> m_itemSettings;
    EditorUndoCommand *m_currUndo;
};

#endif // _BASEITEM_H_
