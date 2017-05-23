
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

#include "WindowGrabber.h"

#include <QtCore/QDebug>
#include <QtCore/QPoint>

#include <QBitmap>
#include <QApplication>
#include <QPixmap>
#include <QCursor>


#ifdef Q_OS_WIN

#include <windows.h>

#include <QDesktopWidget>
#include <QxtWindowSystem>

static UINT cxWindowBorder, cyWindowBorder;

static const int minSize = 8;

static bool operator< ( const QRect& r1, const QRect& r2 )
{
    return r1.width() * r1.height() < r2.width() * r2.height();
}

static HWND windowUnderCursor(bool includeDecorations = true)
{
    POINT pointCursor;
    QPoint qpointCursor = QCursor::pos();
    pointCursor.x = qpointCursor.x();
    pointCursor.y = qpointCursor.y();
    HWND wUnderCursor = WindowFromPoint(pointCursor);

    if( includeDecorations )
    {
        LONG_PTR style = GetWindowLongPtr( wUnderCursor, GWL_STYLE );
        if( ( style & WS_CHILD ) != 0 )
        {
            wUnderCursor = GetAncestor( wUnderCursor, GA_ROOT );
        }
    }
    return wUnderCursor;
}

static QPixmap grabWindow( HWND hWnd, bool decoration ,QRect & wRect, QString *title = 0, QString *windowClass = 0 )
{
    Q_UNUSED(title);
    Q_UNUSED(windowClass);

    RECT windowRect;
    RECT rcClient;

//     WINDOWINFO wi;
//     GetWindowInfo( hWnd, &wi);

    GetWindowRect(hWnd, &windowRect);

    qDebug () << "windowRect = " << windowRect.left << "," << windowRect.top << " " << windowRect.right << "," << windowRect.bottom;

    GetClientRect(hWnd, &rcClient);

    qDebug () << "rcClient orig = " << rcClient.left << "," << rcClient.top << " " << rcClient.right << "," << rcClient.bottom;

    ClientToScreen(hWnd, (LPPOINT)&rcClient.left);
    ClientToScreen(hWnd, (LPPOINT)&rcClient.right);

//     ClientToScreen(hWnd, (LPPOINT)&rcClient.bottom);
//     ClientToScreen(hWnd, (LPPOINT)&rcClient.top);

//     upper left x = client upper left x + xborder
//         upper left y = client upper left y + yborder + titleb
    qDebug () << "rcClient trnaslated = " << rcClient.left << "," << rcClient.top << " " << rcClient.right << "," << rcClient.bottom;

    wRect.setTop(rcClient.top);
    wRect.setBottom(rcClient.bottom);
    wRect.setLeft(rcClient.left);
    wRect.setRight(windowRect.right);

    int w = windowRect.right - windowRect.left;
    int h = windowRect.bottom - windowRect.top;

    //   qDebug() << "window rect w = " << w << " h = " << h;
    int wc = rcClient.right - rcClient.left;
    int hc = rcClient.bottom - rcClient.top;

    //  qDebug() << "rcClient rect w = " << wc << " h = " << hc;

    HDC targetDC = GetWindowDC(hWnd);
    HDC hDC = CreateCompatibleDC(targetDC);

    QPixmap pm;
    if (decoration)
    {
        HBITMAP tempPict = CreateCompatibleBitmap(targetDC, w, h);
        HGDIOBJ oldPict = SelectObject(hDC, tempPict);
        BitBlt(hDC, 0, 0, w, h, targetDC, 0, 0, SRCCOPY);
        tempPict = (HBITMAP) SelectObject(hDC, oldPict);
        pm = QPixmap::fromWinHBITMAP(tempPict);
        wRect.translate(-QPoint(rcClient.left - windowRect.left,rcClient.top - windowRect.top)); // a bit ugly no ?
    }
    else
    {
        HBITMAP tempPict = CreateCompatibleBitmap(targetDC, wc, hc);
        HGDIOBJ oldPict = SelectObject(hDC, tempPict);

        //BitBlt(hDC, 0, 0, w, h, targetDC, 0, 0, SRCCOPY);
        BitBlt(hDC, 0, 0, wc, hc, targetDC, rcClient.left - windowRect.left,rcClient.top - windowRect.top, SRCCOPY);
        tempPict = (HBITMAP) SelectObject(hDC, oldPict);
        pm = QPixmap::fromWinHBITMAP(tempPict);

    }

    DeleteDC(hDC);
    ReleaseDC(hWnd, targetDC);

    return pm;
}

WindowGrabber::WindowGrabber()
{
}

WindowGrabber::~WindowGrabber()
{
}

QPixmap WindowGrabber::grabCurrent(bool includeDecorations, QRect &wRect)
{
    QPixmap pixmap;

    WId id = QxtWindowSystem::activeWindow();
    if(includeDecorations)
    {
        wRect = QxtWindowSystem::windowGeometry(id);
        pixmap = QPixmap::grabWindow(QApplication::desktop()->winId(), wRect.x(), wRect.y(), wRect.width(),wRect.height());
    }
    else 
    {
        pixmap = QPixmap::grabWindow(id);
    }
    return pixmap;
}

QPixmap WindowGrabber::grabCurrentOld( bool includeDecorations, QRect &wRect )
{
    HWND hWindow;
    hWindow = windowUnderCursor(includeDecorations);
    Q_ASSERT(hWindow);

    HWND hParent;

// Now find the top-most window
    do
    {
        hParent = hWindow;
    }
    while( ( hWindow = GetParent(hWindow) ) != NULL );
    Q_ASSERT(hParent);

    QString title,windowClass;
    QPixmap pm( grabWindow( hParent, includeDecorations, wRect, &title, &windowClass ) );
    return pm;
}

HCURSOR GetCurrentCursorHandle()
{
    POINT pt;
    HWND hWnd;
    DWORD dwThreadID, dwCurrentThreadID;
    HCURSOR hCursor = NULL;

    // Find out which window owns the cursor
    GetCursorPos(&pt);
    hWnd = WindowFromPoint(pt);

    // Get the thread ID for the cursor owner.
    dwThreadID = GetWindowThreadProcessId(hWnd, NULL);

    // Get the thread ID for the current thread
    dwCurrentThreadID = GetCurrentThreadId();

    // If the cursor owner is not us then we must attach to
    // the other thread in so that we can use GetCursor() to
    // return the correct hCursor
    if (dwCurrentThreadID != dwThreadID)
    {

        // Attach to the thread that owns the cursor
        if (AttachThreadInput(dwCurrentThreadID, dwThreadID, TRUE))
        {

            // Get the handle to the cursor
            hCursor = GetCursor();

            // Detach from the thread that owns the cursor
            AttachThreadInput(dwCurrentThreadID, dwThreadID, FALSE);
        }
    }
    else
        hCursor = GetCursor();

    return hCursor;
}

QPixmap WindowGrabber::grabCursor(QPoint &hotSpot)
{
    QPixmap pm;

    HDC hdcScreen = GetDC(NULL);
    HDC hdcMem = CreateCompatibleDC(hdcScreen);
    CURSORINFO cursorInfo = { 0 };
    cursorInfo.cbSize = sizeof(cursorInfo);
    if (::GetCursorInfo(&cursorInfo))
    {
        ICONINFO ii = {0};
        GetIconInfo(cursorInfo.hCursor, &ii);
        HBITMAP hbmCanvas = CreateCompatibleBitmap(hdcScreen, 32, 32);
        // Select the bitmap into the device context.
        HGDIOBJ hbmOld = SelectObject(hdcMem, hbmCanvas);
        hotSpot.setX(ii.xHotspot);
        hotSpot.setY(ii.yHotspot);
        DrawIcon(hdcMem, 0,0 , cursorInfo.hCursor);
        // Get the color of the pixel you're interested in.
        COLORREF clr = GetPixel(hdcMem, 0, 0);
        pm = QPixmap::fromWinHBITMAP(hbmCanvas,QPixmap::Alpha); 
        // Clean up after yourself.
        SelectObject(hdcMem, hbmOld);
        DeleteObject(hbmCanvas);
        DeleteDC(hdcMem);
        ReleaseDC(NULL,hdcScreen);
    }

    return pm;
}

#endif // Q_OS_WIN
