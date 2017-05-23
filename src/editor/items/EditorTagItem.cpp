
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

#include "EditorTagItem.h"

#include <QBrush>
#include <QPen>
#include <QPainter>
#include <QGraphicsEllipseItem>
#include <QGraphicsDropShadowEffect>

#include <QFont>
#include <QFontMetrics>
#include <QGraphicsSceneMouseEvent>

#include "TagManager.h"

#include <cmath>

EditorTagItem::EditorTagItem(QGraphicsItem *parent)
    : BaseItem(parent),
    m_size(15.5, 15.5),
    m_numberString("1")
{
    m_type = metaObject()->className();

    m_item = new QGraphicsEllipseItem(this);
    QBrush br(m_item->pen().color(),Qt::SolidPattern);
    m_item->setBrush(br);
    m_item->setGraphicsEffect(m_dropShadowFx);

    m_pathItem = new QGraphicsPathItem(m_item);

    QPen pen;
    pen.setStyle(Qt::SolidLine);
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);
    pen.setBrush(Qt::white);

    m_pathItem->setPen(pen);
    m_pathItem->setBrush(Qt::white);
    m_textFx = new QGraphicsDropShadowEffect();
    m_pathItem->setGraphicsEffect(m_textFx);
    m_textFx->setColor( QColor(63, 63, 63, 190) );
    m_textFx->setBlurRadius(7);
    m_textFx->setOffset( QPoint(3, 3) );

    TagManager::registerTag(this);
}

EditorTagItem::~EditorTagItem()
{
    TagManager::unregisterTag(this);
}

void EditorTagItem::setEnableShadow(bool val)
{
    m_dropShadowFx->setEnabled(val);
    m_textFx->setEnabled(val);
}

void EditorTagItem::setPen(const QPen &p)
{
    m_pen = p;
    QPen pen(p.color(), p.width(), Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    m_item->setPen(Qt::NoPen);
    m_pathItem->setPen( m_pen.color() );
    m_pathItem->setBrush( m_pen.color() );

    //setWidth( p.width() );
    //QBrush br(m_item->pen().color(),Qt::SolidPattern);
    //m_item->setBrush(br);
}

void EditorTagItem::setBrush(const QBrush &b)
{
    m_brush = b;
    QPen pen(m_brush.color(), m_pen.width(), Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    m_item->setPen(pen);

    //setWidth( p.width() );
    QBrush br(b.color(),Qt::SolidPattern);
    m_item->setBrush(br);
}

void EditorTagItem::setFont(const QFont &font)
{
    m_font = font;
    setWidth( font.pointSize() );
    fitText( m_numberString, m_item->rect() );
}

void EditorTagItem::mousePress ( QGraphicsSceneMouseEvent * mouseEvent )
{
    if (m_initialized)
        return;

    m_pos = mouseEvent->scenePos();

    createRect(m_pos);
    m_initialized = true;
}

void EditorTagItem::mouseMove ( QGraphicsSceneMouseEvent * mouseEvent )
{
    m_pos = mouseEvent->scenePos();
    createRect(m_pos);
}

BaseItem::typModeEdition EditorTagItem::mouseRelease ( QGraphicsSceneMouseEvent * mouseEvent )
{
    Q_UNUSED(mouseEvent);
    return BaseItem::EDITION_NEXTITEM;
}

qreal EditorTagItem::convert(qreal o_value, qreal o_min, qreal o_max, qreal n_min, qreal n_max) const
{
    return ( (o_value - o_min) * (n_max - n_min) ) / (o_max - o_min) + n_min;
}

void EditorTagItem::setWidth(int w)
{
    //qreal step = (80.0 - 25.0) / 1.0;
    qreal dim = 10 + 5 * w;
    m_size = QSizeF(dim,dim);
    QRectF rect = m_item->rect();
    rect.setSize(m_size);
    m_item->setRect(rect);
    fitText(m_numberString, rect);
}

void EditorTagItem::setTag(const QString & num)
{
    m_numberString = num;
    fitText( m_numberString, m_item->rect() );
}

const QString &EditorTagItem::tag() const
{
    return m_numberString;
}

void EditorTagItem::createRect(const QPointF &orig)
{
    QPointF center(orig.x() - m_size.width() / 2, orig.y() - m_size.height() / 2);
    QRectF rect(center, m_size);
    m_item->setRect(rect);
    fitText(m_numberString, rect);
}

void EditorTagItem::fitText(const QString &txt, const QRectF &r)
{
    QFont font = m_font;

    //I calculate the rect inscribed into the circle
    qreal l = sqrt(r.width() * r.width() / 2);
    QRectF rect(0, 0, l, l);
    rect.moveCenter( r.center() );

    float factor = fontScaleFactor(txt, font, rect);

    if (factor < 1 || factor > 1.25)
    {
        font.setPointSizeF(font.pointSizeF() * factor);
    }

    drawText(txt, font, rect);
}

float EditorTagItem::fontScaleFactor(const QString &txt,
                                     const QFont &  font,
                                     const QRectF & rect) const
{
    QFontMetrics fm(font);

    if ( rect.isNull() )
        return 1.0;

    QRectF br = fm.tightBoundingRect(txt);
    int fw = br.width();
    int fh = br.height();

    return (fw < fh ? rect.width() / fh : rect.width() / fw);
}

void EditorTagItem::drawText(const QString &txt,
                             const QFont &  font,
                             const QRectF & rect)
{
    QFontMetrics fm(font);

    QRectF txtBr = fm.tightBoundingRect(txt);
    txtBr.moveCenter( rect.center() );

    qreal x = txtBr.bottomLeft().x() - fm.leftBearing( txt.at(0) );
    qreal y = txtBr.bottomLeft().y();

    QPainterPath path;
    path.setFillRule(Qt::WindingFill);
    path.addText(x, y, font, txt);

    m_pathItem->setPath(path);
}

QRectF EditorTagItem::boundingRect() const
{
    return m_item->boundingRect();
}

void EditorTagItem::paint(QPainter *                      painter,
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

void EditorTagItem::toXml(QDomElement &elt) const
{
    elt.setAttribute( "pos", IOHelper::variantToString( m_pos + pos() ) );
    elt.setAttribute( "fgColor", IOHelper::variantToString( m_pen.color() ) );
    elt.setAttribute( "bgColor", IOHelper::variantToString( m_brush.color() ) );
    elt.setAttribute( "width", IOHelper::variantToString( m_item->pen().width() ) );
    elt.setAttribute( "tag", m_numberString);
    elt.setAttribute( "font-size", IOHelper::variantToString( m_font.pointSize() ) );
    elt.setAttribute( "font-family", IOHelper::variantToString( m_font.family() ) );
    elt.setAttribute( "shadow", IOHelper::variantToString( isShadowEnabled() ) );
}

void EditorTagItem::fromXml(const QDomElement &elt)
{
    m_initialized = true;
    bool ok = true;
    m_pos = IOHelper::stringToVariant(elt.attribute("pos"),QVariant::PointF,ok).toPointF();
    QColor bgColor = IOHelper::stringToVariant(elt.attribute("color"),QVariant::Color,ok).value<QColor>();
    setEnableShadow( IOHelper::stringToVariant(elt.attribute("shadow","true"),QVariant::Bool,ok).toBool() );

    // read the bg if available
    if ( elt.hasAttribute("bgColor") )
        bgColor = IOHelper::stringToVariant(elt.attribute("bgColor"),QVariant::Color,ok).value<QColor>();

    // foreground is white by default
    QColor fgColor = IOHelper::stringToVariant(elt.attribute("fgColor","#ffffff"),QVariant::Color,ok).value<QColor>();

    int width = elt.attribute("width").toInt();
    m_numberString = elt.attribute("tag");
    m_pen = QPen(fgColor,width);
    m_pen.setCapStyle(Qt::RoundCap);
    m_pen.setJoinStyle(Qt::RoundJoin);

    m_brush = QBrush(bgColor,Qt::SolidPattern);

    int fontSize = elt.attribute("font-size").toInt();
    QString fontFamily = elt.attribute("font-family");

    m_font = QFont(fontFamily,fontSize);
    setFont(m_font);
    createRect(m_pos);

    setPen(m_pen);
    setBrush(m_brush);
}

// copy constructor
BaseItem * EditorTagItem::clone()
{
    EditorTagItem *copiedItem = new  EditorTagItem();
    copyDataTo(copiedItem);
    copiedItem->setTag( QString::number( TagManager::getTagNumber() ) ); // mustn't have duplicate number !!
    return copiedItem;
}
