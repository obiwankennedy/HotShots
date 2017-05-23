
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

   QSgml.cpp  Copyright (C) 2010  Andreas Lehmann
   This program comes with ABSOLUTELY NO WARRANTY.
   This is free software, and you are welcome to redistribute it
   under certain conditions.
   ------------------------------------------------------------------------------------------*/

#include "QSgml.h"

// find the start of a tag
void QSgml::FindStart(const QString &HtmlString,int &iPos)
{
    iPos = HtmlString.indexOf("<",iPos);
}

// find the end of a tag
// make sure the '>' is not in a quote
void QSgml::FindEnd(const QString &HtmlString,int &iPos)
{
    for(; iPos<HtmlString.length(); iPos++ )
    {
        // Its a tag end
        if( HtmlString.at(iPos)=='>' )
        {
            return;
        }
        if( HtmlString.at(iPos)=='\'' )
        {
            iPos = HtmlString.indexOf("\'",iPos + 1);
        }
        if( HtmlString.at(iPos)=='\"' )
        {
            iPos = HtmlString.indexOf("\"",iPos + 1);
        }
    }
    iPos = -1;
}

// find and of a comment
void QSgml::FindEndComment(const QString &HtmlString,int &iPos)
{
    iPos = HtmlString.indexOf("-->",iPos);
}

// create a html-file as string with default optimization
void QSgml::ExportString(QString *HtmlString)
{
    ExportString(HtmlString,2,2);
}

// create a html-file as string
void QSgml::ExportString(QString *HtmlString,char Optimze,int Tabsize)
{
    QSgmlTag *pTag = DocTag->Children[0];
    QList<QSgmlTag*> StartTags;
    QString sAtr;
    QString sSpc;
    QString sTab = "";
    QString sWrap = "";
    QHash<QString,QString>::iterator i;

    // set new-line and tab
    if( Optimze>0 )
    {
        int i;
        sWrap = "\n";

        // create the tab in spaces
        for( i = 0; i<Tabsize; i++ )
            sTab += " ";
    }

    HtmlString->clear();

    while( pTag->Type!=QSgmlTag::eVirtualEndTag )
    {
        // create spaces in front
        for( int j = 0; j<pTag->Level - 1; j++ )
            sSpc += sTab;

        switch( pTag->Type )
        {
            case QSgmlTag::eVirtualBeginTag:
            case QSgmlTag::eVirtualEndTag:

                // nothing to do
                break;
            case QSgmlTag::eCdata:
                HtmlString->append(pTag->Value);
                break;
            case QSgmlTag::eComment:
                HtmlString->append(sSpc + "<!--" + pTag->Value + "-->");
                break;
            case QSgmlTag::eDoctype:
                HtmlString->append(sSpc + "<!" + pTag->Value + ">");
                break;
            case QSgmlTag::eEndTag:
                HtmlString->append(sSpc + "</" + pTag->Name + ">");
                break;
            case QSgmlTag::eStandalone:
                sAtr = "";
                for( i = pTag->Attributes.begin(); i!=pTag->Attributes.end(); ++i )
                {
                    QString sKey = i.key();
                    sAtr += " " + sKey + "=\"" + pTag->Attributes.value(sKey) + "\"";
                }
                HtmlString->append(sSpc + "<" + pTag->Name + sAtr + "/>");
                break;
            case QSgmlTag::eStartTag:
                sAtr = "";
                for( i = pTag->Attributes.begin(); i != pTag->Attributes.end(); ++i )
                {
                    QString sKey = i.key();
                    sAtr += " " + sKey + "=\"" + pTag->Attributes.value(sKey) + "\"";
                }
                HtmlString->append(sSpc + "<" + pTag->Name + sAtr + ">");
                StartTags.append(pTag);
                break;
            case QSgmlTag::eStartEmpty:
                sAtr = "";
                for( i = pTag->Attributes.begin(); i != pTag->Attributes.end(); ++i )
                {
                    QString sKey = i.key();
                    sAtr += " " + sKey + "=\"" + pTag->Attributes.value(sKey) + "\"";
                }
                HtmlString->append(sSpc + "<" + pTag->Name + sAtr + "></" + pTag->Name + ">");
                break;
            case QSgmlTag::eNoTag:
                break;
        }

        int iDummy = pTag->Level;
        QSgmlTag::TagType eLastType = pTag->Type;
        pTag = &pTag->getNextElement();
        int iLevelDiff = iDummy - pTag->Level;
        if( iLevelDiff>0 )
        {
            // with all end-tags
            for( int j = 0; j<iLevelDiff; j++ )
                // if there are any left
                if( StartTags.count()>0 )
                {
                    // create spaces in front
                    if( (j!=0)||(eLastType!=QSgmlTag::eCdata) ) // last tag was an endtag or not a CDATA-tag
                    {
                        sSpc = sWrap;
                        for( int i = 0; i<StartTags.last()->Level - 1; i++ )
                            sSpc += sTab;
                    }
                    else
                    {
                        // last tag was a CDATA-tag
                        sSpc = "";
                    }

                    // add the tag
                    HtmlString->append(sSpc + "</" + StartTags.last()->Name + ">");
                    StartTags.removeLast();
                }
        }

        // reset the space in front for the next item.
        sSpc = sWrap;
    }
}

// move all children (with children) to an other parent
void QSgml::MoveChildren(QSgmlTag *Source, QSgmlTag *Dest)
{
    int i;
    int iCount = Source->Children.count();

    for( i = 0; i<iCount; i++ )
    {
        Source->Children[0]->Parent = Dest;

        //Source->Children[0]->Level = Dest->Level+1;  // ToDo: Level von allen Kindern auch decrementieren!
        Dest->resetLevel();
        Dest->Children.append( Source->Children[0] );
        Source->Children.removeFirst();
    }
}

// include a CDATA in to the QSgml-class
void QSgml::HandleCdata(const QString &SgmlString,QSgmlTag* &pLastTag,int &iStart,int &iEnd,int &iPos)
{
    QString sDummy;
    QRegExp qNoWhitSpace("(\\S)");
    
    iStart = iEnd + 1;
    iEnd = iPos;
    sDummy = SgmlString.mid(iStart,iEnd - iStart).trimmed();

    // set tag
    if( sDummy.contains(qNoWhitSpace) )
    {
        QSgmlTag *pTag = new QSgmlTag(sDummy,QSgmlTag::eCdata,pLastTag);
        pTag->StartTagPos = iStart;
        pTag->StartTagLength = iEnd - iStart;
        pTag->EndTagPos = iStart;
        pTag->EndTagLength = iEnd - iStart;
        pLastTag->Children.append( pTag );
    }
}

// include a comment in to the QSgml-class
void QSgml::HandleComment(const QString &SgmlString,QSgmlTag* &pLastTag,int &iStart,int &iEnd,int &iPos)
{
    QString sDummy;
    ;

    iPos += 4;
    iStart = iPos;
    FindEndComment(SgmlString,iPos);
    iEnd = iPos;

    sDummy = SgmlString.mid(iStart,iEnd - iStart).trimmed();
    QSgmlTag *pTag = new QSgmlTag(sDummy,QSgmlTag::eComment,pLastTag);
    pTag->StartTagPos = iStart - 4;
    pTag->StartTagLength = iEnd - iStart + 7;
    pTag->EndTagPos = iStart - 4;
    pTag->EndTagLength = iEnd - iStart + 7;
    pLastTag->Children.append( pTag );
    iEnd += 2;
    iPos = iEnd;
}

// include a doctype in to the QSgml-class
void QSgml::HandleDoctype(const QString &SgmlString,QSgmlTag* &pLastTag,int &iStart,int &iEnd,int &iPos)
{
    QString sDummy;
    QSgmlTag *pTag;

    iStart = iPos;
    FindEnd(SgmlString,iPos);
    iEnd = iPos;
    sDummy = SgmlString.mid(iStart + 2,iEnd - iStart - 2).trimmed();

    pTag = new QSgmlTag(sDummy,QSgmlTag::eDoctype,pLastTag);
    pTag->StartTagPos = iStart;
    pTag->StartTagLength = iEnd - iStart + 1;
    pTag->EndTagPos = iStart;
    pTag->EndTagLength = iEnd - iStart + 1;

    pLastTag->Children.append( pTag );
}

// include a endtag in to the QSgml-class
void QSgml::HandleEndTag(const QString &SgmlString,QSgmlTag* &pLastTag,int &iStart,int &iEnd,int &iPos)
{
    QString sDummy;
    QSgmlTag *pTag;
    QSgmlTag *pDummyTag;

    iStart = iPos;
    FindEnd(SgmlString,iPos);
    iEnd = iPos;
    sDummy = SgmlString.mid(iStart + 1,iEnd - iStart - 1).trimmed();

    pTag = new QSgmlTag(sDummy,QSgmlTag::eEndTag,pLastTag);

    // find a fitting start-tag
    pDummyTag = pLastTag;
    while( (pDummyTag->Name!=pTag->Name)&&(pDummyTag->Parent!=NULL) )
        pDummyTag = pDummyTag->Parent;
    delete pTag;

    if( pDummyTag->Parent!=NULL ) // start-tag found
    {
        while( pLastTag!=pDummyTag ) // all tags in between are standalone tags
        {
            pLastTag->Type = QSgmlTag::eStandalone;
            MoveChildren( pLastTag,pLastTag->Parent );
            pLastTag = pLastTag->Parent;
        }

        // set data in start-tag
        pLastTag->EndTagPos = iStart;
        pLastTag->EndTagLength = iEnd - iStart + 1;

        // tags which have no children are special (script can't be a standalone-tag)
        if( pLastTag->Children.count()==0 )
        {
            pLastTag->Type = QSgmlTag::eStartEmpty;
        }
        pLastTag = pLastTag->Parent;
    }
    else
    {
        // no start-tag -> end
        pLastTag->Children.append( EndTag );
        iPos = -1;
    }
}

// include a start-tag in to the QSgml-class
void QSgml::HandleStartTag(const QString &SgmlString,QSgmlTag* &pLastTag,int &iStart,int &iEnd,int &iPos)
{
    QString sDummy;
    QSgmlTag *pTag;

    iStart = iPos;
    FindEnd(SgmlString,iPos);
    iEnd = iPos;
    sDummy = SgmlString.mid(iStart + 1,iEnd - iStart - 1).trimmed();

    if( SgmlString.at(iEnd - 1)=='/' )
    {
        // this is a standalone-tag
        sDummy = sDummy.left( sDummy.count() - 1 );
        pTag = new QSgmlTag(sDummy,QSgmlTag::eStandalone,pLastTag);
        pTag->StartTagPos = iStart;
        pTag->StartTagLength = iEnd - iStart + 1;
        pTag->EndTagPos = iStart;
        pTag->EndTagLength = iEnd - iStart + 1;
        pLastTag->Children.append( pTag );
    }
    else
    {
        // this is a start-tag
        pTag = new QSgmlTag(sDummy,QSgmlTag::eStartTag,pLastTag);
        pTag->StartTagPos = iStart;
        pTag->StartTagLength = iEnd - iStart + 1;
        pTag->EndTagPos = iStart;
        pTag->EndTagLength = iEnd - iStart + 1;
        pLastTag->Children.append( pTag );
        pLastTag = pTag;
    }
}

// find an element with a defined name
void QSgml::getElementsByName(const QString &Name,QList<QSgmlTag*> *Elements)
{
    QSgmlTag *Tag = DocTag;

    Elements->clear();
    while( Tag->Type!=QSgmlTag::eVirtualEndTag )
    {
        if( Tag->Name==Name.toLower() )
        {
            Elements->append(Tag);
        }
        Tag = &Tag->getNextElement();
    }
}

// find an element with a defined name and an atribte
void QSgml::getElementsByName(const QString &Name,const QString &AtrName,QList<QSgmlTag*> *Elements)
{
    QSgmlTag *Tag = DocTag;

    Elements->clear();
    while( Tag->Type!=QSgmlTag::eVirtualEndTag )
    {
        if( (Tag->Name==Name)&&(Tag->hasAttribute(AtrName)==true) )
        {
            Elements->append(Tag);
        }
        Tag = &Tag->getNextElement();
    }
}

// find an element with a defined name and an atribte with a value
void QSgml::getElementsByName(const QString &Name,const QString &AtrName,const QString &AtrValue,QList<QSgmlTag*> *Elements)
{
    QSgmlTag *Tag = DocTag;

    Elements->clear();
    while( Tag->Type!=QSgmlTag::eVirtualEndTag )
    {
        if( (Tag->Name==Name)&&(Tag->hasAttribute(AtrName)==true)&&(Tag->Attributes.value(AtrName)==AtrValue) )
        {
            Elements->append(Tag);
        }
        Tag = &Tag->getNextElement();
    }
}

// find an element with a defined attribute-name
void QSgml::getElementsByAtribute(const QString &AtrName,QList<QSgmlTag*> *Elements)
{
    QSgmlTag *Tag = DocTag;

    Elements->clear();
    while( Tag->Type!=QSgmlTag::eVirtualEndTag )
    {
        if( Tag->Attributes.find(AtrName)!=Tag->Attributes.end() )
        {
            Elements->append( Tag );
        }
        Tag = &Tag->getNextElement();
    }
}

// find an element with a defined attribute-name and attribute-value
void QSgml::getElementsByAtribute(const QString & AtrName,const QString & AtrValue,QList<QSgmlTag*> *Elements)
{
    QSgmlTag *Tag = DocTag;

    Elements->clear();
    while( Tag->Type!=QSgmlTag::eVirtualEndTag )
    {
        if( Tag->Attributes.find(AtrName)!=Tag->Attributes.end() )
        {
            if( Tag->Attributes[AtrName]==AtrValue )
            {
                Elements->append( Tag );
            }
        }
        Tag = &Tag->getNextElement();
    }
}

// load from a file
bool QSgml::load(const QString &sFileName)
{
    QFile fileText( QDir::cleanPath(sFileName) );
    bool qExists = fileText.exists();

    // delete old elements
    delete DocTag;

    //delete EndTag;
    // create new doc-tag
    DocTag = new QSgmlTag("DocTag",QSgmlTag::eVirtualBeginTag,NULL);
    EndTag = new QSgmlTag("EndTag",QSgmlTag::eVirtualEndTag,DocTag);

    // set EndTag as only Child of DocTag
    DocTag->Children.append(EndTag);

    // read the file
    if( qExists==true )
    {
        fileText.open(QIODevice::ReadOnly);
        sSgmlString = fileText.readAll();
        dirPath = QFileInfo(sFileName).absoluteDir();
    }

    // create elements
    String2Sgml(sSgmlString);

    return(qExists);
}

// save to a file
bool QSgml::save(const QString &sFileName)
{
    QFile fileText( QDir::cleanPath(sFileName) );
    qint64 s64_Count = 0;

    // write to file
    if( fileText.open(QIODevice::WriteOnly)==true )
    {
        s64_Count = fileText.write( sSgmlString.toLocal8Bit() );
        dirPath = QFileInfo(sFileName).absoluteDir();
    }

    if( s64_Count>=0 )
        return(true);
    else
        return(false);
}

// Constructor
QSgml::QSgml(void)
{
    // create DocTag and EndTag
    DocTag = new QSgmlTag("DocTag",QSgmlTag::eVirtualBeginTag,NULL);
    EndTag = new QSgmlTag("EndTag",QSgmlTag::eVirtualEndTag,DocTag);

    // set EndTag as only Child of DocTag
    DocTag->Children.append(EndTag);

    // Set Path to Default-Path
    dirPath = QDir::homePath();

    // should be done externaly later
    tagExeption.append("script");
    tagExeption.append("style");
}

// Constructor
QSgml::QSgml(const QString &SgmlString)
{
    // create DocTag and EndTag
    DocTag = new QSgmlTag("DocTag",QSgmlTag::eVirtualBeginTag,NULL);
    EndTag = new QSgmlTag("EndTag",QSgmlTag::eVirtualEndTag,DocTag);

    // set EndTag as only Child of DocTag
    DocTag->Children.append(EndTag);

    // Set Path to Default-Path
    dirPath = QDir::homePath();

    // should be done externaly later
    tagExeption.append("script");
    tagExeption.append("style");

    // Additional set string
    String2Sgml(SgmlString);
}

// Constructor
QSgml::QSgml(QFile &SgmlFile)
{
    // create DocTag and EndTag
    DocTag = new QSgmlTag("DocTag",QSgmlTag::eVirtualBeginTag,NULL);
    EndTag = new QSgmlTag("EndTag",QSgmlTag::eVirtualEndTag,DocTag);

    // set EndTag as only Child of DocTag
    DocTag->Children.append(EndTag);

    // Set Path to Path
    dirPath = QFileInfo(SgmlFile).dir();

    // should be done externaly later
    tagExeption.append("script");
    tagExeption.append("style");

    // read the file
    if( SgmlFile.exists()==true )
    {
        SgmlFile.open(QIODevice::ReadOnly);
        sSgmlString = SgmlFile.readAll();
    }

    // Additional set string
    String2Sgml(sSgmlString);
}

// convert a String to QSgml
void QSgml::String2Sgml(const QString &SgmlString)
{
    QSgmlTag *LastTag;
    QString sDummy;
    int iPos = 0;
    int iStart = 0,iEnd = 0;
    QList<QString>::iterator i;

    sSgmlString = SgmlString;

    DocTag->Children.clear();

    LastTag = DocTag;

    do
    {
        // Handle exception-tags
        for( i = tagExeption.begin(); i!=tagExeption.end(); ++i )
        {
            QString sName = *i;
            if( LastTag->Name.toLower()==sName ) // its an exception-tag
            {
                iPos = SgmlString.toLower().indexOf("</" + sName,iPos);
                iPos--;
            }
        }

        FindStart(SgmlString,iPos);

        // no new start
        if( iPos==-1 )
        {
            LastTag->Children.append( EndTag );
            break;
        }

        // there was CDATA
        else if( iPos > iEnd + 1 )
        {
            HandleCdata(SgmlString,LastTag,iStart,iEnd,iPos);
        }

        // this is a comment
        if( (SgmlString.at(iPos + 1)=='!')&&(SgmlString.at(iPos + 2)=='-')&&(SgmlString.at(iPos + 3)=='-') )
        {
            HandleComment(SgmlString,LastTag,iStart,iEnd,iPos);
        }

        // this is a PI
//      else if( SgmlString.at(iPos+1)=='?' )
//      {
//         HandleDoctype(SgmlString,LastTag,iStart,iEnd,iPos);
//      }
        // this is a Doctype
        else if( SgmlString.at(iPos + 1)=='!' )
        {
            HandleDoctype(SgmlString,LastTag,iStart,iEnd,iPos);
        }

        // this is an Endtag
        else if( SgmlString.at(iPos + 1)=='/' )
        {
            HandleEndTag(SgmlString,LastTag,iStart,iEnd,iPos);
        }

        // this is an Starttag of Standalone
        else
        {
            HandleStartTag(SgmlString,LastTag,iStart,iEnd,iPos);
        }
    }
    while( iPos!=-1 );
}

// destructor
QSgml::~QSgml(void)
{
    delete DocTag;
}
