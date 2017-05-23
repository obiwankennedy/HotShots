
/*------------------------------------------------------------------------------------------
   QSgmlTag helps to handle a SGML/HTML-Tag
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

   QSgmlTag.cpp  Copyright (C) 2010  Andreas Lehmann
   This program comes with ABSOLUTELY NO WARRANTY.
   This is free software, and you are welcome to redistribute it
   under certain conditions.
   ------------------------------------------------------------------------------------------*/

#include "QSgmlTag.h"

//QSgmlTag Notag( "NoTag",QSgmlTag::eNoTag,(QSgmlTag&)NULL );

// set the type of the tag
void QSgmlTag::SetType(const QString &InnerTag)
{
    // Prepareation
    QString sDummy = InnerTag.trimmed();

    if( sDummy.at(InnerTag.count() - 1)=='/' )
    {
        Type = eEndTag;
    }
    else if( sDummy.at(0)=='!' )
    {
        Type = eDoctype;
    }
    else if( sDummy.at(0)=='/' )
    {
        Type = eStandalone;
    }
    else
    {
        Type = eStartTag;
    }
}

// set the names and attributes of this tag
void QSgmlTag::SetNameAttributes(const QString &InnerTag)
{
    QString AllAtr;
    QString AtrString;
    QString NameString;
    QString AttrName;
    QString sDummy;
    QString Atribute;

    // Prepareation
    sDummy = InnerTag.trimmed();

    // --- find name of tag ---
    int EndName = sDummy.indexOf(" ");
    Name = sDummy.left(EndName).trimmed().toLower();
    if( (Name.at(0)=='!')||(Name.at(0)=='/') )
    {
        Name = Name.mid(1);
    }

    // --- find attributes ---
    AllAtr = sDummy.trimmed().mid(EndName).trimmed();

    // First get the attr-name
    EndName = AllAtr.indexOf("=");
    AttrName = AllAtr.left(EndName).trimmed();
    AllAtr = AllAtr.mid(EndName + 1).trimmed();

    // if there is a atr-name get atr-value and next atr-name
    while( EndName!=-1 )
    {
        EndName = AllAtr.indexOf("=");
        int PosQot = AllAtr.indexOf("\"");

        // Save Name of Attribute
        NameString = AttrName.toLower();

        if( PosQot<EndName ) // find end of quote and "=" afterwards
        {
            int iDummy = AllAtr.indexOf("\"",PosQot + 1);
            EndName = AllAtr.indexOf("=",iDummy + 1);
        }
        if( EndName==-1 ) // no "=" -> all attributes
        {
            AtrString = AllAtr;
        }
        else
        {
            sDummy = AllAtr.left(EndName);

            int StartName = sDummy.lastIndexOf( QRegExp("[ \r\n\t]") );
            AtrString = AllAtr.left(StartName);

            AttrName = AllAtr.mid(StartName,EndName - StartName).trimmed();
            AllAtr = AllAtr.mid(EndName + 1).trimmed();
        }

        // set attribute hash; ignore " if exist
        AtrString = AtrString.trimmed();
        if( (AtrString.at(0)=='\"')||(AtrString.at(AtrString.count() - 1)=='\"') )
        {
            QString sDummy = AtrString.mid(1,AtrString.count() - 2);
            Attributes[NameString] = sDummy;
        }
        else if( (AtrString.at(0)=='\'')||(AtrString.at(AtrString.count() - 1)=='\'') )
        {
            QString sDummy = AtrString.mid(1,AtrString.count() - 2);
            Attributes[NameString] = sDummy;
        }
        else
        {
            Attributes[NameString] = AtrString;
        }
    }
}

// check if attribute has the value
bool QSgmlTag::checkAttribute(const QString &AtrName,const QString &AtrValue)
{
    QString sValue = Attributes.value(AtrName);
    if( sValue==AtrValue )
    {
        return true;
    }
    else
    {
        return false;
    }
}

// get the next tag on the same lefel
QSgmlTag& QSgmlTag::getNextSibling(void)
{
    int i;

    for( i = 0; i<Parent->Children.count(); i++ )
        if( (Parent->Children[i])==this )
            break;
    if( i==(Parent->Children.count() - 1) )
    {
        return( *( (QSgmlTag*)NULL ) );
    }
    else
    {
        return( *Parent->Children[i + 1] );
    }
}

// get the previous tag on the same lefel
QSgmlTag& QSgmlTag::getPreviousSibling(void)
{
    int i;

    for( i = 0; i<Parent->Children.count(); i++ )
        if( (Parent->Children[i])==this )
            break;
    if( i==0 )
    {
        return( *( (QSgmlTag*)NULL ) );
    }
    else
    {
        return( *Parent->Children[i - 1] );
    }
}

// get the next tag
QSgmlTag& QSgmlTag::getNextElement(void)
{
    QSgmlTag *Return = NULL;

    // are there children
    if( this->Children.count()>0 )
    {
        Return = this->Children[0];
    }
    else
    {
        // search the next parent with sibling
        Return = this;
        while( &Return->getNextSibling()==NULL )
            Return = Return->Parent;
        Return = &( Return->getNextSibling() );
    }
    return( *Return );
}

// get the previous tag
QSgmlTag& QSgmlTag::getPreviousElement(void)
{
    QSgmlTag *Return = NULL;

    // Is there a previous tag on the same level
    Return = &( this->getPreviousSibling() );
    if( &Return==NULL )
    {
        Return = this->Parent;
    }
    else
    {
        // search the last child with no children
        while( Return->Children.count()!=0 )
            Return = Return->Children.last();
    }
    return( *Return );
}

// get the value of an argument
QString QSgmlTag::getArgValue(const QString &Key)
{
    return( Attributes.value(Key) );
}

// reset the level of the tag
// and all childes and childs of the childs
void QSgmlTag::resetLevel(void)
{
    QList<QSgmlTag*>::const_iterator i;

    Level = Parent->Level + 1;

    for( i = Children.constBegin(); i!=Children.constEnd(); ++i )
    {
        QSgmlTag* t = *i;
        t->resetLevel();
    }
}

// returns true if the tag has an Atribute "AtrName"
bool QSgmlTag::hasAttribute(const QString &AtrName)
{
    return Attributes.contains(AtrName);
}

// add a child of this element
QSgmlTag* QSgmlTag::addChild(const QString &InnerTag, TagType eType)
{
    QSgmlTag * pnewTag = new QSgmlTag;
    QSgmlTag * tagRet = pnewTag;

    // don't add childs to that eDoctype
    if( this->Type==eDoctype )
    {
        tagRet = NULL;
    }

    // change type of this tag if it was eStandalone
    if( Type==eStandalone )
    {
        Type = eStartTag;
    }

    // set name or value
    switch( eType )
    {
        case eVirtualBeginTag:
        case eVirtualEndTag:
        case eEndTag:
        case eNoTag:

            // can't add childs of this type
            tagRet = NULL;
            break;
        case eStartTag:
        case eStandalone:
        case eDoctype:
        case eStartEmpty:
            pnewTag->SetType(InnerTag);
            pnewTag->SetNameAttributes(InnerTag);
            break;
        case eCdata:
        case eComment:
            pnewTag->Value = InnerTag;
            break;
    }

    // set Type
    pnewTag->Type = eType;

    // set Level
    pnewTag->Level = Level + 1;

    // set Parent
    pnewTag->Parent = this;

    // add the new tag
    if( tagRet!=NULL )
    {
        if( Level==0 )
            this->Children.insert(Children.count() - 1,pnewTag);
        else
            this->Children.append(pnewTag);
    }

    return(tagRet);
}

// constructor
QSgmlTag::QSgmlTag(void):
    Level(0),
    Parent(NULL),
    Type((QSgmlTag::TagType)-1),
    StartTagPos(0),
    StartTagLength(0),
    EndTagPos(0),
    EndTagLength(0)
{

}

// constructor
QSgmlTag::QSgmlTag(const QString &InnerTag):
    Level(0),
    Parent(NULL),
    Type((QSgmlTag::TagType)-1),
    StartTagPos(0),
    StartTagLength(0),
    EndTagPos(0),
    EndTagLength(0)
{
    SetType(InnerTag);
    SetNameAttributes(InnerTag);
}

// constructor
QSgmlTag::QSgmlTag(const QString &InnerTag,TagType eType,QSgmlTag *tParent)
{
    Type = eType;
    Parent = tParent;

    if( tParent==NULL )
    {
        Level = 0;
    }
    else
    {
        Level = tParent->Level + 1;
    }

    if( (eType!=eDoctype)&&(eType!=eCdata)&&(eType!=eComment) )
    {
        SetNameAttributes(InnerTag);
        Value = "";
    }
    else
    {
        Value = InnerTag;
    }
}

// destructor
QSgmlTag::~QSgmlTag(void)
{
    int i;
    int iCount = Children.count();

    for( i = 0; i<iCount; i++ )
    {
        delete Children[0];
        Children.removeFirst();
    }
}
