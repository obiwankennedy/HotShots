
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

#ifndef _TAGMANAGER_H_
#define _TAGMANAGER_H_

#include <QtCore/QList>

class EditorTagItem;

class TagManager
{
public:

    static void registerTag(EditorTagItem *);
    static void unregisterTag(EditorTagItem *);
    static int getTagNumber();
    static void reset();

private:

    static QList<EditorTagItem *> m_tags;
};

#endif // _TAGMANAGER_H_
