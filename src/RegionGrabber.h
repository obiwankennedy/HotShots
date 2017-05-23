
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


#ifndef _REGIONGRABBER_H_
#define _REGIONGRABBER_H_

#include <QDialog>
#include <QRegion>
#include <QtCore/QPoint>
#include <QtCore/QVector>
#include <QtCore/QRect>

class QPaintEvent;
class QResizeEvent;
class QMouseEvent;

class RectHandle : public QRect
{
public:

    enum Location
    {
        LOCATION_TOPLEFT = 0,
        LOCATION_TOPCENTER,
        LOVATION_TOPRIGHT,
        LOCATION_CENTERLEFT,
        LOCATION_CENTER,
        LOVATION_CENTERRIGHT,
        LOCATION_BOTTOMLEFT,
        LOCATION_BOTTOMCENTER,
        LOVATION_BOTTOMRIGHT,
    };

    RectHandle(const QRect &r, int location);
    RectHandle( int x, int y, int width, int height, int location);

    QPoint getReferencePoint() const;

private:

    int m_location;
};

class RegionGrabber : public QDialog
{
    Q_OBJECT

    enum MaskType
    {
        StrokeMask, FillMask
    };

public:

    RegionGrabber(QWidget *parent = 0);
    ~RegionGrabber();

    QPixmap getSelection() const;

protected slots:

    void init();

protected:

    void paintEvent( QPaintEvent* e );
    void resizeEvent( QResizeEvent* e );
    void mousePressEvent( QMouseEvent* e );
    void mouseMoveEvent( QMouseEvent* e );
    void mouseReleaseEvent( QMouseEvent* e );
    void mouseDoubleClickEvent( QMouseEvent* );
    void keyPressEvent( QKeyEvent* e );
    void updateHandles();
    QRegion handleMask( MaskType type ) const;
    QPoint limitPointToRect( const QPoint &p, const QRect &r ) const;
    QRect normalizeSelection( const QRect &s ) const;

    void drawMagnifier(QPainter &painter);
    QPoint computeValidOffset(const QRect &scrGeom, const QRect &selectGeom, const QSize &magSize) const;
    void loadSettings();
    void saveSettings();

    QRect m_selection;
    bool m_mouseDown;
    bool m_newSelection;
    RectHandle* m_mouseOverHandle;
    QPoint m_dragStartPoint;
    QPoint m_currentPoint;
    QRect m_selectionBeforeDrag;
    bool m_showHelp;
    bool m_dragSelection;

    // naming convention for handles
    // T top, B bottom, R Right, L left
    // 2 letters: a corner
    // 1 letter: the handle on the middle of the corresponding side
    RectHandle TLHandle, TRHandle, BLHandle, BRHandle;
    RectHandle LHandle, THandle, RHandle, BHandle;
    QRect m_helpTextRect;

    QVector<RectHandle*> m_handles;
    QPixmap m_desktopPixmap;
    bool m_firstPass;
};

#endif // _REGIONGRABBER_H_
