
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
#include <QtCore/QTimer>

#include <QPainter>
#include <QMouseEvent>
#include <QApplication>
#include <QDesktopWidget>
#include <QToolTip>

#include "RegionGrabber.h"
#include "AppSettings.h"

const int handleSize = 10;

RectHandle::RectHandle(const QRect &r, int location) :
    QRect(r),
    m_location(location)
{
}

RectHandle::RectHandle(int x, int y, int width, int height, int location) :
    QRect(x, y, width, height),
    m_location(location)
{
}

QPoint RectHandle::getReferencePoint() const
{
    QPoint ref;
    switch (m_location)
    {
        case LOCATION_TOPLEFT:
            ref = topLeft();
            break;
        case LOCATION_TOPCENTER:
            ref = QPoint( center().x(),top() );
            break;
        case LOVATION_TOPRIGHT:
            ref = topRight();
            break;
        case LOCATION_CENTERLEFT:
            ref = QPoint( left(),center().y() );
            break;
        case LOCATION_CENTER:
            ref = center();
            break;
        case LOVATION_CENTERRIGHT:
            ref = QPoint( right(),center().y() );
            break;
        case LOCATION_BOTTOMLEFT:
            ref = bottomLeft();
            break;
        case LOCATION_BOTTOMCENTER:
            ref = QPoint( center().x(),bottom() );
            break;
        case LOVATION_BOTTOMRIGHT:
            ref = bottomRight();
            break;
        default:
            break;
    }
    return ref;
}

// based on ksnapshot code: 2007 Luca Gugelmann <lucag@student.ethz.ch>

RegionGrabber::RegionGrabber(QWidget *parent ) :
    QDialog(parent,Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint ),
    m_selection(),
    m_mouseDown( false ),
    m_newSelection( false ),
    m_mouseOverHandle( 0 ),
    m_showHelp( true ),
    m_dragSelection(false),
    TLHandle(0,0,handleSize,handleSize,RectHandle::LOCATION_TOPLEFT), TRHandle(0,0,handleSize,handleSize,RectHandle::LOVATION_TOPRIGHT),
    BLHandle(0,0,handleSize,handleSize,RectHandle::LOCATION_BOTTOMLEFT), BRHandle(0,0,handleSize,handleSize,RectHandle::LOVATION_BOTTOMRIGHT),
    LHandle(0,0,handleSize,handleSize,RectHandle::LOCATION_CENTERLEFT), THandle(0,0,handleSize,handleSize,RectHandle::LOCATION_TOPCENTER),
    RHandle(0,0,handleSize,handleSize,RectHandle::LOVATION_CENTERRIGHT), BHandle(0,0,handleSize,handleSize,RectHandle::LOCATION_BOTTOMCENTER),
    m_firstPass(true)
{
    //setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint );
    //setWindowState(Qt::WindowFullScreen);
    setCursor(Qt::CrossCursor);
    setMouseTracking( true ); // for handles move

    m_handles << &TLHandle << &TRHandle << &BLHandle << &BRHandle
              << &LHandle << &THandle << &RHandle << &BHandle;

    // for debugging !!
    //int screenId = QApplication::desktop()->screenNumber( QCursor::pos() );
    //QRect geom = QApplication::desktop()->screenGeometry( screenId );
    //m_desktopPixmap = QPixmap::grabWindow( QApplication::desktop()->winId(), geom.x(), geom.y(), geom.width(), geom.height() );
    m_desktopPixmap = QPixmap::grabWindow( QApplication::desktop()->winId(), 0, 0, QApplication::desktop()->width(), QApplication::desktop()->height() );
    resize( m_desktopPixmap.size() );
    setMinimumSize( m_desktopPixmap.size());
    setMaximumSize( m_desktopPixmap.size());
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    move( 0, 0 );
    loadSettings();
    //QTimer::singleShot(10,this,SLOT(initPosition()));
}

RegionGrabber::~RegionGrabber()
{
    saveSettings();
}

void RegionGrabber::init()
{
    m_desktopPixmap = QPixmap::grabWindow( QApplication::desktop()->winId() );
    resize( m_desktopPixmap.size() );
    move( 0, 0 );
    setCursor( Qt::CrossCursor );
    show();
    grabMouse();
    grabKeyboard();
}


void RegionGrabber::loadSettings()
{
    AppSettings settings;
    settings.beginGroup("RegionGrabber");
    m_selection = settings.value("selection").toRect();
    settings.endGroup();
}

void RegionGrabber::saveSettings()
{
    AppSettings settings;
    settings.beginGroup("RegionGrabber");
    if (m_selection.isValid())
        settings.setValue("selection", m_selection);
    settings.endGroup();
}

static void drawRect( QPainter *painter, const QRect &r, const QColor &outline, const QColor &fill = QColor() )
{
    QRegion clip( r );
    clip = clip.subtracted( r.adjusted( 1, 1, -1, -1 ) );

    painter->save();
    painter->setClipRegion( clip );
    painter->setPen( Qt::NoPen );
    painter->setBrush( outline );
    painter->drawRect( r );
    if ( fill.isValid() )
    {
        painter->setClipping( false );
        painter->setBrush( fill );
        painter->drawRect( r.adjusted( 1, 1, -1, -1 ) );
    }
    painter->restore();
}

void RegionGrabber::paintEvent( QPaintEvent* e )
{
    Q_UNUSED( e );

    // to m_firstPass a problem on xrandr
    if (m_firstPass)
    {
        move(0,0);
        m_firstPass = false;
    }

    QPainter painter( this );

    QPalette pal( QToolTip::palette() );
    QFont font = QToolTip::font();

    QColor handleColor = pal.color( QPalette::Active, QPalette::Highlight );
    handleColor.setAlpha( 160 );
    QColor overlayColor( 0, 0, 0, 160 );
    QColor textColor = pal.color( QPalette::Active, QPalette::Text );
    QColor textBackgroundColor = pal.color( QPalette::Active, QPalette::Base );
    painter.drawPixmap(0, 0, m_desktopPixmap);
    painter.setFont(font);

    QRect r = m_selection;
    if ( !m_selection.isNull() )
    {
        QRegion grey( rect() );
        grey = grey.subtracted( r );
        painter.setClipRegion( grey );
        painter.setPen( Qt::NoPen );
        painter.setBrush( overlayColor );
        painter.drawRect( rect() );
        painter.setClipRect( rect() );
        drawRect( &painter, r, handleColor );
    }

    if ( m_showHelp )
    {
        painter.setPen( textColor );
        painter.setBrush( textBackgroundColor );
        QString helpText = tr( "Select a region using the mouse. To take the snapshot, press the Enter key or double click. Press Esc to quit." );
        QRect scrRect = QApplication::desktop()->screenGeometry( QApplication::desktop()->primaryScreen() ); // only on the primary screen

        m_helpTextRect = painter.boundingRect( scrRect.adjusted( 20, 20, -20, -20 ), Qt::AlignHCenter, helpText );
        m_helpTextRect.adjust( -4, -4, 8, 4 );
        drawRect( &painter, m_helpTextRect, textColor, textBackgroundColor );
        painter.drawText( m_helpTextRect.adjusted( 3, 3, -3, -3 ), helpText );
    }

    if ( m_selection.isNull() )
    {
        return;
    }

    // The grabbed region is everything which is covered by the drawn
    // rectangles (border included). This means that there is no 0px
    // selection, since a 0px wide rectangle will always be drawn as a line.
    QString txt = QString( "%1x%2" ).arg( m_selection.width() )
                      .arg( m_selection.height() );
    QRect textRect = painter.boundingRect( rect(), Qt::AlignLeft, txt );
    QRect boundingRect = textRect.adjusted( -4, 0, 0, 0);

    if ( textRect.width() < r.width() - 2 * handleSize &&
         textRect.height() < r.height() - 2 * handleSize &&
         ( r.width() > 100 && r.height() > 100 ) ) // center, unsuitable for small selections
    {
        boundingRect.moveCenter( r.center() );
        textRect.moveCenter( r.center() );
    }
    else if ( r.y() - 3 > textRect.height() &&
              r.x() + textRect.width() < rect().right() ) // on top, left aligned
    {
        boundingRect.moveBottomLeft( QPoint( r.x(), r.y() - 3 ) );
        textRect.moveBottomLeft( QPoint( r.x() + 2, r.y() - 3 ) );
    }
    else if ( r.x() - 3 > textRect.width() ) // left, top aligned
    {
        boundingRect.moveTopRight( QPoint( r.x() - 3, r.y() ) );
        textRect.moveTopRight( QPoint( r.x() - 5, r.y() ) );
    }
    else if ( r.bottom() + 3 + textRect.height() < rect().bottom() &&
              r.right() > textRect.width() ) // at bottom, right aligned
    {
        boundingRect.moveTopRight( QPoint( r.right(), r.bottom() + 3 ) );
        textRect.moveTopRight( QPoint( r.right() - 2, r.bottom() + 3 ) );
    }
    else if ( r.right() + textRect.width() + 3 < rect().width() ) // right, bottom aligned
    {
        boundingRect.moveBottomLeft( QPoint( r.right() + 3, r.bottom() ) );
        textRect.moveBottomLeft( QPoint( r.right() + 5, r.bottom() ) );
    }

    // if the above didn't catch it, you are running on a very tiny screen...
    drawRect( &painter, boundingRect, textColor, textBackgroundColor );

    painter.drawText( textRect, txt );

    if ( ( r.height() > handleSize * 2 && r.width() > handleSize * 2 )
         || !m_mouseDown )
    {
        updateHandles();
        painter.save();
        painter.setPen( Qt::NoPen );
        painter.setBrush( handleColor );
        painter.setClipRegion( handleMask( StrokeMask ) );
        painter.drawRect( rect() );
        handleColor.setAlpha( 60 );
        painter.setBrush( handleColor );
        painter.setClipRegion( handleMask( FillMask ) );
        painter.drawRect( rect() );
        painter.restore();
    }

    if (m_mouseDown && !m_dragSelection)
        drawMagnifier(painter);
}

void RegionGrabber::resizeEvent( QResizeEvent* e )
{
    Q_UNUSED( e );
    if ( m_selection.isNull() )
        return;

    QRect r = m_selection;
    r.setTopLeft( limitPointToRect( r.topLeft(), rect() ) );
    r.setBottomRight( limitPointToRect( r.bottomRight(), rect() ) );
    if ( r.width() <= 1 || r.height() <= 1 ) //this just results in ugly drawing...
        m_selection = QRect();
    else
        m_selection = normalizeSelection(r);
}

void RegionGrabber::mousePressEvent( QMouseEvent* e )
{
    m_showHelp = !m_helpTextRect.contains( e->pos() );
    m_dragSelection = false;
    if ( e->button() == Qt::LeftButton )
    {
        m_mouseDown = true;
        m_dragStartPoint = e->pos();
        m_selectionBeforeDrag = m_selection;
        if ( !m_selection.contains( e->pos() ) )
        {
            m_newSelection = true;
            m_selection = QRect();
        }
        else
        {
            m_dragSelection = true;
            setCursor( Qt::ClosedHandCursor );
        }
    }
    else if ( e->button() == Qt::RightButton )
    {
        m_newSelection = false;
        m_selection = QRect();
        setCursor( Qt::CrossCursor );
    }
    update();
}

void RegionGrabber::mouseMoveEvent( QMouseEvent* e )
{
    bool shouldShowHelp = !m_helpTextRect.contains( e->pos() );
    if (shouldShowHelp != m_showHelp)
    {
        m_showHelp = shouldShowHelp;
        update();
    }

    if ( m_mouseDown )
    {
        if ( m_newSelection )
        {
            QPoint p = e->pos();
            QRect r = rect();
            m_currentPoint = limitPointToRect( p, r );
            m_selection = normalizeSelection( QRect( m_dragStartPoint, m_currentPoint ) );
        }
        else if ( m_mouseOverHandle == 0 ) // moving the whole selection
        {
            QRect r = rect().normalized(), s = m_selectionBeforeDrag.normalized();
            QPoint p = s.topLeft() + e->pos() - m_dragStartPoint;
            r.setBottomRight( r.bottomRight() - QPoint( s.width(), s.height() ) + QPoint( 1, 1 ) );
            if ( !r.isNull() && r.isValid() )
                m_selection.moveTo( limitPointToRect( p, r ) );
        }
        else // dragging a handle
        {
            QRect r = m_selectionBeforeDrag;
            QPoint offset = e->pos() - m_dragStartPoint;

            if ( m_mouseOverHandle == &TLHandle || m_mouseOverHandle == &THandle
                 || m_mouseOverHandle == &TRHandle ) // dragging one of the top handles
            {
                r.setTop( r.top() + offset.y() );
            }

            if ( m_mouseOverHandle == &TLHandle || m_mouseOverHandle == &LHandle
                 || m_mouseOverHandle == &BLHandle ) // dragging one of the left handles
            {
                r.setLeft( r.left() + offset.x() );
            }

            if ( m_mouseOverHandle == &BLHandle || m_mouseOverHandle == &BHandle
                 || m_mouseOverHandle == &BRHandle ) // dragging one of the bottom handles
            {
                r.setBottom( r.bottom() + offset.y() );
            }

            if ( m_mouseOverHandle == &TRHandle || m_mouseOverHandle == &RHandle
                 || m_mouseOverHandle == &BRHandle ) // dragging one of the right handles
            {
                r.setRight( r.right() + offset.x() );
            }
            r.setTopLeft( limitPointToRect( r.topLeft(), rect() ) );
            r.setBottomRight( limitPointToRect( r.bottomRight(), rect() ) );
            m_selection = normalizeSelection(r);
            m_dragSelection = false;
        }
        update();
    }
    else
    {
        if ( m_selection.isNull() )
            return;

        bool found = false;
        foreach( RectHandle * r, m_handles )
        {
            if ( r->contains( e->pos() ) )
            {
                m_mouseOverHandle = r;
                found = true;
                break;
            }
        }
        if ( !found )
        {
            m_mouseOverHandle = 0;
            if ( m_selection.contains( e->pos() ) )
                setCursor( Qt::OpenHandCursor );
            else
                setCursor( Qt::CrossCursor );
        }
        else
        {
            if ( m_mouseOverHandle == &TLHandle || m_mouseOverHandle == &BRHandle )
                setCursor( Qt::SizeFDiagCursor );
            if ( m_mouseOverHandle == &TRHandle || m_mouseOverHandle == &BLHandle )
                setCursor( Qt::SizeBDiagCursor );
            if ( m_mouseOverHandle == &LHandle || m_mouseOverHandle == &RHandle )
                setCursor( Qt::SizeHorCursor );
            if ( m_mouseOverHandle == &THandle || m_mouseOverHandle == &BHandle )
                setCursor( Qt::SizeVerCursor );
        }
    }
}

void RegionGrabber::mouseReleaseEvent( QMouseEvent* e )
{
    m_mouseDown = false;
    m_dragSelection = false;
    m_newSelection = false;
    if ( m_mouseOverHandle == 0 && m_selection.contains( e->pos() ) )
        setCursor( Qt::OpenHandCursor );
    update();
}

void RegionGrabber::mouseDoubleClickEvent( QMouseEvent* )
{
    accept();
}

void RegionGrabber::keyPressEvent( QKeyEvent* e )
{
    if ( e->key() == Qt::Key_Escape )
    {
        reject();
    }
    else if ( e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return )
    {
        accept();
    }
    else
    {
        e->ignore();
    }
}

QPixmap RegionGrabber::getSelection() const
{
    QRect r = m_selection;
    if ( !r.isNull() && r.isValid() )
        return m_desktopPixmap.copy(r);

    return QPixmap();
}

void RegionGrabber::updateHandles()
{
    QRect r = m_selection;
    int s2 = handleSize / 2;

    TLHandle.moveTopLeft( r.topLeft() );
    TRHandle.moveTopRight( r.topRight() );
    BLHandle.moveBottomLeft( r.bottomLeft() );
    BRHandle.moveBottomRight( r.bottomRight() );

    LHandle.moveTopLeft( QPoint( r.x(), r.y() + r.height() / 2 - s2) );
    THandle.moveTopLeft( QPoint( r.x() + r.width() / 2 - s2, r.y() ) );
    RHandle.moveTopRight( QPoint( r.right(), r.y() + r.height() / 2 - s2 ) );
    BHandle.moveBottomLeft( QPoint( r.x() + r.width() / 2 - s2, r.bottom() ) );
}

QRegion RegionGrabber::handleMask( MaskType type ) const
{
    // note: not normalized QRects are bad here, since they will not be drawn
    QRegion mask;
    foreach( QRect * rect, m_handles )
    {
        if ( type == StrokeMask )
        {
            QRegion r( *rect );
            mask += r.subtracted( rect->adjusted( 1, 1, -1, -1 ) );
        }
        else
        {
            mask += QRegion( rect->adjusted( 1, 1, -1, -1 ) );
        }
    }
    return mask;
}

QPoint RegionGrabber::limitPointToRect( const QPoint &p, const QRect &r ) const
{
    QPoint q;
    q.setX( p.x() < r.x() ? r.x() : p.x() < r.right() ? p.x() : r.right() );
    q.setY( p.y() < r.y() ? r.y() : p.y() < r.bottom() ? p.y() : r.bottom() );
    return q;
}

QRect RegionGrabber::normalizeSelection( const QRect &s ) const
{
    QRect r = s;
    if (r.width() <= 0)
    {
        int l = r.left();
        int w = r.width();
        r.setLeft(l + w - 1);
        r.setRight(l);
    }
    if (r.height() <= 0)
    {
        int t = r.top();
        int h = r.height();
        r.setTop(t + h - 1);
        r.setBottom(t);
    }
    return r;
}

void RegionGrabber::drawMagnifier(QPainter &painter)
{
    const quint8 zoomSide = 120;

    QPoint ref = m_currentPoint;

    if (m_mouseOverHandle)
        ref = m_mouseOverHandle->getReferencePoint();

    // create magnifier coords
    QPoint zoomStart = ref;
    zoomStart -= QPoint(zoomSide / 5, zoomSide / 5); // 40, 40

    QPoint zoomEnd = ref;
    zoomEnd += QPoint(zoomSide / 5, zoomSide / 5);

    // creating rect area for magnifier
    QRect zoomRect = QRect(zoomStart, zoomEnd);

    QPixmap zoomPixmap = m_desktopPixmap.copy(zoomRect).scaled(QSize(zoomSide, zoomSide), Qt::KeepAspectRatio);

    QPainter zoomPainer(&zoomPixmap);     // create painter from pixmap magnifier
    zoomPainer.setPen( QPen(QBrush( QColor(255, 0, 0, 180) ), 2) );
    zoomPainer.drawRect( zoomPixmap.rect() );   // draw
    zoomPainer.drawText(zoomPixmap.rect().center() - QPoint(4, -4), "+");

    // position for drawing preview
    QPoint zoomCenter = computeValidOffset( m_desktopPixmap.rect(), m_selection, zoomPixmap.size() );

    painter.drawPixmap(zoomCenter, zoomPixmap);
}

QPoint RegionGrabber::computeValidOffset(const QRect &scrGeom, const QRect &selectGeom, const QSize &magSize) const
{
    QPoint pos;

    QList<QRect> m_possibleLocation;

    // put windows at up-left corner
    m_possibleLocation << QRect( selectGeom.topLeft() + QPoint( -magSize.width(), -magSize.height() ), magSize );
    m_possibleLocation << QRect( selectGeom.topLeft() + QPoint( -magSize.width(), 0 ), magSize );
    m_possibleLocation << QRect( selectGeom.topLeft() + QPoint( 0, -magSize.height() ), magSize );

    // put windows at up-right corner
    m_possibleLocation << QRect(selectGeom.topRight() + QPoint( 0, -magSize.height() ), magSize );
    m_possibleLocation << QRect(selectGeom.topRight() + QPoint( 0, 0 ), magSize );
    m_possibleLocation << QRect(selectGeom.topRight() + QPoint( -magSize.width(), -magSize.height() ), magSize );

    // put windows at bottom-left corner
    m_possibleLocation << QRect ( selectGeom.bottomLeft() + QPoint( -magSize.width(),0 ), magSize );
    m_possibleLocation << QRect ( selectGeom.bottomLeft() + QPoint( -magSize.width(),-magSize.height() ), magSize );
    m_possibleLocation << QRect ( selectGeom.bottomLeft() + QPoint( 0,0 ), magSize );

    // put windows at bottom-right corner
    m_possibleLocation << QRect( selectGeom.bottomRight() + QPoint( 0, 0 ), magSize );
    m_possibleLocation << QRect( selectGeom.bottomRight() + QPoint( 0, -magSize.height() ), magSize );
    m_possibleLocation << QRect( selectGeom.bottomRight() + QPoint(-magSize.width(), 0 ), magSize );

    foreach (const QRect &rect, m_possibleLocation)
    {
        if ( scrGeom.contains(rect,true) )
        {
            pos = rect.topLeft();
            break;
        }
    }

    return pos;
}
