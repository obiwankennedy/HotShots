
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

#ifndef _FREEHANDGRABBER_H_
#define _FREEHANDGRABBER_H_

#include <QtCore/QPoint>
#include <QtCore/QRect>

#include <QDialog>
#include <QPolygon>

class QPaintEvent;
class QMouseEvent;

class FreehandGrabber : public QDialog
{
    Q_OBJECT

public:

    FreehandGrabber(QWidget *parent = 0);
    ~FreehandGrabber();

    QPixmap getSelection() const;

protected:

    void paintEvent( QPaintEvent* e );
    void mousePressEvent( QMouseEvent* e );
    void mouseMoveEvent( QMouseEvent* e );
    void mouseReleaseEvent( QMouseEvent* e );
    void mouseDoubleClickEvent( QMouseEvent* );
    void keyPressEvent( QKeyEvent* e );

    void loadSettings();
    void saveSettings();

    QPolygon m_selection;
    bool m_mouseDown;
    bool m_newSelection;

    QRect* m_mouseOverHandle;
    QPoint m_dragStartPoint;
    QPolygon m_selectionBeforeDrag;
    bool m_showHelp;

    QRect m_helpTextRect;

    QPixmap m_desktopPixmap;
    QPoint m_pBefore;
    bool m_firstPass;
};

#endif // _FREEHANDGRABBER_H_
