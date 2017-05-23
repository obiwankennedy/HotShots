
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

#include <QPainter>
#include <QPixmap>
#include <QtCore/QDebug>

#include "WidgetColor.h"

static const char *ColorTable1 [] =
{
    "#000000", "#ffffff", "#ff0000",
    "#00ff00","#0000ff", "#00ffff",
    "#800080", "#ff8000", "#808080"
};

static int ColorTable [] =
{
    Qt::black, Qt::white, Qt::red,
    Qt::green,Qt::blue, Qt::cyan,
    Qt::magenta, Qt::yellow, Qt::gray
};

//---------------------------------------------------------------------

WidgetColor::WidgetColor (QWidget *parent, const QString &name)
    : WidgetBase (parent, name)
{
    const int w = (width () - 2 /*margin*/ - 2 /*spacing*/) / 3;
    const int h = (height () - 2 /*margin*/ - 3 /*spacing*/) / 3;

    for (unsigned int color = 0; color < sizeof(ColorTable) / sizeof(int); color++)
    {
        QImage previewPixmap (w, h, QImage::Format_ARGB32);
        previewPixmap.fill(Qt::transparent);
        QPainter painter(&previewPixmap);
        painter.fillRect(2,2,previewPixmap.width() - 4,previewPixmap.height() - 4,ColorTable1[color]);

        addOption (QPixmap::fromImage(previewPixmap), colorName (color) /*tooltip*/);

        if (color % 3 == 2)
            startNewOptionRow ();
    }

    finishConstruction (0, 0);
}

//---------------------------------------------------------------------

WidgetColor::~WidgetColor ()
{}

//---------------------------------------------------------------------

QString WidgetColor::colorName (int color) const
{
    QString colorName;

    switch (color)
    {
        case 0:
            colorName = tr ("Black");
            break;
        case 1:
            colorName = tr ("White");
            break;
        case 2:
            colorName = tr ("Red");
            break;
        case 3:
            colorName = tr ("Green");
            break;
        case 4:
            colorName = tr ("Blue");
            break;
        case 5:
            colorName = tr ("Cyan");
            break;
        case 6:
            colorName = tr ("Magenta");
            break;
        case 7:
            colorName = tr ("Orange");
            break;
        case 8:
            colorName = tr ("Gray");
            break;
    }

    return colorName;
}

//---------------------------------------------------------------------

QColor WidgetColor::color () const
{
    return QColor(ColorTable1[selectedCol () + selectedRow () * 3]);
}

//---------------------------------------------------------------------

void WidgetColor::setCurrentColor(const QColor &c)
{
    for (unsigned int color = 0; color < sizeof(ColorTable) / sizeof(int); color++)
        if ( c == QColor(ColorTable1[color]) )
        {
            int row = color / 3;
            int col = color % 3;
            setSelected(row,col);

            return;
        }
}

//---------------------------------------------------------------------

WidgetColor::DrawPackage WidgetColor::drawFunctionDataForRowCol ( const QColor &color, int row, int col)
{
    Q_ASSERT (row >= 0 && col >= 0);

    DrawPackage pack;

    pack.row = row;
    pack.col = col;
    pack.color = color;

    return pack;
}

//---------------------------------------------------------------------

WidgetColor::DrawPackage WidgetColor::drawFunctionData ( const QColor &color) const
{
    return drawFunctionDataForRowCol ( color, selectedRow (), selectedCol () );
}

//---------------------------------------------------------------------

bool WidgetColor::setSelected (int row, int col)
{
    const bool ret = WidgetBase::setSelected (row, col);
    if (ret)
        emit colorChanged ( color() );
    return ret;
}
