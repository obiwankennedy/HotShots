
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

#ifndef _EDITORVIEW_H_
#define _EDITORVIEW_H_

// Qt
#include <QGraphicsView>
#include <QtCore/QPointF>

class EditorView : public QGraphicsView
{
    Q_OBJECT

public:

    EditorView(QWidget * map);
    ~EditorView();

    //virtual void setImage(const QImage &newImg);

public slots:

    void resetScale();
    void fitScale();
    void updateSceneRect(const QRectF &);

    void zoomPlus();
    void zoomMinus();
    void zoom(double factor);

signals:

    void mouseMoved(const QPointF &pos);
    void scaleChanged(qreal);

protected:

    void setCenter(const QPointF& position);

    // Center
    const QPointF& getCenter() const
    {
        return m_centerPoint;
    }

    //Take over the interaction
    virtual void mouseMoveEvent(QMouseEvent* event);
    virtual void wheelEvent(QWheelEvent* event);
    virtual void resizeEvent(QResizeEvent* event);
    virtual void paintEvent(QPaintEvent* event);
    virtual bool event(QEvent *e);

private:

    void initBackground();

    //Holds the current centerpoint for the view, used for panning and zooming
    QPointF m_centerPoint;
    QPixmap m_tileBg;
};

#endif // _EDITORVIEW_H_
