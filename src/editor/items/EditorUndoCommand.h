
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

#ifndef _EDITORUNDOCOMMAND_H_
#define _EDITORUNDOCOMMAND_H_

#include <QtCore/QPointer>

#include <QUndoCommand>

#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>

class BaseItem;

class UndoCommand : public QUndoCommand
{
public:

    UndoCommand(const QString & text, QUndoCommand *parent = 0);
    virtual int id () const {return -1; }

protected:
};

class EditorUndoCommand : public UndoCommand
{
public:

    EditorUndoCommand(BaseItem *item);

    QDomElement & getPrevState()  {return m_prevState; }
    QDomElement & getNextState()  {return m_nextState; }

    static bool equal( const QDomElement & element1, const QDomElement & element2 );

    virtual void undo();
    virtual void redo();

protected:

    QDomDocument m_doc;
    QDomElement m_prevState;
    QDomElement m_nextState;
    QPointer<BaseItem> m_item;
};

#endif // _EDITORUNDOCOMMAND_H_
