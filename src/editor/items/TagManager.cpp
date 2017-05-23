
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

#include "TagManager.h"
#include "EditorTagItem.h"

QList<EditorTagItem *> TagManager::m_tags;

int TagManager::getTagNumber()
{
    return m_tags.size();
}

void TagManager::reset()
{
    m_tags.clear();
}

void TagManager::registerTag(EditorTagItem *t)
{
    m_tags << t;
    t->setTag( QString::number( m_tags.size() ) );
}

void TagManager::unregisterTag(EditorTagItem *t)
{
    if ( m_tags.contains(t) )
    {
        m_tags.removeOne(t);
        int n = 1;
        foreach (EditorTagItem * tag, m_tags)
        tag->setTag( QString::number(n++) );
    }
}
