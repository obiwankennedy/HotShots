
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

#ifndef _APPETTINGS_H_
#define _APPETTINGS_H_

#include <QtCore/QSettings>

#define PACKAGE_ORGANIZATION "TheHive"
#define PACKAGE_NAME "HotShots"
#define PACKAGE_VERSION "2.2.0"
#define PACKAGE_ONLINE_HELP_URL "http://thehive.xbee.net/index.php?module=pages&func=display&pageid=31"
#define PACKAGE_CHECK_RELEASE_URL "http://sourceforge.net/projects/hotshots/files"
#define PACKAGE_DESCRIPTION "Screenshot utility"
#define PAYPAL_BUSINESS_ID "Q2SYR2EMYKKYE"

class AppSettings :  public QSettings
{
    Q_OBJECT

public:

    AppSettings( );

protected:
};

#endif // _APPETTINGS_H_
