
/*------------------------------------------------------------------------------------------
   QSGML helps to handle a SGML/HTML-File
   Copyright (C) 2010  Andreas Lehmann

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.

   QSgml.h  Copyright (C) 2010  Andreas Lehmann
   This program comes with ABSOLUTELY NO WARRANTY.
   This is free software, and you are welcome to redistribute it
   under certain conditions.
   ------------------------------------------------------------------------------------------*/

#ifndef QSGML_H
#define QSGML_H

#include <QtCore/QString>
#include <QtCore/QRegExp>
#include <QtCore/QList>
#include <QtCore/QFile>
#include <QtCore/QDir>
#include "QSgmlTag.h"

class QSgml
{
public:
    typedef QList<QString>           QSgmlStringlist;

    QSgmlTag *DocTag;
    QSgmlTag *EndTag;
    QString sSgmlString;
    QSgmlStringlist tagExeption;

    QSgml(void);
    QSgml(const QString &SgmlString);
    QSgml(QFile &SgmlFile);
    ~QSgml(void);

    class iterator
    {
    private:
        QSgmlTag *pTag;
    public:
        iterator(void) { pTag = NULL; }
        iterator(QSgmlTag *oTag){ pTag = oTag; }
        ~iterator() {}
        iterator & operator++() { pTag = &pTag->getNextElement(); return(*this); }
        iterator operator++(int) { pTag = &pTag->getNextElement(); return(*this); }
        iterator & operator--() { pTag = &pTag->getPreviousElement(); return(*this); }
        iterator operator--(int) { pTag = &pTag->getPreviousElement(); return(*this); }
        QSgmlTag & operator*() const { return(*pTag); }
        QSgmlTag * operator->() const { return(pTag); }
        bool operator==(iterator &x) {
            if(x.pTag==this->pTag)
                return(true);

            return(false);
        }

        bool operator!=(iterator &x) {
            if(x.pTag!=this->pTag)
                return(true);

            return(false);
        }
    };

    iterator & begin(void)   { return( *( new iterator( &DocTag->getNextElement() ) ) ); }
    iterator & end(void)   { return( *( new iterator(EndTag) ) ); }

    bool load(const QString &sFileName);
    bool save(const QString &sFileName);

    void getElementsByName(const QString &Name,QList<QSgmlTag*> *Elements);
    void getElementsByName(const QString &Name,const QString &AtrName,QList<QSgmlTag*> *Elements);
    void getElementsByName(const QString &Name,const QString &AtrName,const QString &AtrValue,QList<QSgmlTag*> *Elements);
    void getElementsByAtribute(const QString &AtrName,QList<QSgmlTag*> *Elements);
    void getElementsByAtribute(const QString& AtrName,const QString &AtrValue,QList<QSgmlTag*> *Elements);

    void ExportString(QString *HtmlString);
    void ExportString(QString *HtmlString,char Optimze,int Tabsize);
    void String2Sgml(const QString &SgmlString);
protected:
    QDir dirPath;

    void MoveChildren(QSgmlTag *Source, QSgmlTag *Dest);
    void FindStart(const QString &HtmlString,int &iPos);
    void FindEnd(const QString &HtmlString,int &iPos);
    void FindEndComment(const QString &HtmlString,int &iPos);
    void HandleCdata(const QString &SgmlString,QSgmlTag* &pLastTag,int &iStart,int &iEnd,int &iPos);
    void HandleComment(const QString &SgmlString,QSgmlTag* &pLastTag,int &iStart,int &iEnd,int &iPos);

    void HandleDoctype(const QString &SgmlString,QSgmlTag* &pLastTag,int &iStart,int &iEnd,int &iPos);
    void HandleEndTag(const QString &SgmlString,QSgmlTag* &pLastTag,int &iStart,int &iEnd,int &iPos);
    void HandleStartTag(const QString &SgmlString,QSgmlTag* &pLastTag,int &iStart,int &iEnd,int &iPos);
};

#endif // QSGML_H
