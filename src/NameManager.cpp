
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

#include <QtCore/QDateTime>

#include "NameManager.h"

bool NameManager::addDateTime = false;
QString NameManager::formatDateTime = "yyyy-MM-dd-hh-mm-ss";
QString NameManager::lastDateTime;
QString NameManager::lastSnapshotDirectory;
QString NameManager::lastSnapshotName = "snapshot";
QString NameManager::defaultOutputFormat = "PNG";
int NameManager::currentSnapshotNum = 0;

QString NameManager::getProposedFilename()
{
    QString file;

    if (addDateTime)
    {
        lastDateTime = QDateTime::currentDateTime().toString(formatDateTime);
        file = lastSnapshotDirectory + QString("/%1-%2.%3").arg(lastSnapshotName).arg(lastDateTime).arg( defaultOutputFormat.toLower() );
    }
    else
    {
        if (currentSnapshotNum)
        file = lastSnapshotDirectory + QString("/%1-%2.%3").arg(lastSnapshotName).arg(currentSnapshotNum).arg( defaultOutputFormat.toLower() );
        else // for 0 we don't add the snapshot number
        file = lastSnapshotDirectory + QString("/%1.%2").arg(lastSnapshotName).arg( defaultOutputFormat.toLower() );
    }

    return file;
}
