
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
#include <QGraphicsDropShadowEffect>
#include <QColor>
#include <QGraphicsPathItem>
#include <QGraphicsSceneMouseEvent>

#include "EditorCurveItem.h"
#include "HandgripItem.h"

EditorCurveItem::EditorCurveItem(QGraphicsItem *parent)
    : BaseItem(parent)
{
    m_type = metaObject()->className();
    m_item = new QGraphicsPathItem(this);
    m_item->setGraphicsEffect(m_dropShadowFx);
}

EditorCurveItem::~EditorCurveItem()
{ }

void EditorCurveItem::setPen(const QPen &p)
{
    m_pen = p;
    m_item->setPen(p);
    m_item->setBrush(Qt::NoBrush);
}

QRectF EditorCurveItem::boundingRect() const
{
    return childrenBoundingRect();
}

QPainterPath EditorCurveItem::shape() const
{
    return m_item->shape();
}

void EditorCurveItem::paint(QPainter *                      painter,
                            const QStyleOptionGraphicsItem *option,
                            QWidget *                       widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    // draw a box if selected
    if( isSelected() && m_line.size() > 1 )
    {
        painter->save();

        QPen pen;
        pen.setCosmetic(true);
        pen.setColor(Qt::black);
        pen.setWidth(1);
        pen.setStyle(Qt::DotLine);
        painter->setPen(pen);

        QVector<QPointF> tmpLine;
        foreach (const QPointF &pt, m_line)
        tmpLine << mapToScene(pt);

        painter->drawPolyline( m_line.toVector() );
        painter->restore();
    }
}

void EditorCurveItem::mousePress ( QGraphicsSceneMouseEvent * mouseEvent )
{
    if ( (!m_initialized && mouseEvent->button() == Qt::RightButton) ||
         (m_initialized && mouseEvent->button() == Qt::LeftButton) ) // to avoid move by post selection of item
        return;

    QPointF newPos = mapFromScene( mouseEvent->scenePos() );

    if (m_initialized)
    {
        // check if point of line selected
        int index = -1;
        if ( mouseEvent->button() == Qt::MiddleButton &&
             isPointSelected(newPos,m_line,index,5.0f) )
        {
            deletePoint(index,m_line);
            m_item->setPath( createSplinePath() );
            return;
        }

        if ( mouseEvent->button() == Qt::RightButton &&
             !isPointSelected(newPos,m_line,index,10.0f) &&
             createPointIfNeeded(newPos,m_line, false) )
            m_item->setPath( createSplinePath() );
        return;
    }

    if (mouseEvent->button() == Qt::MiddleButton)
    {
        deletePoint(m_line.size() - 1,m_line);
        mouseMove(mouseEvent); // simulate
        //m_item->setPath( createSplinePath() );
        return;
    }

    insertPoint(m_line.size(),newPos,m_line);
    m_item->setPath( createSplinePath() );
}

void EditorCurveItem::mouseMove ( QGraphicsSceneMouseEvent * mouseEvent )
{
    // create a temp last point
    QVector<QPointF> tmpLine( m_line.toVector() );
    tmpLine << mouseEvent->scenePos();
    QPainterPath p = createSplinePath(tmpLine);
    m_item->setPath(p);
}

BaseItem::typModeEdition EditorCurveItem::mouseRelease ( QGraphicsSceneMouseEvent * mouseEvent )
{
    Q_UNUSED(mouseEvent);
    if (mouseEvent->button() != Qt::RightButton)
        return BaseItem::EDITION_CONTINUEITEM;
    else
        return BaseItem::EDITION_NEXTITEM;
}

void EditorCurveItem::updatePoint(const QPointF &pt)
{
    HandgripItem *hg = qobject_cast<HandgripItem *>( sender() );

    if (hg)
    {
        // to avoid scene relative move
        for (int i = 0; i < m_line.size(); i++)
            m_line[i] = m_handgrips.at(i)->scenePos();
        m_line[hg->getId()] = pt;
        setPos(0,0);

        for (int i = 0; i < m_line.size(); i++)
            m_handgrips[i]->setPos( mapFromScene(m_line[i]) );

        m_item->setPath( createSplinePath() );
    }
}

void EditorCurveItem::endEdition()
{
    m_initialized = true;

    for (int i = 0; i < m_line.size(); i++)
        m_line[i] = m_handgrips.at(i)->scenePos();
    setPos(0,0);

    for (int i = 0; i < m_line.size(); i++)
        m_handgrips[i]->setPos( mapFromScene(m_line[i]) );

    // redraw path with only stored points
    m_item->setPath( createSplinePath() );

    // check if valid
    if (m_line.size() < 3)
        deleteLater();
}

QPainterPath EditorCurveItem::createSplinePath(const QVector<QPointF> &ln)
{
    QVector<QPointF> line(ln);
    if ( line.isEmpty() )
        line = m_line.toVector();

    int nbPoints = line.size();

    QPainterPath path;

    if(nbPoints<=0)
        return path;

    if(nbPoints==2)
    {
        path.moveTo( mapFromScene( line.at(0) ) );
        path.lineTo( mapFromScene( line.at(1) ) );
    }
    else
    {
        int i;

        path.moveTo( mapFromScene( line.at(0) ) );
        path.lineTo( mapFromScene( nextMiddlePoint(0,line) ) );
        for(i = 1; i<(nbPoints - 1); i++)
            path.quadTo( mapFromScene( line.at(i) ),mapFromScene( nextMiddlePoint(i,line) ) );
        path.lineTo( mapFromScene( line.at(nbPoints - 1) ) );
    }

    return path;
}

//----------------------------------------------------------------------------
QPointF EditorCurveItem::nextMiddlePoint(int index, const QVector<QPointF> &line) const
{
    QPointF point1 = line.at(index);
    QPointF point2 = line.at( (index + 1) % line.size() );
    return ( point1 + 0.5 * (point2 - point1) );
}

void EditorCurveItem::toXml(QDomElement &elt) const
{
    QString data;
    for (int i = 0; i < m_line.size(); i++)
        data += IOHelper::variantToString( mapToScene(m_line[i]) ) + ";";
    elt.setAttribute( "coords", data);
    elt.setAttribute( "color", IOHelper::variantToString( m_item->pen().color() ) );
    elt.setAttribute( "width", IOHelper::variantToString( m_item->pen().width() ) );
    //elt.setAttribute( "style", IOHelper::variantToString( m_item->pen().style() ) );
    elt.setAttribute( "shadow", IOHelper::variantToString( isShadowEnabled() ) );
}

void EditorCurveItem::fromXml(const QDomElement &elt)
{
    setPos( QPointF(0,0) );
    QStringList coords = elt.attribute("coords").split(";",QString::SkipEmptyParts);
    m_line.clear();
    qDeleteAll(m_handgrips);
    m_handgrips.clear();

    m_initialized = true;
    bool ok = true;
    foreach (const QString &coord, coords)
    {
        m_line << IOHelper::stringToVariant(coord,QVariant::PointF,ok).toPointF();
        m_handgrips << new HandgripItem(this,this,m_line.size() - 1);
        m_handgrips.last()->hide();
        connect( m_handgrips.last(), SIGNAL( moved(const QPointF &) ),this, SLOT( updatePoint(const QPointF &) ) );
        m_handgrips.last()->setPos( m_line.last() );

        // detect press/release point for undo translation
        connect( m_handgrips.last(), SIGNAL( initPos(const QPointF &) ),this, SLOT( pressPoint(const QPointF &) ) );
        connect( m_handgrips.last(), SIGNAL( releasePos(const QPointF &) ),this, SLOT( releasePoint(const QPointF &) ) );
    }

    setEnableShadow( IOHelper::stringToVariant(elt.attribute("shadow","true"),QVariant::Bool,ok).toBool() );
    QColor color = IOHelper::stringToVariant(elt.attribute("color"),QVariant::Color,ok).value<QColor>();
    int width = elt.attribute("width").toInt();
    int style = elt.attribute("style").toInt();
    m_pen = QPen(color,width,(Qt::PenStyle)style);
    m_pen.setCapStyle(Qt::RoundCap);
    m_pen.setJoinStyle(Qt::RoundJoin);

    //pen.setCosmetic(true);
    setPen(m_pen);

    m_item->setPath( createSplinePath() );
}

// copy constructor
BaseItem * EditorCurveItem::clone()
{
    EditorCurveItem *copiedItem = new  EditorCurveItem();
    copyDataTo(copiedItem);
    return copiedItem;
}
