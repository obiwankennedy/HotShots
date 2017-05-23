
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
#include <QTextDocument>
#include <QGraphicsSceneMouseEvent>

#include <QGraphicsDropShadowEffect>
#include <QCursor>
#include <QGraphicsRectItem>

#include "EditorTextItem.h"
#include "HandgripItem.h"
#include "SimpleTextItem.h"

EditorTextItem::EditorTextItem(QGraphicsItem *parent)
    : BaseItem(parent),
    m_adapt(false),
    m_currentRotation(0.0),
    m_rotation(0.0),
    m_relativeRotation(0.0)
{
    m_type = metaObject()->className();
    setFlag(QGraphicsItem::ItemSendsScenePositionChanges,true);

    m_bgItem = new QGraphicsRectItem(this);

    m_item = new SimpleTextItem(this);
    m_item->setTextInteractionFlags(Qt::TextEditorInteraction);
    m_item->setGraphicsEffect(m_dropShadowFx);

    QObject::connect( m_item->document(), SIGNAL( contentsChanged() ),this, SLOT( textChanged() ) );
    QObject::connect( m_item, SIGNAL( lostFocus() ),this, SLOT( focusOut() ) );

    // because textitem can be selected and deleted by user => must track destroy
    QObject::connect( m_item, SIGNAL( destroyed() ),this, SLOT( deleteLater() ) );

    // prepare handgrips
    m_handgrips << new HandgripItem(this,m_item,HandgripItem::TopLeft,HandgripItem::ROTATOR);
    m_handgrips << new HandgripItem(this,m_item,HandgripItem::TopRight,HandgripItem::ROTATOR);
    m_handgrips << new HandgripItem(this,m_item,HandgripItem::BottomLeft,HandgripItem::ROTATOR);
    m_handgrips << new HandgripItem(this,m_item,HandgripItem::BottomRight,HandgripItem::ROTATOR);

    updateHandgrips();
}

EditorTextItem::~EditorTextItem()
{
}

void EditorTextItem::setPen(const QPen &p)
{
    m_pen = p;
    m_item->setDefaultTextColor( p.color() );
}

void EditorTextItem::setBrush(const QBrush &b)
{
    m_brush = b;
    m_bgItem->setPen(Qt::NoPen);
    m_bgItem->setBrush(b);
}

void EditorTextItem::setFont(const QFont &f)
{
    m_font = f;
    m_item->setFont(f);
    textChanged();
}

QRectF EditorTextItem::boundingRect() const
{
    return childrenBoundingRect();
}

QPainterPath EditorTextItem::shape() const
{
    QPainterPath path;
    path.addPolygon( m_item->mapToParent( m_item->boundingRect().adjusted(-3,-3,3,3) ) );
    return path;
}

QVariant EditorTextItem::itemChange(GraphicsItemChange change,
                                    const QVariant &   value)
{
//     if (change == QGraphicsItem::ItemSelectedHasChanged)
//         emit selectedChange(this);

    // clear focus on child text item
    if (change == QGraphicsItem::ItemSelectedChange)
    {
        //if (!value.toBool())
        qDebug() << m_centerPoint;
        m_item->reset();
    }

    if (change == QGraphicsItem::ItemPositionHasChanged)
    {
        return value.toPointF();

        if(!m_adapt)
        {
            qDebug() << "QGraphicsItem::ItemPositionHasChanged <<<<<<<<<<<<<<<<<<<<<<<<<";
            qreal width = boundingRect().width();
            qreal height = boundingRect().height();
            m_centerPoint = mapToParent( mapFromParent( scenePos() ) + QPointF(width / 2,height / 2) );
            m_adapt = true;
            prepareGeometryChange();
            return mapToParent( m_centerPoint - QPointF(width / 2,height / 2) );
        }
        m_adapt = false;
        return value.toPointF();
    }
    return BaseItem::itemChange(change,value);
}

void EditorTextItem::updatePoint(const QPointF &pt)
{
    HandgripItem *hg = qobject_cast<HandgripItem *>( sender() );

    if (!hg)
        return;

    prepareGeometryChange();

    if (hg->getType() == HandgripItem::ROTATOR)
    {
        m_currentRotation = HandgripItem::applyRotation(hg,pt,m_item->sceneBoundingRect().center(),m_refRotationPoint);
        rotate(m_currentRotation);
    }
}

void EditorTextItem::initRotationReferencePoint(const QPointF &p)
{
    m_refRotationPoint = p;
}

void EditorTextItem::validRotationReferencePoint(const QPointF &p)
{
    Q_UNUSED(p);
    m_rotation += m_relativeRotation;
}

void EditorTextItem::rotate(double r)
{
    QTransform m;
    QPointF anchor = m_item->boundingRect().center();

    m.translate( anchor.x(),anchor.y() );
    m.rotate(-m_rotation - r);
    m.translate( -anchor.x(),-anchor.y() );

    m_item->setTransform(m,false);
    m_bgItem->setTransform(m,false);

    m_relativeRotation = r;
}

void EditorTextItem::focusOut()
{
    setData(0,false);

    // hara kiri ?
    if ( m_item->toPlainText().isEmpty() )
        deleteLater();
}

void EditorTextItem::focusInEvent(QFocusEvent *event)
{
    setData(0,true); // must track the fact that editor get focus in order to move text cursor with keyboard
    activateEditor();
    QGraphicsItem::focusInEvent(event);
}

void EditorTextItem::textChanged()
{
    qreal width = m_item->boundingRect().width();
    qreal height = m_item->boundingRect().height();
    m_adapt = true;
    prepareGeometryChange();
    QPointF offset = pos() - m_centerPoint + QPointF(width / 2.,height / 2.);
    setPos( mapToParent( mapFromParent(m_centerPoint + offset) - QPointF(width / 2.,height / 2.) ) );

    m_bgItem->setRect( m_item->boundingRect() );

    HandgripItem::syncHandgrips( m_item->boundingRect(), m_handgrips );
}

void EditorTextItem::setCenterPoint(const QPointF  &point)
{
    m_centerPoint = point;
    textChanged();
}

void EditorTextItem::activateEditor()
{
    qDebug() << "EditorTextItem::activateEditor";
    if (m_item->textInteractionFlags() == Qt::NoTextInteraction)
        m_item->setTextInteractionFlags(Qt::TextEditorInteraction);

    m_item->setCursor( QCursor(Qt::IBeamCursor) );
}

void EditorTextItem::mousePress ( QGraphicsSceneMouseEvent * mouseEvent )
{
    if (m_initialized) // to avoid move by post selection of item
        return;

    setPos( mouseEvent->scenePos() );
    setCenterPoint( mouseEvent->scenePos() );

    m_initialized = true;
}

void EditorTextItem::mouseMove ( QGraphicsSceneMouseEvent * mouseEvent )
{
    prepareGeometryChange();
    setCenterPoint( mouseEvent->scenePos() );
}

BaseItem::typModeEdition EditorTextItem::mouseRelease ( QGraphicsSceneMouseEvent * mouseEvent )
{
    Q_UNUSED(mouseEvent);
    return BaseItem::EDITION_NEXTITEM;
}

void EditorTextItem::toXml(QDomElement &elt) const
{
    //elt.setAttribute( "pos", IOHelper::variantToString( m_centerPoint + m_item->pos() ) );
    elt.setAttribute( "pos", IOHelper::variantToString( scenePos() ) );
    elt.setAttribute( "color", IOHelper::variantToString( m_item->defaultTextColor() ) );
    elt.setAttribute( "bgColor", IOHelper::variantToString( m_brush.color() ) );
    elt.setAttribute( "text", IOHelper::variantToString( m_item->toPlainText() ) );
    elt.setAttribute( "font-size", IOHelper::variantToString( m_item->font().pointSize() ) );
    elt.setAttribute( "font-family", IOHelper::variantToString( m_item->font().family() ) );
    elt.setAttribute( "rotation", QString::number( m_rotation,'f',1) );
    elt.setAttribute( "shadow", IOHelper::variantToString( isShadowEnabled() ) );
    if ( m_brush.style() == Qt::NoBrush)
        elt.setAttribute( "alpha", "0");
    else
        elt.setAttribute( "alpha", IOHelper::variantToString( m_brush.color().alpha() ) );
}

void EditorTextItem::fromXml(const QDomElement &elt)
{
    m_item->setPlainText( elt.attribute("text") );
    m_initialized = true;
    bool ok = true;
    m_rotation = elt.attribute("rotation").toDouble();
    setEnableShadow( IOHelper::stringToVariant(elt.attribute("shadow","true"),QVariant::Bool,ok).toBool() );
    QColor color = IOHelper::stringToVariant(elt.attribute("color"),QVariant::Color,ok).value<QColor>();
    int alpha = elt.attribute("alpha","0").toInt();
    QPen pen(color);

    // read the bg if available
    if ( elt.hasAttribute("bgColor") )
    {
        QColor bgColor = IOHelper::stringToVariant(elt.attribute("bgColor"),QVariant::Color,ok).value<QColor>();
        bgColor.setAlpha(alpha);
        m_brush = QBrush(bgColor,Qt::SolidPattern);
    }

    int fontSize = elt.attribute("font-size").toInt();
    QString fontFamily = elt.attribute("font-family");

    QFont font(fontFamily,fontSize);
    setFont(font);

    setPen(pen);
    if (alpha == 0)
        m_brush.setStyle(Qt::NoBrush);
    setBrush(m_brush);

    m_centerPoint = IOHelper::stringToVariant(elt.attribute("pos"),QVariant::PointF,ok).toPointF();
    setPos(m_centerPoint);
    setCenterPoint(m_centerPoint);
    rotate(0.0);
}

// copy constructor
BaseItem * EditorTextItem::clone()
{
    EditorTextItem *copiedItem = new  EditorTextItem();
    copyDataTo(copiedItem);
    return copiedItem;
}
