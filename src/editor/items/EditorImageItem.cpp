
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

#include <QPen>
#include <QBrush>

#include <QPainter>
#include <QColor>
#include <QGraphicsLineItem>
#include <QGraphicsDropShadowEffect>
#include <QGraphicsSceneMouseEvent>

#include <QGraphicsPixmapItem>

#include "EditorImageItem.h"
#include "HandgripItem.h"

const QString defaultPixmap(":/editor/editor/photo.png");
const QString pixmapTag("pixmap");

EditorImageItem::EditorImageItem(QGraphicsItem *parent)
    : BaseItem(parent)
{
    m_type = metaObject()->className();
    m_item = new QGraphicsPixmapItem(this);
    m_item->setGraphicsEffect(m_dropShadowFx);

    m_handgrips << new HandgripItem(this,this,0);
    m_handgrips << new HandgripItem(this,this,1);

    updateHandgrips();

    /*
       connect( m_handgrips[0], SIGNAL( moved(const QPointF &) ),this, SLOT( updatePoint(const QPointF &) ) );
       connect( m_handgrips[1], SIGNAL( moved(const QPointF &) ),this, SLOT( updatePoint(const QPointF &) ) );

       hideHandgrips();
     */

    // init with 2 dummy points
    m_line << QPointF();
    m_line << QPointF();
}

EditorImageItem::~EditorImageItem()
{ }

void EditorImageItem::setImage(const QString &file)
{
    // load pixmap
    m_file = file;

    QPixmap pix(m_file);

    if ( pix.isNull() )
        pix = QPixmap(defaultPixmap);

    m_item->setPixmap( pix );
    m_item->setOffset( -m_item->pixmap().rect().center() );
    m_imageRect = m_item->boundingRect();
}

QRectF EditorImageItem::boundingRect() const
{
    return m_item->mapToParent( m_item->boundingRect() ).boundingRect();
}

void EditorImageItem::paint(QPainter *                      painter,
                            const QStyleOptionGraphicsItem *option,
                            QWidget *                       widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    // draw a box if selected
    if( isSelected() )
    {
        painter->save();

        QPen pen;
        pen.setCosmetic(true);
        pen.setColor(Qt::black);
        pen.setWidth(1);
        pen.setStyle(Qt::DotLine);
        painter->setPen(pen);
        painter->drawRect( boundingRect() );
        painter->restore();
    }
}

void EditorImageItem::mousePress ( QGraphicsSceneMouseEvent * mouseEvent )
{
    if (m_initialized) // to avoid move by post selection of item
        return;

    setPos( mouseEvent->scenePos() );

    // by default display image with ratio 1/1
    m_line[BEGIN] = m_imageRect.topLeft();
    m_line[END] = m_imageRect.bottomRight();
    m_handgrips[BEGIN]->setPos(m_line[BEGIN]);
    m_handgrips[END]->setPos(m_line[END]);

    m_initialized = true;
}

void EditorImageItem::mouseMove ( QGraphicsSceneMouseEvent * mouseEvent )
{
    prepareGeometryChange();

    // must user pos() point because user want "resize" the image
    m_line[END] = mouseEvent->scenePos();
    QPointF offset(scenePos() - m_line[END]);
    m_line[BEGIN] = m_line[END] - 2 * offset;

    setSize( QRectF(m_line[BEGIN],m_line[END]).normalized().size() );

    m_line[BEGIN] = boundingRect().topLeft();
    m_line[END] = boundingRect().bottomRight();

    m_handgrips[BEGIN]->setPos(m_line[BEGIN]);
    m_handgrips[END]->setPos(m_line[END]);
}

void EditorImageItem::setSize(const QSizeF& theValue)
{
    QSizeF size = theValue;

    // update the matrix
    QSizeF source = m_imageRect.size();
    QSizeF dest = size;

    if( source.width() == 0 || source.height() == 0 || dest.width() == 0 || dest.height() == 0 )
        return;

    qreal xscale = dest.width() / source.width();
    qreal yscale = dest.height() / source.height();

    qreal scale = qMax(xscale,yscale);

    QTransform matrix;
    matrix.scale(scale, scale);

    m_item->setTransform(matrix);
}

BaseItem::typModeEdition EditorImageItem::mouseRelease ( QGraphicsSceneMouseEvent * mouseEvent )
{
    Q_UNUSED(mouseEvent);
    return BaseItem::EDITION_NEXTITEM;
}

void EditorImageItem::updatePoint(const QPointF &pt)
{
    HandgripItem *hg = qobject_cast<HandgripItem *>( sender() );

    if (hg)
    {
        // to avoid scene relative move
        m_line[BEGIN] = m_handgrips[BEGIN]->pos();
        m_line[END] = m_handgrips[END]->pos();
        m_line[hg->getId()] = mapFromScene(pt);
        prepareGeometryChange();

        setSize( QRectF(m_line[0],m_line[1]).normalized().size() );
        m_line[BEGIN] = boundingRect().topLeft();
        m_line[END] = boundingRect().bottomRight();

        m_handgrips[BEGIN]->setPos(m_line[BEGIN]);
        m_handgrips[END]->setPos(m_line[END]);
    }
}

void EditorImageItem::endEdition()
{
    // ensure rect is "normalized"
    QRectF norm = QRectF(m_line[BEGIN],m_line[END]).normalized();
    m_line[BEGIN] = norm.topLeft();
    m_line[END] = norm.bottomRight();

    // check validity
    if ( !QRectF(m_line[BEGIN],m_line[END]).isValid() )
        deleteLater();
}

void EditorImageItem::toXml(QDomElement &elt) const
{
    elt.setAttribute( "start", IOHelper::variantToString( m_line[0] ) );
    elt.setAttribute( "end", IOHelper::variantToString( m_line[1] ) );
    elt.setAttribute( "pos", IOHelper::variantToString( pos() ) );
    elt.setAttribute( "path", IOHelper::variantToString( m_file ) );
    elt.setAttribute( "shadow", IOHelper::variantToString( isShadowEnabled() ) );
}

void EditorImageItem::fromXml(const QDomElement &elt)
{
    bool ok = true;
    m_line[BEGIN] = IOHelper::stringToVariant(elt.attribute("start"),QVariant::PointF,ok).toPointF();
    m_line[END] = IOHelper::stringToVariant(elt.attribute("end"),QVariant::PointF,ok).toPointF();
    QPointF pos = IOHelper::stringToVariant(elt.attribute("pos"),QVariant::PointF,ok).toPointF();
    setPos(pos);

    setEnableShadow( IOHelper::stringToVariant(elt.attribute("shadow","true"),QVariant::Bool,ok).toBool() );

    QString file = elt.attribute("path");
    setImage(file);

    setSize( QRectF(m_line[BEGIN],m_line[END]).normalized().size() );
    m_line[BEGIN] = boundingRect().topLeft();
    m_line[END] = boundingRect().bottomRight();

    m_handgrips[BEGIN]->setPos(m_line[BEGIN]);
    m_handgrips[END]->setPos(m_line[END]);
}

void EditorImageItem::processSettings()
{
    setImage( getItemSetting(pixmapTag,defaultPixmap).toString() );
}

// copy constructor
BaseItem * EditorImageItem::clone()
{
    EditorImageItem *copiedItem = new  EditorImageItem();
    copyDataTo(copiedItem);
    return copiedItem;
}
