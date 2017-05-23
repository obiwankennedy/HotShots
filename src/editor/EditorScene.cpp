
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
#include <QtCore/QFile>
#include <QtCore/QFileInfo>

#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>
#include <QGraphicsItem>
#include <QTextCursor>
#include <QApplication>

#include <QGraphicsSimpleTextItem>
#include <QGraphicsPathItem>
#include <QGraphicsTextItem>
#include <QUndoStack>

#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>

#include "EditorScene.h"
#include "ItemFactory.h"
#include "ItemRegistering.h"

#include "GroupLayer.h"
#include "BaseItem.h"
#include "TagManager.h"

#include "WidgetFillStyle.h"

#include "LogHandler.h"

#ifdef Q_OS_WIN
#include <direct.h>
#else
#include <unistd.h>
#endif

#if !defined(CLAMP)
#define  CLAMP(x,min,max)   ( (x<min) ? min : ( (x>max) ? max : x ) )
#endif

EditorScene::EditorScene(QObject *parent) : QGraphicsScene(parent),
    m_typElem("Select"),
    m_modeEdition(BaseItem::EDITION_NONE),
    m_primitive(NULL),
    m_fgColor( QColor(Qt::black) ),
    m_lineWidth(1),
    m_dashStyle(Qt::SolidLine),
    m_fillStyle(WidgetFillStyle::NoFill),
    m_shadow(false),
    m_multiSelection(false),
    m_factory(new ItemFactory),
    m_undoStack( new QUndoStack(this) )
{
    m_pen.setColor(m_fgColor);
    m_pen.setWidth(m_lineWidth);
    m_pen.setStyle( (Qt::PenStyle)m_dashStyle );
    m_pen.setCapStyle(Qt::RoundCap);
    m_pen.setJoinStyle(Qt::RoundJoin);

    setBrushFromStyle();

    m_groupUnderlay = new GroupLayer;
    addItem(m_groupUnderlay);
    m_groupOverlay = new GroupLayer;
    addItem(m_groupOverlay);

    connect( this,SIGNAL( selectionChanged() ),this,SLOT( innerSelectionChanged() ) );

    ItemRegistering::allRegister(m_factory);
}

EditorScene::~EditorScene()
{
    delete m_factory;
}

void EditorScene::setItemPen(const QPen &pen)
{
    m_pen = pen;
}

QPixmap EditorScene::getUnderlayPixmap() const
{
    QGraphicsPixmapItem* pixItem;
    if ( !m_groupUnderlay->childItems().isEmpty() && ( pixItem = qgraphicsitem_cast<QGraphicsPixmapItem*>(m_groupUnderlay->childItems()[0]) ) )
        return pixItem->pixmap();

    return QPixmap();
}

QImage EditorScene::getUnderlayImage() const
{
    QGraphicsPixmapItem* pixItem;
    if ( !m_groupUnderlay->childItems().isEmpty() && ( pixItem = qgraphicsitem_cast<QGraphicsPixmapItem*>(m_groupUnderlay->childItems()[0]) ) )
        return pixItem->pixmap().toImage();

    return QImage();
}

QPointF EditorScene::getUnderlayOffset() const
{
    QGraphicsPixmapItem* pixItem;
    if ( !m_groupUnderlay->childItems().isEmpty() && ( pixItem = qgraphicsitem_cast<QGraphicsPixmapItem*>(m_groupUnderlay->childItems()[0]) ) )
        return pixItem->pos();

    return QPointF();
}

void EditorScene::setUnderlayImage(const QImage &image, const QPointF &pos)
{
    qDeleteAll( m_groupUnderlay->childItems() );

    QGraphicsPixmapItem *item = new QGraphicsPixmapItem( QPixmap::fromImage(image) );
    item->setTransformationMode(Qt::SmoothTransformation);
    item->setPos(pos);
    m_groupUnderlay->addChildItem(item);

    setSceneRect( image.rect() );
}

void EditorScene::setItemBrush(const QBrush &brush)
{
    m_brush = brush;
}

void EditorScene::setFont(const QFont &font)
{
    m_font = font;
    applyChanges();
}

void EditorScene::innerSelectionChanged()
{
    qDebug() << "EditorScene::innerSelectionChanged" << selectedItems().size();
    QList<QGraphicsItem*> items = selectedItems();

    /*
       foreach (QGraphicsItem *item, items)
       {
        BaseItem *bi = dynamic_cast<BaseItem *>(item);
        if (bi)
            bi->prepareUndo();
       }
     */

    if ( items.count() != 1 )
    {
        m_primitive = NULL;
        return;
    }
    else if (items.count() == 0)
    {
        clearFocus(); // especially for text item
    }

    m_primitive = dynamic_cast<BaseItem *>(items[0]);
    if (m_primitive)
    {
        // get base data
        m_pen = m_primitive->pen();

        m_brush = m_primitive->brush();
        m_font = m_primitive->font();

        m_fgColor = m_pen.color();
        m_bgColor = m_brush.color();
        m_lineWidth = m_pen.width();
        m_dashStyle = m_pen.style();

        m_fillStyle = getStyleFromBrush();
    }
}

void EditorScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    qDebug() << "EditorScene::mousePressEvent " << selectedItems().count() << " " << m_primitive;

    bool intercepted = false;

    /*
        QList<QGraphicsItem*> items = selectedItems();


        foreach (QGraphicsItem *item, items)
        {
            BaseItem *bi = dynamic_cast<BaseItem *>(item);
            if (bi)
                bi->prepareUndo();
        }
     */

    if (m_typElem != "Select" && m_modeEdition == BaseItem::EDITION_NEXTITEM)
    {
        if (m_primitive)
            m_primitive->endEdition();

        m_primitive = m_factory->create(m_typElem);
        if (m_primitive)
        {
            m_primitive->setPen(m_pen);
            m_primitive->setBrush(m_brush);
            m_primitive->setFont(m_font);
            m_primitive->setItemSettings(m_itemSettings);
            m_groupOverlay->addChildItem(m_primitive);
        }
        intercepted = true;
    }

    if (m_primitive)
    {
        m_primitive->mousePress(mouseEvent);
        intercepted = true;
    }

    if (intercepted)
        mouseEvent->accept();

    QGraphicsScene::mousePressEvent(mouseEvent);
}

void EditorScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    bool intercepted = false;

    if (m_typElem != "Select" && m_primitive)
    {
        m_primitive->mouseMove(mouseEvent);
    }

    if (intercepted)
        mouseEvent->accept();
    else
        QGraphicsScene::mouseMoveEvent(mouseEvent);
}

void EditorScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    qDebug() << "EditorScene::mouseReleaseEvent " << selectedItems().count() << " " << m_primitive;
    bool intercepted = false;

    if (m_primitive && m_typElem != "Select")
        m_modeEdition = m_primitive->mouseRelease(mouseEvent);

    //m_resetPrimitive = m_primitive->mouseRelease(mouseEvent);

    if (m_modeEdition == BaseItem::EDITION_NEXTITEM)
    {
        intercepted = true;

        if (m_primitive)
        {
            m_primitive->endEdition();
        }
        m_primitive = NULL; // in order to change pen or brush after creation
        emit newColorPair(m_fgColor,m_bgColor);
    }
    else if (m_modeEdition == BaseItem::EDITION_FINISHITEM)
    {
        if (m_primitive)
        {
            m_primitive->endEdition();
        }
        m_primitive = NULL; // in order to change pen or brush after creation

        setMode("Select");
    }

    {
        /*
            QList<QGraphicsItem*> items = selectedItems();
            foreach (QGraphicsItem *item, items)
            {
                BaseItem *bi = dynamic_cast<BaseItem *>(item);
                if (bi)
                    bi->saveUndo();
            }
         */
    }

    if (intercepted)
        mouseEvent->accept();

    QGraphicsScene::mouseReleaseEvent(mouseEvent);
}

void EditorScene::keyPressEvent(QKeyEvent *event)
{
    EditorScene::typMove action = EditorScene::MOVE_NONE;

    if ( m_typElem == "Select" && event->modifiers() & ( Qt::ControlModifier ) )
    {
        //QApplication::setOverrideCursor( QCursor(Qt::PointingHandCursor) );
        m_multiSelection = true;
        changeDragMode(QGraphicsView::RubberBandDrag);

//          QGraphicsScene::keyPressEvent(event);
//          return;
    }

    // no selection => default action
    if ( selectedItems().isEmpty() ||
            ( selectedItems().size() == 1 && selectedItems()[0]->data(0).toBool() ) ) // ugly hack for textitem in order to move text cursor with keyboard
    {
        QGraphicsScene::keyPressEvent(event);
        return;
    }

    switch ( event->key() )
    {
    case Qt::Key_Left:
        if ( event->modifiers() & ( Qt::ShiftModifier | Qt::ControlModifier ) )
            action = EditorScene::MOVE_LEFT_FAST;
        else
            action = EditorScene::MOVE_LEFT;
        break;
    case Qt::Key_Right:
        if ( event->modifiers() & ( Qt::ShiftModifier | Qt::ControlModifier ) )
            action = EditorScene::MOVE_RIGHT_FAST;
        else
            action = EditorScene::MOVE_RIGHT;
        break;
    case Qt::Key_Down:
        if ( event->modifiers() & ( Qt::ShiftModifier | Qt::ControlModifier ) )
            action = EditorScene::MOVE_DOWN_FAST;
        else
            action = EditorScene::MOVE_DOWN;
        break;
    case Qt::Key_Up:
        if ( event->modifiers() & ( Qt::ShiftModifier | Qt::ControlModifier ) )
            action = EditorScene::MOVE_UP_FAST;
        else
            action = EditorScene::MOVE_UP;
        break;
    default:
        break;
    }

    if (action != EditorScene::MOVE_NONE)
        moveSelectedLayers(action);
    else
        QGraphicsScene::keyPressEvent(event);
}

void EditorScene::keyReleaseEvent(QKeyEvent *event)
{
    if ( m_multiSelection && !( event->modifiers() & ( Qt::ControlModifier ) ) )
    {
        //QApplication::restoreOverrideCursor();
        changeDragMode(QGraphicsView::ScrollHandDrag);
        m_multiSelection = false;
    }
    QGraphicsScene::keyReleaseEvent(event);
}

void EditorScene::resetSelection()
{
    clearSelection();
}

void EditorScene::setMode(const QString &m)
{
    m_typElem = m;
    m_modeEdition = BaseItem::EDITION_NEXTITEM;
    if (m_primitive)
        m_primitive->endEdition();
    m_primitive = NULL;

    resetSelection();

    clearFocus();

    // restore cursor
    if ( QApplication::overrideCursor() )
        QApplication::restoreOverrideCursor();

    // change cursor and drag mode according to state
    if (m_typElem == "Select")
    {
        m_modeEdition = BaseItem::EDITION_NONE;
        changeDragMode(QGraphicsView::ScrollHandDrag);
        enableSelection(true);
    }
    else
    {
        QApplication::setOverrideCursor( QCursor(Qt::CrossCursor) );
        changeDragMode(QGraphicsView::NoDrag);
        enableSelection(false);
    }
    emit currentMode(m);
}

void EditorScene::setLineWidth(int val)
{
    m_lineWidth = val;
    m_pen.setWidth(m_lineWidth);
    applyChanges();
}

void EditorScene::setDashStyle(int val)
{
    m_dashStyle = val;
    m_pen.setStyle( (Qt::PenStyle)m_dashStyle );
    applyChanges();
}

void EditorScene::setFillStyle(int val)
{
    m_fillStyle = val;
    setBrushFromStyle();
    applyChanges();
}

void EditorScene::increaseFontSize()
{
    int ps = m_font.pointSize();
    m_font.setPointSize(ps + 1);
    applyChanges();
}

void EditorScene::decreaseFontSize()
{
    int ps = m_font.pointSize();
    m_font.setPointSize(ps - 1);
    applyChanges();
}

void EditorScene::setColorPair(const QColor&fg, const QColor &bg)
{
    setFgColor(fg);
    setBgColor(bg);
}

void EditorScene::setFgColor(const QColor &color)
{
    m_fgColor = color;
    m_pen.setColor(m_fgColor);
    setBrushFromStyle();
    applyChanges();
}

void EditorScene::setBgColor(const QColor &color)
{
    m_bgColor = color;
    m_brush.setColor(m_bgColor);
    setBrushFromStyle();
    applyChanges();
}

void EditorScene::applyChanges()
{
    QList<QGraphicsItem*> items = selectedItems();

    if ( !items.isEmpty() )
    {
        foreach (QGraphicsItem * item, items)
        {
            BaseItem *primitive = dynamic_cast<BaseItem *>(item);
            if (primitive)
            {
                primitive->setPen(m_pen);
                primitive->setBrush(m_brush);
                primitive->setFont(m_font);
            }
        }
    }
    else if (m_primitive)
    {
        m_primitive->setPen(m_pen);
        m_primitive->setBrush(m_brush);
        m_primitive->setFont(m_font);
    }
}

int EditorScene::getStyleFromBrush() const
{
    int alpha = m_brush.color().alpha();
    if (m_brush.style() == Qt::NoBrush)
        return WidgetFillStyle::NoFill;
    else if (alpha != 255)
        return WidgetFillStyle::FillWithTranslucentBackground;

    return WidgetFillStyle::FillWithBackground;
}

void EditorScene::setBrushFromStyle()
{
    switch (m_fillStyle)
    {
    case WidgetFillStyle::NoFill:
    {
        m_brush.setColor(m_bgColor);
        m_brush.setStyle(Qt::NoBrush);
        break;
    }
    case WidgetFillStyle::FillWithTranslucentBackground:
    {
        m_brush.setColor( QColor(m_bgColor.red(),m_bgColor.green(),m_bgColor.blue(),100) );
        m_brush.setStyle(Qt::SolidPattern);
        break;
    }
    case WidgetFillStyle::FillWithBackground:
    {
        m_brush.setColor(m_bgColor);
        m_brush.setStyle(Qt::SolidPattern);
        break;
    }
    }
}

void EditorScene::changeDragMode(QGraphicsView::DragMode mode)
{
    foreach ( QGraphicsView * view, views() )
    view->setDragMode(mode);
}

void EditorScene::addOverlayItem(BaseItem *item)
{
    m_groupOverlay->addChildItem(item);
}

QList<QGraphicsItem *>  EditorScene::getOverlayItems() const
{
    return m_groupOverlay->childItems ();
}

void EditorScene::enableSelection(bool val)
{
    QList<QGraphicsItem *>   items = m_groupOverlay->childItems ();
    foreach (QGraphicsItem * item, items)
    {
        item->setFlag(QGraphicsItem::ItemIsSelectable,val);
        item->setFlag(QGraphicsItem::ItemIsMovable,val);
    }
}

void EditorScene::toggleShadow()
{
    //m_shadow = val;
    //QList<QGraphicsItem *>   items = m_groupOverlay->childItems ();
    QList<QGraphicsItem*> items = selectedItems();
    foreach (QGraphicsItem * item, items)
    {
        BaseItem *it = dynamic_cast<BaseItem*>(item);
        if (it)
            it->setEnableShadow( !it->isShadowEnabled() );
    }
}

void EditorScene::deleteSelection()
{
    foreach ( QGraphicsItem * item, selectedItems() )
    {
        BaseItem *it = dynamic_cast<BaseItem*>(item);

        if (it)
            it->deleteLater();
        else
            delete item;
    }
    m_primitive = NULL;
}

void EditorScene::cloneSelection()
{
    foreach ( QGraphicsItem * item, selectedItems() )
    {
        BaseItem *it = dynamic_cast<BaseItem*>(item);

        if (it)
        {
            BaseItem *newItem = it->clone();
            if (newItem)
            {
                // enable the selection
                newItem->setFlag(QGraphicsItem::ItemIsSelectable,true);
                newItem->setFlag(QGraphicsItem::ItemIsMovable,true);
                newItem->setPos( newItem->pos() + QPointF(20,20) );
                m_groupOverlay->addChildItem(newItem);
            }
        }
    }
    m_primitive = NULL;
}

bool EditorScene::hasItems() const
{
    return m_groupOverlay->childItems ().isEmpty() == false;
}

void EditorScene::clearScene()
{
    clearItems();
    qDeleteAll( m_groupUnderlay->childItems() );
}

void EditorScene::clearItems()
{
    m_primitive = NULL;
    TagManager::reset(); // to prevent unwanted update of tag items
    resetSelection();
    qDeleteAll( m_groupOverlay->childItems () );
    m_undoStack->clear();
}

void EditorScene::placeSelectedLayers(typZPosition pos)
{
    QList<QGraphicsItem*> items = selectedItems();

    int offsetIndex = 0; // in order to preserve relative position of group of elements

    foreach (QGraphicsItem * item, items)
    {
        // get the parent item
        QGraphicsItem *parentItem = m_groupOverlay;

        // get the list of sibling items
        QList<QGraphicsItem *> lchild = parentItem->childItems ();

        // get current position of item
        int stackNum = lchild.indexOf(item);

        qreal targetPos = 0;

        switch (pos)
        {
        case POSITION_TOP:
        {
            targetPos = CLAMP(lchild.size() - 1 - offsetIndex, 0,lchild.size() - 1);
            for (int i = stackNum + 1; i <= targetPos; i++)
                lchild[i]->stackBefore(item);
            break;
        }
        case POSITION_UP:
        {
            targetPos = CLAMP(stackNum + 1, 0,lchild.size() - 1);
            lchild[targetPos]->stackBefore(item);
            break;
        }
        case POSITION_DOWN:
        {
            targetPos = CLAMP(stackNum - 1, 0,lchild.size() - 1);
            item->stackBefore(lchild[targetPos]);
            break;
        }
        case POSITION_BOTTOM:
        {
            targetPos = CLAMP(0 + offsetIndex, 0,lchild.size() - 1);
            item->stackBefore(lchild[targetPos]);
            break;
        }
        default:
            break;
        }
        offsetIndex++;
        parentItem->update();
    }
    update();
}

void EditorScene::moveSelectedLayers(typMove pos)
{
    QList<QGraphicsItem*> items = selectedItems();
    foreach (QGraphicsItem * item, items)
    {
        switch (pos)
        {
        case MOVE_UP:
            item->setPos( item->pos() + QPointF(0,-1) );
            break;
        case MOVE_DOWN:
            item->setPos( item->pos() + QPointF(0,1) );
            break;
        case MOVE_RIGHT:
            item->setPos( item->pos() + QPointF(1,0) );
            break;
        case MOVE_LEFT:
            item->setPos( item->pos() + QPointF(-1,0) );
            break;
        case MOVE_UP_FAST:
            item->setPos( item->pos() + QPointF(0,-10) );
            break;
        case MOVE_DOWN_FAST:
            item->setPos( item->pos() + QPointF(0,10) );
            break;
        case MOVE_LEFT_FAST:
            item->setPos( item->pos() + QPointF(-10,0) );
            break;
        case MOVE_RIGHT_FAST:
            item->setPos( item->pos() + QPointF(10,0) );
            break;
        default:
            break;
        }
    }
}

QPixmap EditorScene::getRenderToPixmap()
{
    resetSelection ();

    bool cropped = false;
    int margin = 5;

    if (cropped)
    {
        QPixmap p( width(), height() );
        QPainter painter(&p);
        painter.setRenderHint(QPainter::Antialiasing);
        render(&painter);
        return p;
    }

    QRectF rect(itemsBoundingRect());
    rect.adjust(-margin,-margin,margin,margin);

    QPixmap p( rect.width(), rect.height() );
    p.fill(Qt::transparent);
    QPainter painter(&p);
    painter.setRenderHint(QPainter::Antialiasing);
    render( &painter,p.rect(),rect );
    return p;
}

void EditorScene::setItemSetting(const QString &key, const QVariant &val)
{
    // settings data
    m_itemSettings[key] = val;
}

void EditorScene::cropBackground(const QRectF &cropRect)
{
    // cropRect is in scene coord
    foreach ( QGraphicsItem * item, m_groupUnderlay->childItems() )
    {
        QGraphicsPixmapItem* pixItem;
        if ( ( pixItem = qgraphicsitem_cast<QGraphicsPixmapItem*>(item) ) != NULL )
        {
            QRect itemCropRect = pixItem->mapFromScene(cropRect).boundingRect().toRect();
            QRect pixRect = pixItem->pixmap().rect().intersected( itemCropRect );
            pixItem->setPixmap( pixItem->pixmap().copy(pixRect) );
            pixItem->setPos( pixItem->pos() + pixRect.topLeft() );
        }
    }
    emit backgroundChanged();
}

QFont EditorScene::font() const
{
    return m_font;
}

QColor EditorScene::fgColor() const
{
    return m_fgColor;
}

QColor EditorScene::bgColor() const
{
    // must return color without alpha !!
    return QColor( m_bgColor.red(),m_bgColor.green(),m_bgColor.blue() );
}

QBrush EditorScene::itemBrush() const
{
    return m_brush;
}

QPen EditorScene::itemPen() const
{
    return m_pen;
}

int EditorScene::getLineWidth() const
{
    return m_lineWidth;
}

int EditorScene::getDashStyle() const
{
    return m_dashStyle;
}

int EditorScene::getFillStyle() const
{
    return m_fillStyle;
}

const QString & EditorScene::getMode() const
{
    return m_typElem;
}

ItemFactory *EditorScene::getItemFactory() const
{
    return m_factory;
}

void EditorScene::setAlignment(typGroupAlignment align)
{
    QList<QGraphicsItem*> items = selectedItems();

    if (items.count() < 2)
        return;

    QRectF bb(items.at(0)->sceneBoundingRect());
    foreach (QGraphicsItem *item, items)
    {
        bb = bb.united(item->sceneBoundingRect());
        qDebug() << item->scenePos();
    }

    qDebug() << bb;

    foreach (QGraphicsItem *item, items)
    {
        QRectF br(item->sceneBoundingRect());
        QPointF newPos;
        QPointF oldPos(item->scenePos());
        switch (align)
        {
        case ALIGN_LEFT:
        {
            newPos = QPointF(bb.left()-br.left(),0);
            break;
        }
        case ALIGN_V_CENTER:
        {
            newPos = QPointF(bb.center().x()-br.center().x(),0);
            break;
        }
        case ALIGN_RIGHT:
        {
            newPos = QPointF(bb.right()-br.right(),0);
            break;
        }
        case ALIGN_TOP:
        {
            newPos = QPointF(0,bb.top()-br.top());
            break;
        }
        case ALIGN_H_CENTER:
        {
            newPos = QPointF(0,bb.center().y()-br.center().y());
            break;
        }
        default:
        case ALIGN_BOTTOM:
        {
            newPos = QPointF(0,bb.bottom()-br.bottom());
            break;
        }
        }
        item->setPos(item->mapToParent(newPos));
    }
}
