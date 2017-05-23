
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

#include <QtCore/QDebug>
#include <QGraphicsSceneMouseEvent>
#include <QTextCursor>
#include <QKeyEvent>
#include <QTextDocument>

#include "SimpleTextItem.h"

SimpleTextItem::SimpleTextItem(QGraphicsItem *parent) : QGraphicsTextItem(parent)
{
    setFlag(QGraphicsItem::ItemIsMovable,true);
    setFlag(QGraphicsItem::ItemIsSelectable,true);

/*
    QTextOption option = document()->defaultTextOption ();
    option.setAlignment ( Qt::AlignHCenter );
    document()->setDefaultTextOption ( option );
 */
}

QVariant SimpleTextItem::itemChange(GraphicsItemChange change,
                                    const QVariant &   value)
{
    if (change == QGraphicsItem::ItemSelectedChange)
    {
        if ( parentItem() )
            parentItem()->setSelected( value.toBool() );
        return false;

        emit selectedChange(this);
    }
    return value;
}

void SimpleTextItem::reset()
{
    setTextInteractionFlags(Qt::NoTextInteraction);

    // clear selection if exists
    QTextCursor t = textCursor();
    t.clearSelection();
    setTextCursor(t);
}

void SimpleTextItem::focusInEvent(QFocusEvent *event)
{
    if ( parentItem() )
        parentItem()->setData(0,true);
    qDebug() << "SimpleTextItem::focusInEvent";
    QGraphicsTextItem::focusInEvent(event);
}

void SimpleTextItem::focusOutEvent(QFocusEvent *event)
{
    reset();
    emit lostFocus();
    QGraphicsTextItem::focusOutEvent(event);
}

void SimpleTextItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    if (textInteractionFlags() == Qt::NoTextInteraction)
        setTextInteractionFlags(Qt::TextEditorInteraction);
    QGraphicsTextItem::mouseDoubleClickEvent(event);
}
