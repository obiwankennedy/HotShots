
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

#ifndef _LOGHANDLER_H_
#define _LOGHANDLER_H_

#include "IMessageHandler.h"

#include <QtCore/QObject>
#include <QtCore/QMutex>
#include <QtCore/QStringList>

//! A dialog for displaying message from reader/writer

/*!
 *	Displaying colored message by taking care of the message level
 */

class LogHandler :
    public QObject, public IMessageHandler
{
    Q_OBJECT

public:

    static LogHandler *getInstance();
    ~LogHandler();

    void setMessageLevel(MessageLevel level)
    {
        m_currLevel = level;
    }

    void setBufferization(bool);

public slots:

    void reportMessage(MessageLevel level, const QString &message );
    void reportDebug(const QString &message );
    void reportInfo(const QString &message );
    void reportWarning(const QString &message );
    void reportError(const QString &message );

signals:

    void newMessage(const QString &);

private:

    LogHandler(  );

    LogHandler(const LogHandler &); // hide copy constructor
    LogHandler& operator=(const LogHandler &); // hide assign op
    // we leave just the declarations, so the compiler will warn us
    // if we try to use those two functions by accident

private:

    static LogHandler* m_Instance;
    MessageLevel m_currLevel;
    QStringList m_buffer;
    bool m_bufferized;
};

#endif  // _LOGHANDLER_H_
