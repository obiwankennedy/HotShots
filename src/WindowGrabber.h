
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

#ifndef _WINDOWGRABBER_H_
#define _WINDOWGRABBER_H_

#include <QtCore/QPoint>
#include <QtCore/QRect>

#include <QPixmap>

class WindowGrabber
{
public:

    WindowGrabber();
    ~WindowGrabber();

    //! Grab a screenshot of the current window.  x and y are set to the position of the window
    static QPixmap grabCurrent( bool includeDecorations, QRect &wRect );

    //! Grab the cursor image
    static QPixmap grabCursor(QPoint &hotSpot);

#ifdef Q_OS_WIN
    static QPixmap grabCurrentOld( bool includeDecorations, QRect &wRect );
#endif

};

#endif // _WINDOWGRABBER_H_
