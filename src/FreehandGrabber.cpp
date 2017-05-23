
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

#include <QPainter>
#include <QMouseEvent>
#include <QApplication>
#include <QDesktopWidget>
#include <QToolTip>
#include <QPaintEngine>

#include "FreehandGrabber.h"
#include "LogHandler.h"
#include "AppSettings.h"

const int handleSize = 10;

// based on ksnapshot code: 2010 Pau Garcia i Quiles <pgquiles@elpauer.org>

FreehandGrabber::FreehandGrabber( QWidget *parent) :
    QDialog(parent,Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint),
    m_selection(),
    m_mouseDown( false ),
    m_newSelection( false ),
    m_mouseOverHandle( 0 ),
    m_showHelp( true ),
    m_firstPass(true)
{
    //setWindowState(Qt::WindowFullScreen);
    setCursor(Qt::CrossCursor);

    m_desktopPixmap = QPixmap::grabWindow( QApplication::desktop()->winId(), 0, 0, QApplication::desktop()->width(), QApplication::desktop()->height() );
    resize( m_desktopPixmap.size() );
    setMinimumSize( m_desktopPixmap.size());
    setMaximumSize( m_desktopPixmap.size());
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    move( 0, 0 );
    loadSettings();
}

FreehandGrabber::~FreehandGrabber()
{
    saveSettings();
}

void FreehandGrabber::loadSettings()
{
    AppSettings settings;
    settings.beginGroup("FreehandGrabber");
    m_selection = settings.value("selection").value<QPolygon>();
    settings.endGroup();
}

void FreehandGrabber::saveSettings()
{
    AppSettings settings;
    settings.beginGroup("FreehandGrabber");
    if (!m_selection.isEmpty())
        settings.setValue("selection", m_selection);
    settings.endGroup();
}

static void drawPolygon( QPainter *painter, const QPolygon &p, const QColor &outline, const QColor &fill = QColor() )
{
    QRegion clip( p );
    clip = clip - p;
    QPen pen(outline, 1, Qt::SolidLine, Qt::SquareCap, Qt::BevelJoin);

    painter->save();
    painter->setClipRegion( clip );
    painter->setPen(pen);
    painter->drawPolygon( p );
    if ( fill.isValid() )
    {
        painter->setClipping( false );
        painter->setBrush( fill );
        painter->drawPolygon( p /*.translated( 1, -1 ) */);
    }
    painter->restore();
}

void FreehandGrabber::paintEvent( QPaintEvent* e )
{
    Q_UNUSED( e );

    // to workaround a problem on xrandr
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

    QPolygon pol = m_selection;
    if ( !m_selection.boundingRect().isNull() )
    {
        // Draw outline around selection.
        // Important: the 1px-wide outline is *also* part of the captured free-region because
        // I found no way to draw the outline *around* the selection because I found no way
        // to create a QPolygon which is smaller than the selection by 1px (QPolygon::translated
        // is NOT equivalent to QRect::adjusted)
        QPen pen(handleColor, 1, Qt::SolidLine, Qt::SquareCap, Qt::BevelJoin);
        painter.setPen(pen);
        painter.drawPolygon( pol );

        // Draw the grey area around the selection
        QRegion grey( rect() );
        grey = grey - pol;
        painter.setClipRegion( grey );
        painter.setPen( Qt::NoPen );
        painter.setBrush( overlayColor );
        painter.drawRect( rect() );
        painter.setClipRect( rect() );
        drawPolygon( &painter, pol, handleColor);
    }

    if ( m_showHelp )
    {
        painter.setPen( textColor );
        painter.setBrush( textBackgroundColor );
        QString helpText = tr( "Select a region using the mouse. To take the snapshot, press the Enter key or double click. Press Esc to quit." );
        QRect scrRect = QApplication::desktop()->screenGeometry( QApplication::desktop()->primaryScreen() ); // only on the primary screen

        m_helpTextRect = painter.boundingRect( scrRect.adjusted( 20, 20, -20, -20 ), Qt::AlignHCenter, helpText );
        m_helpTextRect.adjust( -4, -4, 8, 4 );
        drawPolygon( &painter, m_helpTextRect, textColor, textBackgroundColor );
        painter.drawText( m_helpTextRect.adjusted( 3, 3, -3, -3 ), helpText );
    }

    if ( m_selection.isEmpty() )
    {
        return;
    }

    // The grabbed region is everything which is covered by the drawn
    // rectangles (border included). This means that there is no 0px
    // selection, since a 0px wide rectangle will always be drawn as a line.
    QString txt = QString( "%1x%2" ).arg( m_selection.boundingRect().width() )
                      .arg( m_selection.boundingRect().height() );
    QRect textRect = painter.boundingRect( rect(), Qt::AlignLeft, txt );
    QRect boundingRect = textRect.adjusted( -4, 0, 0, 0);

    if ( textRect.width() < pol.boundingRect().width() - 2 * handleSize &&
         textRect.height() < pol.boundingRect().height() - 2 * handleSize &&
         ( pol.boundingRect().width() > 100 && pol.boundingRect().height() > 100 ) ) // center, unsuitable for small selections
    {
        boundingRect.moveCenter( pol.boundingRect().center() );
        textRect.moveCenter( pol.boundingRect().center() );
    }
    else if ( pol.boundingRect().y() - 3 > textRect.height() &&
              pol.boundingRect().x() + textRect.width() < rect().right() ) // on top, left aligned
    {
        boundingRect.moveBottomLeft( QPoint( pol.boundingRect().x(), pol.boundingRect().y() - 3 ) );
        textRect.moveBottomLeft( QPoint( pol.boundingRect().x() + 2, pol.boundingRect().y() - 3 ) );
    }
    else if ( pol.boundingRect().x() - 3 > textRect.width() ) // left, top aligned
    {
        boundingRect.moveTopRight( QPoint( pol.boundingRect().x() - 3, pol.boundingRect().y() ) );
        textRect.moveTopRight( QPoint( pol.boundingRect().x() - 5, pol.boundingRect().y() ) );
    }
    else if ( pol.boundingRect().bottom() + 3 + textRect.height() < rect().bottom() &&
              pol.boundingRect().right() > textRect.width() ) // at bottom, right aligned
    {
        boundingRect.moveTopRight( QPoint( pol.boundingRect().right(), pol.boundingRect().bottom() + 3 ) );
        textRect.moveTopRight( QPoint( pol.boundingRect().right() - 2, pol.boundingRect().bottom() + 3 ) );
    }
    else if ( pol.boundingRect().right() + textRect.width() + 3 < rect().width() ) // right, bottom aligned
    {
        boundingRect.moveBottomLeft( QPoint( pol.boundingRect().right() + 3, pol.boundingRect().bottom() ) );
        textRect.moveBottomLeft( QPoint( pol.boundingRect().right() + 5, pol.boundingRect().bottom() ) );
    }

    // if the above didn't catch it, you are running on a very tiny screen...
    drawPolygon( &painter, boundingRect, textColor, textBackgroundColor );

    painter.drawText( textRect, txt );

    if ( ( pol.boundingRect().height() > handleSize * 2 && pol.boundingRect().width() > handleSize * 2 )
         || !m_mouseDown )
    {
        painter.setBrush( QBrush(Qt::transparent) );
        painter.setClipRegion( QRegion(pol) );
        painter.drawPolygon( rect() );
    }
}

void FreehandGrabber::mousePressEvent( QMouseEvent* e )
{
    m_pBefore = e->pos();
    m_showHelp = !m_helpTextRect.contains( e->pos() );
    if ( e->button() == Qt::LeftButton )
    {
        m_mouseDown = true;
        m_dragStartPoint = e->pos();
        m_selectionBeforeDrag = m_selection;
        if ( !m_selection.containsPoint( e->pos(), Qt::WindingFill ) )
        {
            m_newSelection = true;
            m_selection = QPolygon();
        }
        else
        {
            setCursor( Qt::ClosedHandCursor );
        }
    }
    else if ( e->button() == Qt::RightButton )
    {
        m_newSelection = false;
        m_selection = QPolygon();
        setCursor( Qt::CrossCursor );
    }
    update();
}

void FreehandGrabber::mouseMoveEvent( QMouseEvent* e )
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
            m_selection << p;
        }
        else // moving the whole selection
        {
            QPoint p = e->pos() - m_pBefore; // Offset
            m_pBefore = e->pos(); // Save position for next iteration
            m_selection.translate(p);
        }

        update();
    }
    else
    {
        if ( m_selection.boundingRect().isEmpty() )
            return;

        if ( m_selection.containsPoint( e->pos(), Qt::WindingFill ) )
            setCursor( Qt::OpenHandCursor );
        else
            setCursor( Qt::CrossCursor );
    }
}

void FreehandGrabber::mouseReleaseEvent( QMouseEvent* e )
{
    m_mouseDown = false;
    m_newSelection = false;
    if ( m_mouseOverHandle == 0 && m_selection.containsPoint( e->pos(), Qt::WindingFill ) )
        setCursor( Qt::OpenHandCursor );
    update();
}

void FreehandGrabber::mouseDoubleClickEvent( QMouseEvent* )
{
    accept();
}

void FreehandGrabber::keyPressEvent( QKeyEvent* e )
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

QPixmap FreehandGrabber::getSelection() const
{
    QPolygon pol = m_selection;
    if ( !pol.isEmpty() )
    {
        /*
           int xOffset = m_desktopPixmap.rect().x() - pol.boundingRect().x();
           int yOffset = m_desktopPixmap.rect().y() - pol.boundingRect().y();
           QPolygon translatedPol = pol.translated(xOffset, yOffset);

           LogHandler::getInstance()->reportDebug( QString("FreehandGrabber::getSelection(): processing polygon with %1 nodes").arg( translatedPol.size() ));

           // first try a simple extraction
           //return m_desktopPixmap.copy(pol.boundingRect());

           // a second try: a simple clipping region
           QPixmap pix( pol.boundingRect().size() );
           pix.fill(Qt::transparent);

           QPainter painter(&pix);
           painter.setClipRegion( QRegion(translatedPol.boundingRect()) );
           painter.setCompositionMode(QPainter::CompositionMode_Source);
           painter.drawPixmap( pix.rect(), m_desktopPixmap, pol.boundingRect() );
           painter.end();

           return pix;
         */

        int xOffset = m_desktopPixmap.rect().x() - pol.boundingRect().x();
        int yOffset = m_desktopPixmap.rect().y() - pol.boundingRect().y();
        QPolygon translatedPol = pol.translated(xOffset, yOffset);

        QPixmap pix( pol.boundingRect().size() );
        pix.fill(Qt::transparent);

        QPainter painter(&pix);
        painter.setClipRegion( QRegion(translatedPol) );
        painter.setCompositionMode(QPainter::CompositionMode_Source);
        painter.drawPixmap( pix.rect(), m_desktopPixmap, pol.boundingRect() );
        painter.end();

        return pix;

        /*

           int xOffset = m_desktopPixmap.rect().x() - pol.boundingRect().x();
           int yOffset = m_desktopPixmap.rect().y() - pol.boundingRect().y();
           QPolygon translatedPol = pol.translated(xOffset, yOffset);

           QPixmap pix(pol.boundingRect().size());
           pix.fill(Qt::transparent);

           QPainter pt;
           pt.begin(&pix);
           if (pt.paintEngine()->hasFeature(QPaintEngine::PorterDuff))
           {
            pt.setRenderHints(QPainter::Antialiasing | QPainter::HighQualityAntialiasing | QPainter::SmoothPixmapTransform, true);
            pt.setBrush(Qt::black);
            pt.setPen(QPen(QBrush(Qt::black), 0.5));
            pt.drawPolygon(translatedPol);
            pt.setCompositionMode(QPainter::CompositionMode_SourceIn);
           }
           else
           {
            pt.setClipRegion(QRegion(translatedPol));
            pt.setCompositionMode(QPainter::CompositionMode_Source);
           }

           pt.drawPixmap(pix.rect(), m_desktopPixmap, pol.boundingRect());
           pt.end();

           return pix;
         */
    }
    return QPixmap();
}
