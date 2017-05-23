
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

//------------------------------------------------------------------------------
// extract from http://www.qtcentre.org/wiki/index.php?title=SingleApplication
//------------------------------------------------------------------------------

#ifndef _SINGLE_APPLICATION_H_
#define _SINGLE_APPLICATION_H_

#include <QApplication>
#include <QtNetwork/QLocalServer>

#ifndef QT_NO_SHAREDMEMORY // for os/2 compilation
#include <QtCore/QSharedMemory>
#endif // QT_NO_SHAREDMEMORY

class SingleApplication :
    public QApplication
{
    Q_OBJECT

public:

    SingleApplication(int &argc, char *argv[], const QString &uniqueKey);
    ~SingleApplication();

    bool isRunning();
    bool sendMessage(const QString &message);

public slots:

    void receiveMessage();

signals:

    void messageAvailable(const QString &message);

private:

    bool m_isRunning;
    QString m_uniqueKey;

    QLocalServer *m_localServer;

    static const int m_timeout = 1000;

#ifndef QT_NO_SHAREDMEMORY // for os/2 compilation
    QSharedMemory m_sharedMemory;
#endif // QT_NO_SHAREDMEMORY
};

#endif // _SINGLE_APPLICATION_H_
