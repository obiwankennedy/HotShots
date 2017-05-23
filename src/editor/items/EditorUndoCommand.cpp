
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

#include <QtCore/QDebug>

#include "EditorUndoCommand.h"
#include "BaseItem.h"

UndoCommand::UndoCommand(const QString & text, QUndoCommand *parent)
    :   QUndoCommand(text,parent)
{
}

//------------------------------------------------------------------------------

EditorUndoCommand::EditorUndoCommand(BaseItem *item)
    :   UndoCommand("EditorUndoCommand")
{
    m_prevState = m_doc.createElement( "dummy" );
    m_item = item;
    if (m_item)
        m_item->toXml(m_prevState);
}

void EditorUndoCommand::undo()
{
    qDebug() << "EditorUndoCommand::undo " << m_item << " " << m_item->boundingRect();
    if (!m_item)
        return;

    m_item->fromXml(m_prevState);
    m_item->endEdition();
    m_item->update();
}

void EditorUndoCommand::redo()
{
    qDebug() << "EditorUndoCommand::redo " << m_item << " " << m_item->boundingRect();
    if (!m_item)
        return;

    m_item->fromXml(m_nextState);
    m_item->endEdition();
    m_item->update();
}

bool EditorUndoCommand::equal( const QDomElement & element1, const QDomElement & element2 )
{
    QDomNamedNodeMap map1 = element1.attributes ();
    QDomNamedNodeMap map2 = element2.attributes ();

    for(int i = 0; i<map1.count(); i++)
    {
        QDomNode node1 = map1.item(i);
        QDomAttr attr1 = node1.toAttr();
        if( attr1.isNull() )
            continue;
        QString attr1Value = attr1.value();
        qDebug() << "map = " << attr1.name() << " " << attr1Value;
        if ( !map2.contains( attr1.name() ) )
            return false;

        QDomNode node2 = map2.namedItem( attr1.name() );
        QDomAttr attr2 = node2.toAttr();
        if ( attr1Value != attr2.value() )
            return false;
    }

    return true;
}
