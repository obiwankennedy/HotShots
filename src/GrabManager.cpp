
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

#include <QtCore/QTimer>
#include <QtCore/QDebug>

#include <QApplication>
#include <QDesktopWidget>
#include <QCursor>
#include <QPainter>
#include <QMessageBox>


#include "GrabManager.h"
#include "RegionGrabber.h"
#include "FreehandGrabber.h"
#include "WindowGrabber.h"
#include "PostEffect.h"
#include "AppSettings.h"

const int defaultMinDelay = 300; // min delay

GrabManager::GrabManager(QObject *parent) : QObject(parent),
    m_delay(defaultMinDelay),
    m_autoGrab(false),
    m_mode(GRAB_ALL_SCREENS),
    m_defaultMode(GRAB_ALL_SCREENS),
    m_stop(false),
    m_captureDecoration(true),
    m_addMouseCursor(false)
{
}

GrabManager::~GrabManager()
{
}

void GrabManager::setCaptureDecoration(bool val)
{
    m_captureDecoration = val;
}

void GrabManager::grab( int mode )
{
    m_mode = mode;
    m_stop = false;
    saveGrabConfig();
}

void GrabManager::grabCurrentMode( )
{
    QPixmap currPixmap;
    if (m_mode == -1)
        m_mode = m_defaultMode;

    switch (m_mode)
    {
        case GRAB_RECTANGULAR_REGION:
        {
            RegionGrabber selector;
            if (selector.exec() == QDialog::Accepted)
                currPixmap = selector.getSelection();

            break;
        }
        case GRAB_FREEHAND_REGION:
        {
            FreehandGrabber selector;
            if (selector.exec() == QDialog::Accepted)
                currPixmap = selector.getSelection();

            break;
        }
        case GRAB_WINDOW_UNDER_CURSOR:
        {
            QRect wRect; // position of extraction
            currPixmap = WindowGrabber::grabCurrent(m_captureDecoration, wRect);
            addMouseCursor(currPixmap,wRect);
            break;
        }
        case GRAB_CURRENT_SCREEN:
        {
            QDesktopWidget *desktop = QApplication::desktop();
            int screenId = desktop->screenNumber( QCursor::pos() );
            QRect geom = desktop->screenGeometry( screenId );

            currPixmap = QPixmap::grabWindow( desktop->winId(), geom.x(), geom.y(), geom.width(), geom.height() );
            addMouseCursor(currPixmap,geom);
            break;
        }
        case GRAB_ALL_SCREENS:
        default:
        {
            QRect geom( 0, 0, QApplication::desktop()->width(), QApplication::desktop()->height());
            currPixmap = QPixmap::grabWindow( QApplication::desktop()->winId(), 0, 0, QApplication::desktop()->width(), QApplication::desktop()->height() );
            addMouseCursor(currPixmap,geom);
            break;
        }
    }

    emit newSnapshot(currPixmap);

    // restart if auto grabbing
    if (m_autoGrab && !m_stop)
        QTimer::singleShot( m_delay, this, SLOT( grabCurrentMode() ) );
    else
        restoreGrabConfig();
}

QRectF GrabManager::getDesktopRect () const
{
    QRectF sum;
    QDesktopWidget *desktop = QApplication::desktop();

    for (int screen = 0; screen < desktop->screenCount(); screen++)
    {
        QRect avail = desktop->screenGeometry(screen);
        sum |= avail;
    }
    return sum;
}

void GrabManager::addMouseCursor(QPixmap &pix, const QRect & wRect)
{
    if (!m_addMouseCursor)
        return;

    QPoint mpos = QCursor::pos();

    qDebug() << mpos << wRect;

    if (wRect.contains(mpos))
    {
        //QPixmap curs(":/hotshots/cursor.png");

        QPoint hotspot;
        QPixmap curs = WindowGrabber::grabCursor(hotspot);
        qDebug() <<"hotspot"<<hotspot;
        mpos-=wRect.topLeft();
        mpos-=hotspot;

       qDebug()<<"mpos=" << mpos << wRect;

        QPainter painter(&pix);
        painter.drawPixmap(mpos,curs); 
    }
}

void GrabManager::stopGrab()
{
    m_stop = true;
}

void GrabManager::setDelay(int sec)
{
    m_delay = qMax(defaultMinDelay,sec * 1000);
}

int GrabManager::getDelay() const
{
    return m_delay;
}

void GrabManager::setAddMouseCursor(bool val)
{
    m_addMouseCursor = val;
}

bool GrabManager::getAddMouseCursor() const
{
    return m_addMouseCursor;
}

void GrabManager::setAutoGrab(bool val)
{
    m_autoGrab = val;
}

bool GrabManager::getAutoGrab() const
{
    return m_autoGrab;
}

void GrabManager::setDefaultMode(int val)
{
    m_defaultMode = val;
}

int GrabManager::getDefaultMode() const
{
    return m_defaultMode;
}

void GrabManager::saveGrabConfig()
{
    int delay = m_delay;
    QTimer::singleShot( 0, this, SIGNAL( hideMainInterface() ) );

    // special case on X11
#ifdef Q_OS_UNIX
    static bool firstPass = true;

    if (firstPass && (m_mode == GRAB_RECTANGULAR_REGION || m_mode == GRAB_FREEHAND_REGION))
    {
        QMessageBox::information(NULL,PACKAGE_NAME,tr("Some Windows Managers applies restrictions on the region and freehand capture tools, so the output can be a bit buggy."
                    " If you have such problem, a workaround can be to choose full screen capture then crop the needed part in the editor."));
        firstPass = false;
        delay = qBound(1000,m_delay,10000);
    }
#endif

    QTimer::singleShot( delay, this, SLOT( grabCurrentMode() ) );
}

void GrabManager::restoreGrabConfig()
{
    emit showMainInterface();
}

