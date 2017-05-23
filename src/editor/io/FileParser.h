
/******************************************************************************
   HotShot: Screenshot utility
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

#ifndef _HOTSHOT_PARSER_H_
#define _HOTSHOT_PARSER_H_

#include <QtXml/QDomDocument>
#include <QtCore/QFile>
#include <QtCore/QObject>

class EditorScene;

class FileParser :
    public QObject
{
    Q_OBJECT

public:

    FileParser();
    ~FileParser();

    bool  load(const QString &filePath, EditorScene *);
    bool save(const QString &filePath, const EditorScene *);

private:

    bool read(QIODevice *device, EditorScene *);
    bool updateVersion();
    void fillDomDoc(QDomDocument &doc, const EditorScene *scn);
    void parse(const QDomNode &node, EditorScene *);

    QDomDocument m_domDocument;
    QFile m_file;
};

#endif // _HOTSHOT_PARSER_H_
