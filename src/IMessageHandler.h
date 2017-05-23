
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

#ifndef _IMESSAGEHANDLER_H_
#define _IMESSAGEHANDLER_H_

#include <QtCore/QString>

//! The Message handler, which can be implemented by the client

class IMessageHandler
{
public:

    enum MessageLevel
    {
        MSG_DEBUG = 0,                      /*!< Message only for debugging purpose. */
        MSG_INFO,                           /*!< Information message. */
        MSG_WARNING,                            /*!< Warning, abnormal event. */
        MSG_ERROR                           /*!< Error, invalid file. */
    };

    //! Virtual destructor
    virtual ~IMessageHandler() {}

    //! This method is called  when a message is to be reported to the client
    virtual void reportMessage( MessageLevel level, const QString & message ) = 0;

    //! Report only a debug message
    virtual void reportDebug(const QString & message ) = 0;

    //! Report only a debug message
    virtual void reportInfo(const QString & message ) = 0;

    //! Report only a debug message
    virtual void reportWarning(const QString & message ) = 0;

    //! Report only a debug message
    virtual void reportError(const QString & message ) = 0;
};

//! Sets the error handler to be used for error and warning reporting

/*!
 *	This allows the user to set the current Message handler. If the
 *	parameter is NULL, API reverts back to the default error handler,
 *	which redirects all warnings and errors to std::cerr.
 *
 *	\sa IMessageHandler
 */
void setMessageHandler( IMessageHandler* Hdl );

#endif  // _IMESSAGEHANDLER_H_
