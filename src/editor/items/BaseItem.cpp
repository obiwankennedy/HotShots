
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

#include <QColor>
#include <QGraphicsDropShadowEffect>
#include <QKeyEvent>

#include <cmath>

#include "BaseItem.h"
#include "HandgripItem.h"
#include "EditorUndoCommand.h"
#include "EditorScene.h"

BaseItem::BaseItem(QGraphicsItem *parent)
    : QGraphicsObject(parent),
    m_initialized(false),
    m_currUndo(NULL)
{
    m_dropShadowFx = new QGraphicsDropShadowEffect();
    m_dropShadowFx->setColor( QColor(63, 63, 63, 220) );
    m_dropShadowFx->setBlurRadius(10);
    m_dropShadowFx->setOffset( QPointF(3, 3) );
}

QRectF BaseItem::boundingRect() const
{
    return childrenBoundingRect ();
}

void BaseItem::setEnableShadow(bool val)
{
    m_dropShadowFx->setEnabled(val);
}

bool BaseItem::isShadowEnabled() const
{
    return m_dropShadowFx->isEnabled();
}

void BaseItem::paint(QPainter *                      painter,
                     const QStyleOptionGraphicsItem *option,
                     QWidget *                       widget)
{
    Q_UNUSED(painter);
    Q_UNUSED(option);
    Q_UNUSED(widget);
}

QPen BaseItem::pen() const
{
    return m_pen;
}

QBrush BaseItem::brush() const
{
    return m_brush;
}

QFont BaseItem::font() const
{
    return m_font;
}

void BaseItem::setPen(const QPen &p)
{
    m_pen = p;
}

void BaseItem::setBrush(const QBrush &b)
{
    m_brush = b;
}

void BaseItem::setFont(const QFont &f)
{
    m_font = f;
}

const QString & BaseItem::getType () const
{
    return m_type;
}

QVariant BaseItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemSelectedHasChanged)
    {
        bool selected = value.toBool();
        if (selected)
            showHandgrips();
        else
            hideHandgrips();
    }

    return QGraphicsObject::itemChange(change, value);
}

void BaseItem::showHandgrips()
{
    qDebug() << "BaseItem::showHandgrips";
    for(int i = 0; i<m_handgrips.size(); ++i)
        m_handgrips.at(i)->fadeIn();
}

void BaseItem::hideHandgrips()
{
    qDebug() << "BaseItem::hideHandgrips";
    for(int i = 0; i<m_handgrips.size(); ++i)
        m_handgrips.at(i)->hide();
}

void BaseItem::endEdition()
{
}

void BaseItem::mouseDoubleClick ( QGraphicsSceneMouseEvent * mouseEvent )
{
    Q_UNUSED(mouseEvent);
}

QVariant BaseItem::getItemSetting (const QString &key, const QVariant &defaultValue) const
{
    if ( m_itemSettings.contains(key) )
        return m_itemSettings[key];

    return defaultValue;
}

void BaseItem::setItemSettings(const QMap<QString, QVariant>&s)
{
    m_itemSettings = s;

    // take into account local settings
    processSettings();
}

void BaseItem::processSettings()
{
}

void BaseItem::copyDataTo(BaseItem *item)
{
    QDomDocument doc;
    QDomElement elt = doc.createElement( "dummy" );
    doc.appendChild( elt );
    toXml(elt);
    item->fromXml(elt);
}

void BaseItem::forceSelection()
{
    setFlag(QGraphicsItem::ItemIsSelectable,true);
    setFlag(QGraphicsItem::ItemIsMovable,true);
    setFocus(Qt::MouseFocusReason);
    setSelected(true);
}

void BaseItem::insertPoint(int index, const QPointF &pos, QList<QPointF> & line)
{
    line.insert(index,pos);

    m_handgrips.insert( index,new HandgripItem(this,this,index) );

    // update tag for next handgrip in the list
    for (int i = index + 1; i < m_handgrips.size(); i++)
        m_handgrips[i]->setId(i);

    // m_handgrips[i2]->hide();
    connect( m_handgrips[index], SIGNAL( moved(const QPointF &) ),this, SLOT( updatePoint(const QPointF &) ) );

    m_handgrips[index]->setPos( line[index] );

    // detect press/release point for undo translation
    connect( m_handgrips[index], SIGNAL( initPos(const QPointF &) ),this, SLOT( pressPoint(const QPointF &) ) );
    connect( m_handgrips[index], SIGNAL( releasePos(const QPointF &) ),this, SLOT( releasePoint(const QPointF &) ) );
}

void BaseItem::deletePoint(int index, QList<QPointF> & line)
{
    if ( line.isEmpty() )
        return;

    line.takeAt(index);
    delete m_handgrips.takeAt(index);

    // update tag for next handgrip in the list
    for (int i = index; i < m_handgrips.size(); i++)
        m_handgrips[i]->setId(i);
}

bool BaseItem::isPointSelected(const QPointF &newPos, QList<QPointF> & line, int & index, qreal selectDist )
{
    // first check if press point isn't on an existing vertex
    for(int i = 0; i < line.size(); i++)
    {
        qreal distX = newPos.x() - line[i].x();
        qreal distY = newPos.y() - line[i].y();
        qreal dist = sqrtf(distX * distX + distY * distY);
        if (dist < selectDist)
        {
            index = i;
            return true;
        }
    }
    return false;
}

qreal BaseItem::distancePointToLine(const QPointF &p, const QPointF &a, const QPointF &b)
{
    float A = p.x() - a.x();
    float B = p.y() - a.y();
    float C = b.x() - a.x();
    float D = b.y() - a.y();

    return ( fabs(A * D - C * B) / sqrtf(C * C + D * D) );
}

bool BaseItem::createPointIfNeeded(const QPointF &newPos, QList<QPointF> & line, bool loop )
{
    if (line.size() < 2)
        return false;

    qreal minDist = 10.0f;

    // check if new point is on a segment
    int max = line.size() - 1;
    if (loop)
        max++;
    for(int i = 0; i < max; i++)
    {
        int i1 = i;
        int i2 = i + 1;
        if (loop)
        {
            i1 %= line.size();
            i2 %= line.size();
        }

        if ( !QRectF(line[i1],line[i2]).normalized().contains(newPos) )
            continue;

        if (abs( distancePointToLine(newPos, line[i1],line[i2]) ) < minDist)
        {
            insertPoint(i2,newPos,line);
            return true;
        }
    }

    return false;
}

void BaseItem::prepareUndo()
{
    return; // TODO !!!!!!!!!!!!!!!!!!!!!

    if (m_currUndo)
        delete m_currUndo;
    m_currUndo = new EditorUndoCommand(this);
}

void BaseItem::saveUndo()
{
    return; // TODO !!!!!!!!!!!!!!!!!!!!!

    if (!m_currUndo)
        return;

    // only save if needed
    QDomDocument doc;
    QDomElement nextState = doc.createElement( "dummy" );
    toXml(nextState);
    if ( !EditorUndoCommand::equal(m_currUndo->getPrevState(),nextState) )
    {
        // push undo command
        qDebug() << "push undo command";
        EditorScene *scn = qobject_cast<EditorScene *>( scene() );
        m_currUndo->getNextState() = nextState;
        scn->getUndoStack()->push(m_currUndo);
    }
    else
    {
        delete m_currUndo;
        qDebug() << "no change";
    }

    m_currUndo = NULL;
}

void BaseItem::pressPoint(const QPointF &)
{
    prepareUndo();
}

void BaseItem::releasePoint(const QPointF &)
{
    saveUndo();
}

void BaseItem::updateHandgrips()
{
    // connect handgrip if needed
    foreach (HandgripItem * item, m_handgrips)
    {
        connect( item, SIGNAL( moved(const QPointF &) ),this, SLOT( updatePoint(const QPointF &) ) );
        if (item->getType() == HandgripItem::ROTATOR)
        {
            // detect press/release point for rotation
            connect( item, SIGNAL( initPos(const QPointF &) ),this, SLOT( initRotationReferencePoint(const QPointF &) ) );
            connect( item, SIGNAL( releasePos(const QPointF &) ),this, SLOT( validRotationReferencePoint(const QPointF &) ) );
        }
        else
        {
            // detect press/release point for undo translation
            connect( item, SIGNAL( initPos(const QPointF &) ),this, SLOT( pressPoint(const QPointF &) ) );
            connect( item, SIGNAL( releasePos(const QPointF &) ),this, SLOT( releasePoint(const QPointF &) ) );
        }
    }
    hideHandgrips();
}

QTransform BaseItem::makeRotation(double angle, const QRectF &rect)
{
    return makeRotation(angle,rect.center());
}

QTransform BaseItem::makeRotation(double angle, const QPointF &anchor)
{
    QTransform m;

    m.translate( anchor.x(),anchor.y() );
    m.rotate(angle);
    m.translate( -anchor.x(),-anchor.y() );

    return m;
}

