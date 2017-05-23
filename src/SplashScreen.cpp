
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

#include "SplashScreen.h"

#include <QtCore/QTimer>
#include <QtCore/QtDebug>
#include <QtCore/QThread>
#include <QApplication>
#include <QDesktopWidget>

const int refreshInterval = 201;

SplashScreen::SplashScreen(const QPixmap& pixmap, unsigned int minimalDisplayDuration, int screenId, bool transparentBackground,  Qt::WindowFlags /* f */)
    : QSplashScreen(),
    m_nextWidget(NULL),
    m_timerFired(false),
    m_currAlign(Qt::AlignLeft),
    m_currColor(Qt::black),
    m_screenId(screenId)
{
    setWindowModality(Qt::ApplicationModal);
    setSplash(pixmap,transparentBackground);
    m_refreshId = startTimer(0);
    startTimer(minimalDisplayDuration);
}

void SplashScreen::timerEvent(QTimerEvent *event)
{
    if ( event->timerId()  == m_refreshId)
    {
        repaint();
        return;
    }

    killTimer( event->timerId() );
    killTimer( m_refreshId );

    m_timerFired = true;
    if (m_nextWidget)
    {
        m_nextWidget->show();
    }
    QSplashScreen::finish(m_nextWidget);
}

void SplashScreen::setSplash(const QPixmap& pixmap,bool transparentBackground)
{
    QPixmap extract( pixmap.width(),pixmap.height() );
    if (transparentBackground) // extract background from desktop
    {
        QRect geom = QApplication::desktop()->screenGeometry( m_screenId );
        QPixmap desktopPixmap = QPixmap::grabWindow( QApplication::desktop()->winId(), geom.x(), geom.y(), geom.width(), geom.height() );

        int x0 = desktopPixmap.width() / 2.0 - pixmap.width() / 2.0;
        int y0 = desktopPixmap.height() / 2.0 - pixmap.height() / 2.0;
        extract = desktopPixmap.copy( x0,y0,pixmap.width(),pixmap.height() );
    }
    else // create a dummy background
    {
        QPainter painter(&extract);
        QLinearGradient linearGrad( 0, 0, 0, extract.height() );
        linearGrad.setColorAt( 0.0, QColor(100,187,187) );
        linearGrad.setColorAt( 1.0, QColor(43,117,144) );
        painter.setBrush( linearGrad );
        painter.fillRect( 0, 0, extract.width(),extract.height(), QBrush(linearGrad) );
        painter.setBrush( QBrush() );
        painter.drawRect( 0, 0, extract.width() - 1,extract.height() - 1);
    }

    QImage resultImage = QImage(pixmap.size(), QImage::Format_ARGB32_Premultiplied);
    QPainter painter(&resultImage);
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
    painter.drawImage( 0, 0, extract.toImage() );
    painter.drawImage( 0, 0, pixmap.toImage() );
    painter.end();
    setPixmap( QPixmap::fromImage(resultImage) );

    // must move the splashscreen by taking ito account screen Number
    move( QApplication::desktop()->screenGeometry(m_screenId).center() - resultImage.rect().center() );
    repaint();
}

void SplashScreen::delayedFinish(QWidget *next)
{
    m_nextWidget = next;
    if (m_timerFired && m_nextWidget)
    {
        m_nextWidget->show();
        QSplashScreen::finish(m_nextWidget);
    }
}

void SplashScreen::closeEvent(QCloseEvent *event)
{
    QWidget::closeEvent(event);
}

void SplashScreen::drawContents(QPainter *painter)
{
    painter->setPen(m_currColor);

    QPixmap pm = QSplashScreen::pixmap();
    QRect r( 0, 0, pm.width(), pm.height() );

    r.setRect(r.x() + 100, r.y() - 15, r.width() - 10, r.height() - 10);
    painter->setFont( QFont("arial", 8) );
    painter->drawText(r, m_currAlign, m_currStatus);
}

void SplashScreen::showMessage(const QString &message, int alignment,
                               const QColor &color)
{
    m_currStatus = message;
    m_currAlign = alignment;
    m_currColor = color;
    emit messageChanged(m_currStatus);
    repaint();
}
