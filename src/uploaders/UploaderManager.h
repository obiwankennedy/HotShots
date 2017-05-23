
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

#ifndef _UPLOADERMANAGER_H_
#define _UPLOADERMANAGER_H_

#include <QtCore/QObject>

class BaseUploader;

class UploaderManager : public QObject
{
    Q_OBJECT

public:

    explicit UploaderManager(QObject *parent = 0);
    virtual ~UploaderManager();

    static void registerUploader(BaseUploader*);

    static const QList<BaseUploader *> & getUploaders();

    static void clear();

private:

    static QList<BaseUploader *> m_uploaders;
};

#endif // _UPLOADERMANAGER_H_
