
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

#ifndef _SPLASHSCREEN_H_
#define _SPLASHSCREEN_H_

#include <QSplashScreen>
#include <QWidget>
#include <QPixmap>
#include <QPaintEvent>
#include <QPainter>

class SplashScreen :
    public QSplashScreen
{
    Q_OBJECT

public:

    SplashScreen(const QPixmap& pixmap, unsigned int minimalDisplayDuration = 0, int screenId = -1, bool transparentBackground = true, Qt::WindowFlags f = 0);

    void delayedFinish(QWidget *next = 0);
    void showMessage(const QString &message, int alignment = Qt::AlignLeft, const QColor &color = Qt::black);

signals:

    void finished();

protected:

    void drawContents(QPainter *painter);

private slots:

    void closeEvent(QCloseEvent *event);
    void timerEvent(QTimerEvent *event);

private:

    void init(unsigned int minimalDisplayDuration);
    void setSplash(const QPixmap& pixmap,bool transparentBackground = true);

    QWidget *m_nextWidget;
    bool m_timerFired;
    QPixmap m_pixmap;
    QString m_currStatus;
    int m_currAlign;
    QColor m_currColor;
    int m_screenId;
    int m_refreshId;
};

#endif // _SPLASHSCREEN_H_
