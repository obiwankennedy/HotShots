
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

#include "LogHandler.h"
#include <QtCore/QTime>

LogHandler* LogHandler::m_Instance = 0;

LogHandler *LogHandler::getInstance()
{
    static QMutex mutex;

    if (!m_Instance)
    {
        mutex.lock();

        if (!m_Instance)
            m_Instance = new LogHandler;

        mutex.unlock();
    }

    return m_Instance;
}

LogHandler::~LogHandler()
{
    static QMutex mutex;
    mutex.lock();
    m_Instance = 0;
    mutex.unlock();
}

LogHandler::LogHandler(  ) :
    m_currLevel(MSG_DEBUG),
    m_bufferized(true)
{
}

void LogHandler::reportMessage(MessageLevel level, const QString &message )
{
    // message Ignored
    if (level < m_currLevel)
        return;

    QString msgText( message );
    msgText.replace( "<", "&lt;" );
    msgText.replace( ">", "&gt;" );

    // add the current time
    msgText = QTime::currentTime().toString("[hh:mm:ss]: ") + msgText;
    QString msg;

    switch (level)
    {
        case MSG_DEBUG:
        {
            msg = QString( "<font color=\"blue\"><b>DEBUG</b></font>: " ) + msgText;
            break;
        }

        case MSG_INFO:
        {
            msg = QString( "<font color=\"green\"><b>INFO</b>: </font>" ) + msgText;
            break;
        }

        case MSG_WARNING:
        {
            msg = QString( "<font color=\"orange\"><b>WARNING</b>: </font>" ) + msgText;
            break;
        }

        case MSG_ERROR:
        {
            msg = QString( " <font color=\"red\"><b>ERROR</b>:</font>" ) + msgText;
            break;
        }

        default:
        {
            msg = msgText;
            break;
        }
    }

    qDebug( "%s",msg.toStdString().c_str() );
    if (m_bufferized)
        m_buffer << msg;
    else
        emit newMessage(msg);
}

void LogHandler::reportDebug(const QString &message )
{
    reportMessage(IMessageHandler::MSG_DEBUG,message);
}

void LogHandler::reportInfo(const QString &message )
{
    reportMessage(IMessageHandler::MSG_INFO,message);
}

void LogHandler::reportWarning(const QString &message )
{
    reportMessage(IMessageHandler::MSG_WARNING,message);
}

void LogHandler::reportError(const QString &message )
{
    reportMessage(IMessageHandler::MSG_ERROR,message);
}

void LogHandler::setBufferization(bool val)
{
    m_bufferized = val;
    if (!m_bufferized) // send all stored messages
    {
        foreach (const QString &mess, m_buffer)
        {
            emit newMessage(mess);
        }
        m_buffer.clear();
    }
}
