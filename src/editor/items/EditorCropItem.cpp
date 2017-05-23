
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
#include <QtCore/QDebug>
#include <QtCore/QTimer>

#include <QPen>
#include <QBrush>

#include <QPainter>
#include <QColor>
#include <QGraphicsLineItem>
#include <QGraphicsDropShadowEffect>
#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>
#include <QCursor>

#include "EditorCropItem.h"
#include "EditorScene.h"
#include "HandgripItem.h"

EditorCropItem::EditorCropItem(QGraphicsItem *parent)
    : BaseItem(parent),
    m_valid(false),
    m_stopEdition(false)
{
    m_type = metaObject()->className();
    setFlag(QGraphicsItem::ItemIsFocusable,true); // for keypressevent

    setCursor( QCursor( QPixmap(":/editor/editor/crop_cursor.png") ) );

    m_handgrips << new HandgripItem(this,this,HandgripItem::TopLeft);
    m_handgrips << new HandgripItem(this,this,HandgripItem::TopRight);
    m_handgrips << new HandgripItem(this,this,HandgripItem::BottomLeft);
    m_handgrips << new HandgripItem(this,this,HandgripItem::BottomRight);

    updateHandgrips();

    /*
       foreach (HandgripItem * item, m_handgrips)
       connect( item, SIGNAL( moved(const QPointF &) ),this, SLOT( updatePoint(const QPointF &) ) );

       hideHandgrips();
     */

    // init with 2 dummy points
    m_line << QPointF();
    m_line << QPointF();
}

EditorCropItem::~EditorCropItem()
{
    QGraphicsItem::unsetCursor ();
}

QRectF EditorCropItem::boundingRect() const
{
    return QRectF(m_line[BEGIN],m_line[END]).normalized();
}

void EditorCropItem::paint(QPainter *                      painter,
                           const QStyleOptionGraphicsItem *option,
                           QWidget *                       widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    if ( !scene() || !boundingRect().isValid() || !m_valid )
        return;

    QRectF rect( boundingRect() );

    painter->save();

    QPen pen;
    pen.setCosmetic(true);
    pen.setColor(Qt::black);
    pen.setWidth(2);
    pen.setStyle(Qt::DotLine);
    painter->setPen(pen);
    painter->setBrush( QBrush( QColor(255,0,0,100) ) );
    painter->drawRect(rect);
    painter->restore();
}

void EditorCropItem::mousePress ( QGraphicsSceneMouseEvent * mouseEvent )
{
    if (m_initialized) // to avoid move by post selection of item
        return;

    m_line[BEGIN] = mouseEvent->scenePos();
    m_line[END] = mouseEvent->scenePos(); // to insure invalid bounding rect for beginning

    m_initialized = true;
}

void EditorCropItem::mouseMove ( QGraphicsSceneMouseEvent * mouseEvent )
{
    if (m_stopEdition)
        return;

    m_valid = true; // receive 2 points !!
    prepareGeometryChange();
    m_line[END] = mouseEvent->scenePos();

    HandgripItem::syncHandgrips( boundingRect(), m_handgrips );
}

BaseItem::typModeEdition EditorCropItem::mouseRelease ( QGraphicsSceneMouseEvent * mouseEvent )
{
    Q_UNUSED(mouseEvent);
    m_stopEdition = true;
    QTimer::singleShot( 10, this, SLOT( forceSelection() ) );

//    return false;
    return BaseItem::EDITION_FINISHITEM;
}

void EditorCropItem::updatePoint(const QPointF & pt)
{
    HandgripItem *hg = qobject_cast<HandgripItem *>( sender() );

    if (hg)
    {
        QRectF newRect = HandgripItem::applyTranslation(hg,boundingRect(),pt, this);
        m_line[BEGIN] = newRect.topLeft();
        m_line[END] = newRect.bottomRight();

        prepareGeometryChange();

        HandgripItem::syncHandgrips( boundingRect(), m_handgrips );
    }
}

void EditorCropItem::endEdition()
{
    // ensure rect is "normalized"
    QRectF norm = QRectF(m_line[BEGIN],m_line[END]).normalized();
    m_line[BEGIN] = norm.topLeft();
    m_line[END] = norm.bottomRight();

    // check validity
    if ( !QRectF(m_line[BEGIN],m_line[END]).isValid() )
        deleteLater();
}

void EditorCropItem::toXml(QDomElement &elt) const
{
    Q_UNUSED(elt);
}

void EditorCropItem::fromXml(const QDomElement &elt)
{
    Q_UNUSED(elt);
}

// copy constructor
BaseItem * EditorCropItem::clone()
{
    // musn't duplicate this item
    return NULL;
}

void EditorCropItem::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return)
    {
        QRectF rect( boundingRect() );
        EditorScene *scn = qobject_cast<EditorScene *>( scene() );
        if (rect.isValid() && scn && m_valid)
        {
            scn->cropBackground( rect.translated( pos() ) );
            deleteLater();
        }
    }

    QGraphicsItem::keyPressEvent(event);
}
