
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

#ifndef _GRABMANAGER_H_
#define _GRABMANAGER_H_

#include <QtCore/QObject>
#include <QPixmap>

class GrabManager : public QObject
{
    Q_OBJECT

public:

    enum typGrabMode
    {
        GRAB_ALL_SCREENS = 0,
        GRAB_CURRENT_SCREEN,
        GRAB_WINDOW_UNDER_CURSOR,
        GRAB_RECTANGULAR_REGION,
        GRAB_FREEHAND_REGION,
        NB_GRAB_MODE
    };

    GrabManager(QObject *parent = 0);
    ~GrabManager();

    void grab( int mode = -1 );

    void setDelay(int sec);
    int getDelay() const;

    void setAutoGrab(bool);
    bool getAutoGrab() const;

    void setDefaultMode(int);
    int getDefaultMode() const;

    void stopGrab();

    void setCaptureDecoration(bool);

    void setAddMouseCursor(bool);
    bool getAddMouseCursor() const;

signals:

    void hideMainInterface();
    void showMainInterface();
    void newSnapshot(const QPixmap&);

private slots:

    void grabCurrentMode();

private:

    void saveGrabConfig();
    void restoreGrabConfig();
    QRectF getDesktopRect () const;
    void addMouseCursor(QPixmap &pix, const QRect &wRect);

    int m_delay;
    bool m_autoGrab;
    int m_mode;
    int m_defaultMode;
    bool m_stop;
    bool m_captureDecoration;
    bool m_addMouseCursor;
};

#endif // _GRABMANAGER_H_
