
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

#include <iostream>
#include <algorithm>

#include <QtCore/QDebug>
#include <QtCore/QPoint>
#include <QtCore/QVarLengthArray>

#include <QBitmap>
#include <QPixmap>
#include <QCursor>

#ifdef Q_OS_UNIX
#include <X11/Xlib.h>
#include <X11/extensions/Xfixes.h>

#ifdef HAVE_X11_EXTENSIONS_SHAPE_H
#include <X11/extensions/shape.h>
#endif // HAVE_X11_EXTENSIONS_SHAPE_H
#include <QX11Info>

static const int minDistance = 10;
static const int minSize = 8;

static bool operator< ( const QRect& r1, const QRect& r2 )
{
    return r1.width() * r1.height() < r2.width() * r2.height();
}

// Recursively iterates over the window w and its children, thereby building
// a tree of window descriptors. Windows in non-viewable state or with height
// or width smaller than minSize will be ignored.
static
void getWindowsRecursive( std::vector<QRect> *windows, Window w,
                          int rx = 0, int ry = 0, int depth = 0 )
{
    XWindowAttributes atts;
    XGetWindowAttributes( QX11Info::display(), w, &atts );

    if ( atts.map_state == IsViewable &&
         atts.width >= minSize && atts.height >= minSize )
    {
        int x = 0, y = 0;
        if ( depth )
        {
            x = atts.x + rx;
            y = atts.y + ry;
        }

        QRect r( x, y, atts.width, atts.height );
        if ( std::find( windows->begin(), windows->end(), r ) == windows->end() )
        {
            windows->push_back( r );
        }

        Window root, parent;
        Window* children;
        unsigned int nchildren;

        if( XQueryTree( QX11Info::display(), w, &root, &parent, &children, &nchildren ) != 0 )
        {
            for( unsigned int i = 0; i < nchildren; ++i )
                getWindowsRecursive( windows, children[ i ], x, y, depth + 1 );

            if( children != NULL )
            {
                XFree( children );
            }
        }
    }

    if ( depth == 0 )
    {
        std::sort( windows->begin(), windows->end() );
    }
}

static Window findRealWindow( Window w, int depth = 0 )
{
    if( depth > 5 )
    {
        return None;
    }

    static Atom wm_state = XInternAtom( QX11Info::display(), "WM_STATE", False );
    Atom type;
    int format;
    unsigned long nitems, after;
    unsigned char* prop;

    if( XGetWindowProperty( QX11Info::display(), w, wm_state, 0, 0, False, AnyPropertyType,
                            &type, &format, &nitems, &after, &prop ) == Success )
    {
        if( prop != NULL )
        {
            XFree( prop );
        }

        if( type != None )
        {
            return w;
        }
    }

    Window root, parent;
    Window* children;
    unsigned int nchildren;
    Window ret = None;

    if( XQueryTree( QX11Info::display(), w, &root, &parent, &children, &nchildren ) != 0 )
    {
        for( unsigned int i = 0;
             i < nchildren && ret == None;
             ++i )
            ret = findRealWindow( children[ i ], depth + 1 );

        if( children != NULL )
        {
            XFree( children );
        }
    }

    return ret;
}

static Window windowUnderCursor( bool includeDecorations = true )
{
    Window root;
    Window child;
    uint mask;
    int rootX, rootY, winX, winY;

    XGrabServer( QX11Info::display() );
    XQueryPointer( QX11Info::display(), QX11Info::appRootWindow(), &root, &child,
                   &rootX, &rootY, &winX, &winY, &mask );

    if( child == None )
    {
        child = QX11Info::appRootWindow();
    }

    if( !includeDecorations )
    {
        Window real_child = findRealWindow( child );

        if( real_child != None )   // test just in case
        {
            child = real_child;
        }
    }

    return child;
}

static QPixmap grabWindow( Window child, int x, int y, uint w, uint h, uint border,
                           QString *title = 0, QString *windowClass = 0 )
{
    QPixmap pm( QPixmap::grabWindow( QX11Info::appRootWindow(), x, y, w, h ) );

    // remove some warnings on compilation
    Q_UNUSED(child);
    Q_UNUSED(border);
    Q_UNUSED(title);
    Q_UNUSED(windowClass);

#ifdef HAVE_X11_EXTENSIONS_SHAPE_H
    int tmp1, tmp2;

    //Check whether the extension is available
    if ( XShapeQueryExtension( QX11Info::display(), &tmp1, &tmp2 ) )
    {
        QBitmap mask( w, h );

        //As the first step, get the mask from XShape.
        int count, order;
        XRectangle* rects = XShapeGetRectangles( QX11Info::display(), child,
                                                 ShapeBounding, &count, &order );

        //The ShapeBounding region is the outermost shape of the window;
        //ShapeBounding - ShapeClipping is defined to be the border.
        //Since the border area is part of the window, we use bounding
        // to limit our work region
        if (rects)
        {
            //Create a QRegion from the rectangles describing the bounding mask.
            QRegion contents;
            for ( int pos = 0; pos < count; pos++ )
                contents += QRegion( rects[pos].x, rects[pos].y,
                                     rects[pos].width, rects[pos].height );
            XFree( rects );

            //Create the bounding box.
            QRegion bbox( 0, 0, w, h );

            if( border > 0 )
            {
                contents.translate( border, border );
                contents += QRegion( 0, 0, border, h );
                contents += QRegion( 0, 0, w, border );
                contents += QRegion( 0, h - border, w, border );
                contents += QRegion( w - border, 0, border, h );
            }

            //Get the masked away area.
            QRegion maskedAway = bbox - contents;
            QVector<QRect> maskedAwayRects = maskedAway.rects();

            //Construct a bitmap mask from the rectangles
            QPainter p(&mask);
            p.fillRect(0, 0, w, h, Qt::color1);
            for (int pos = 0; pos < maskedAwayRects.count(); pos++)
                p.fillRect(maskedAwayRects[pos], Qt::color0);
            p.end();

            pm.setMask(mask);
        }
    }
#endif // HAVE_X11_EXTENSIONS_SHAPE_H

    return pm;
}

WindowGrabber::WindowGrabber()
{
}

WindowGrabber::~WindowGrabber()
{
}

QPixmap WindowGrabber::grabCurrent( bool includeDecorations, QRect &wRect  )
{
    int x, y;

    Window root;
    uint w, h, border, depth;

    XGrabServer( QX11Info::display() );
    Window child = windowUnderCursor( includeDecorations );
    XGetGeometry( QX11Info::display(), child, &root, &x, &y, &w, &h, &border, &depth );

    Window parent;
    Window* children;
    unsigned int nchildren;

    if( XQueryTree( QX11Info::display(), child, &root, &parent,
                    &children, &nchildren ) != 0 )
    {
        if( children != NULL )
        {
            XFree( children );
        }

        int newx, newy;
        Window dummy;

        if( XTranslateCoordinates( QX11Info::display(), parent, QX11Info::appRootWindow(),
                                   x, y, &newx, &newy, &dummy ) )
        {
            x = newx;
            y = newy;
        }
    }

    //m_windowPosition = QPoint(x,y);
    QString title, windowClass;
    QPixmap pm( grabWindow( child, x, y, w, h, border, &title, &windowClass ) );
    wRect = QRect(x, y, w, h);
    XUngrabServer( QX11Info::display() );
    return pm;
}

QPixmap WindowGrabber::grabCursor(QPoint &hotSpot)
{
    QPixmap pm;
    XFixesCursorImage *xfcursorImage = XFixesGetCursorImage( QX11Info::display() );
    if ( !xfcursorImage )
        return pm;
    hotSpot.setX(xfcursorImage->xhot);
    hotSpot.setY(xfcursorImage->yhot);

   //Annoyingly, xfixes specifies the data to be 32bit, but places it in an unsigned long *
   //which can be 64 bit.  So we need to iterate over a 64bit structure to put it in a 32bit
   //structure.
   QVarLengthArray< quint32 > pixels( xfcursorImage->width * xfcursorImage->height );
   for (int i = 0; i < xfcursorImage->width * xfcursorImage->height; ++i)
       pixels[i] = xfcursorImage->pixels[i] & 0xffffffff;

   QImage mouseCursor((uchar *) pixels.data(), xfcursorImage->width, xfcursorImage->height,
                      QImage::Format_ARGB32_Premultiplied);

    pm = QPixmap::fromImage(mouseCursor);
    XFree(xfcursorImage);
    return pm;
}

#endif // Q_OS_UNIX
