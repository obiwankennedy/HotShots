
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

#include <QtNetwork/QLocalSocket>

#include "SingleApplication.h"

SingleApplication::SingleApplication(int &argc, char *argv[], const QString &uniqueKey) : QApplication(argc, argv),
    m_isRunning(false),
    m_uniqueKey(uniqueKey),
    m_localServer(NULL)
{
#ifndef QT_NO_SHAREDMEMORY // for os/2 compilation
    m_sharedMemory.setKey(m_uniqueKey);
    if ( m_sharedMemory.attach() )
        m_isRunning = true;
    else
    {
        m_isRunning = false;

        // create shared memory.
        if ( !m_sharedMemory.create(1) )
        {
            qDebug("Unable to create single instance.");
            return;
        }

        // create local server and listen to incomming messages from other instances.
        m_localServer = new QLocalServer(this);
        connect( m_localServer, SIGNAL( newConnection() ), this, SLOT( receiveMessage() ) );
        m_localServer->listen(m_uniqueKey);
    }
#endif
}

SingleApplication::~SingleApplication()
{
#ifndef QT_NO_SHAREDMEMORY // for os/2 compilation
    m_sharedMemory.detach();
#endif
}

// public slots.

void SingleApplication::receiveMessage()
{
    QLocalSocket *localSocket = m_localServer->nextPendingConnection();
    if ( !localSocket->waitForReadyRead(m_timeout) )
    {
        qDebug( "%s",localSocket->errorString().toStdString().c_str() );
        return;
    }
    QByteArray byteArray = localSocket->readAll();
    QString message = QString::fromUtf8( byteArray.constData() );
    emit messageAvailable(message);
    localSocket->disconnectFromServer();
}

// public functions.

bool SingleApplication::isRunning()
{
    return m_isRunning;
}

bool SingleApplication::sendMessage(const QString &message)
{
    if (!m_isRunning)
        return false;

    QLocalSocket localSocket(this);
    localSocket.connectToServer(m_uniqueKey, QIODevice::WriteOnly);
    if ( !localSocket.waitForConnected(m_timeout) )
    {
        qDebug( "%s",localSocket.errorString().toStdString().c_str() );
        return false;
    }

    localSocket.write( message.toUtf8() );
    if ( !localSocket.waitForBytesWritten(m_timeout) )
    {
        qDebug( "%s",localSocket.errorString().toStdString().c_str() );
        return false;
    }
    localSocket.disconnectFromServer();
    return true;
}
