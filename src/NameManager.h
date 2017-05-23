
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

#ifndef _NAMEMANAGER_H_
#define _NAMEMANAGER_H_

#include <QtCore/QString>

//! Class for manage proposed filename across widget

class NameManager
{
public:

    // give a valid directory for temporary files
    static QString getProposedFilename();

    static bool addDateTime;
    static QString formatDateTime;
    static QString lastDateTime;
    static QString lastSnapshotDirectory;
    static QString lastSnapshotName;
    static QString defaultOutputFormat;
    static int currentSnapshotNum;
};

#endif // _NAMEMANAGER_H_
