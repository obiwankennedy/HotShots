
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

#ifndef _EDITORSCENE_H_
#define _EDITORSCENE_H_

#include <QGraphicsScene>
#include <QtCore/QMap>
#include <QtCore/QPointer>
#include <QtCore/QSet>
#include <QGraphicsView>
#include <QtXml/QDomNamedNodeMap>

class QGraphicsSceneMouseEvent;
class QMenu;
class QPointF;
class QGraphicsLineItem;
class QFont;
class QGraphicsTextItem;
class QColor;
class GroupLayer;
class QUndoStack;

class ItemFactory;
class BaseItem;

class EditorScene : public QGraphicsScene
{
    Q_OBJECT

public:

    enum typZPosition
    {
        POSITION_TOP = 0,
        POSITION_UP,
        POSITION_DOWN,
        POSITION_BOTTOM
    };

    enum typMove
    {
        MOVE_NONE = 0,
        MOVE_UP,
        MOVE_DOWN,
        MOVE_RIGHT,
        MOVE_LEFT,
        MOVE_UP_FAST,
        MOVE_DOWN_FAST,
        MOVE_LEFT_FAST,
        MOVE_RIGHT_FAST
    };

    enum typGroupAlignment
    {
        ALIGN_LEFT = 0,
        ALIGN_H_CENTER,
        ALIGN_RIGHT,
        ALIGN_TOP,
        ALIGN_V_CENTER,
        ALIGN_BOTTOM
    };

    EditorScene(QObject *parent = 0);
    ~EditorScene();

    // Getters
    QFont font() const;

    QColor fgColor() const;

    QColor bgColor() const;

    QBrush itemBrush() const;

    QPen itemPen() const;

    int getLineWidth() const;

    int getDashStyle() const;

    int getFillStyle() const;

    const QString & getMode() const;

    ItemFactory *getItemFactory() const;

    void addOverlayItem(BaseItem *);

    QPixmap getRenderToPixmap();

    QUndoStack* getUndoStack(void) const { return m_undoStack; }

public slots:

    // Setters
    void setItemPen(const QPen &color);
    void setFgColor(const QColor &color);
    void setBgColor(const QColor &color);
    void setItemBrush(const QBrush &color);
    void setFont(const QFont &font);

    void setLineWidth(int);
    void setDashStyle(int);
    void setFillStyle(int);

    void setUnderlayImage( const QImage &, const QPointF &pos = QPointF() );
    QImage getUnderlayImage() const;
    QPixmap getUnderlayPixmap() const;
    QPointF getUnderlayOffset() const;

    QList<QGraphicsItem *>  getOverlayItems() const;

    void setMode(const QString & m);
    void setItemSetting(const QString &key, const QVariant &value);

    void toggleShadow();
    void deleteSelection();
    void cloneSelection();
    void resetSelection();
    void clearItems();
    void clearScene();
    bool hasItems() const;

    void placeSelectedLayers(typZPosition pos);
    void moveSelectedLayers(typMove pos);

    void increaseFontSize();
    void decreaseFontSize();

    void cropBackground(const QRectF &);
    void setColorPair(const QColor&fg, const QColor &bg);

    void setAlignment(typGroupAlignment);

signals:

    void currentMode(const QString &);
    void backgroundChanged();
    void newColorPair(const QColor&fg, const QColor &bg);

protected:

    virtual void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent);
    virtual void keyPressEvent(QKeyEvent *event);
    virtual void keyReleaseEvent(QKeyEvent *event);

protected slots:

    void innerSelectionChanged();

private:

    void applyChanges();
    void setBrushFromStyle();
    int getStyleFromBrush() const;
    void changeDragMode(QGraphicsView::DragMode);
    void enableSelection(bool);

    QString m_typElem;

    //bool m_resetPrimitive;
    int m_modeEdition;

    //Current primitive
    QPointer<BaseItem> m_primitive;

    QFont m_font;
    QColor m_fgColor;
    QColor m_bgColor;

    QBrush m_brush;
    QPen m_pen;
    int m_lineWidth;
    int m_dashStyle;
    int m_fillStyle;
    GroupLayer *m_groupUnderlay;
    GroupLayer *m_groupOverlay;
    bool m_shadow;
    bool m_multiSelection;

    ItemFactory *m_factory;

    // item setting data
    QMap<QString, QVariant> m_itemSettings;
    QUndoStack *m_undoStack;
};

#endif // _EDITORSCENE_H_
