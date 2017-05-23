
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

#include "WidgetBrush.h"

//---------------------------------------------------------------------

// LOREFACTOR: more OO, no arrays (use safer structs).
/* sync: <brushes> */
static int BrushSizes [][3] =
{
    {8, 4, 1 /*like Pen*/},
    {9, 5, 2},
    {9, 5, 2},
    {9, 5, 2}
};

#define BRUSH_SIZE_NUM_COLS ( int ( sizeof (::BrushSizes [0]) / sizeof (::BrushSizes [0][0]) ) )
#define BRUSH_SIZE_NUM_ROWS ( int ( sizeof (::BrushSizes) / sizeof (::BrushSizes [0]) ) )

//---------------------------------------------------------------------

static void Draw (QImage *destImage, const QPoint &topLeft, void *userData)
{
    WidgetBrush::DrawPackage *pack =
        static_cast <WidgetBrush::DrawPackage *> (userData);

    const int size = ::BrushSizes [pack->row][pack->col];

    QPainter painter(destImage);

    if ( size == 1 )
    {
        painter.setPen(pack->color);
        painter.drawPoint(topLeft);
        return;
    }

    // sync: <brushes>
    switch (pack->row /*shape*/)
    {
        case 0:
        {
            // work around ugly circle when using QPainter on QImage
            if ( size == 4 )
            {
                // do not draw a pixel twice, as with an alpha color it will become darker
                painter.setPen(Qt::NoPen);
                painter.setBrush(pack->color);
                painter.drawRect(topLeft.x() + 1, topLeft.y(), 2, size);
                painter.setPen(pack->color);
                painter.drawLine(topLeft.x(), topLeft.y() + 1, topLeft.x(), topLeft.y() + 2);
                painter.drawLine(topLeft.x() + 3, topLeft.y() + 1, topLeft.x() + 3, topLeft.y() + 2);
            }
            else if ( size == 8 ) // size defined in BrushSizes above
            {
                // do not draw a pixel twice, as with an alpha color it will become darker
                painter.setPen(Qt::NoPen);
                painter.setBrush(pack->color);
                painter.drawRect(topLeft.x() + 2, topLeft.y(), 4, size);
                painter.drawRect(topLeft.x(), topLeft.y() + 2, 2, 4);
                painter.drawRect(topLeft.x() + 6, topLeft.y() + 2, 2, 4);
                painter.setPen(pack->color);
                painter.drawPoint(topLeft.x() + 1, topLeft.y() + 1);
                painter.drawPoint(topLeft.x() + 6, topLeft.y() + 1);
                painter.drawPoint(topLeft.x() + 1, topLeft.y() + 6);
                painter.drawPoint(topLeft.x() + 6, topLeft.y() + 6);
            }
            else
            {
                Q_ASSERT(!"illegal size");
            }
            break;
        }

        case 1:
        {
            // only paint filling so that a color with an alpha channel does not
            // create a darker border due to drawing some pixels twice with composition
            painter.setPen(Qt::NoPen);
            painter.setBrush(pack->color);
            painter.drawRect(topLeft.x(), topLeft.y(), size, size);
            break;
        }

        case 2:
        {
            painter.setPen(pack->color);
            painter.drawLine(topLeft.x() + size - 1, topLeft.y(),
                             topLeft.x(), topLeft.y() + size - 1);
            break;
        }

        case 3:
        {
            painter.setPen(pack->color);
            painter.drawLine(topLeft.x(), topLeft.y(),
                             topLeft.x() + size - 1, topLeft.y() + size - 1);
            break;
        }

        default:
            Q_ASSERT (!"Unknown row");
            break;
    }
}

//---------------------------------------------------------------------

WidgetBrush::WidgetBrush (QWidget *parent, const QString &name)
    : WidgetBase (parent, name)
{
}

//---------------------------------------------------------------------

WidgetBrush::~WidgetBrush ()
{
}

//---------------------------------------------------------------------

void WidgetBrush::init()
{
    reset();
    for (int shape = 0; shape < BRUSH_SIZE_NUM_ROWS; shape++)
    {
        for (int i = 0; i < BRUSH_SIZE_NUM_COLS; i++)
        {
            const int s = ::BrushSizes [shape][i];

            const int w = (width () - 2 /*margin*/ - 2 /*spacing*/)
                          / BRUSH_SIZE_NUM_COLS;
            const int h = (height () - 2 /*margin*/ - 3 /*spacing*/)
                          / BRUSH_SIZE_NUM_ROWS;
            Q_ASSERT (w >= s && h >= s);
            QImage previewPixmap (w, h, QImage::Format_ARGB32);
            previewPixmap.fill(0);

            DrawPackage pack = drawFunctionDataForRowCol (Qt::black, shape, i);
            ::Draw (&previewPixmap,
                    QPoint ( (previewPixmap.width () - s) / 2,
                             (previewPixmap.height () - s) / 2 ),
                    &pack);

            addOption (QPixmap::fromImage(previewPixmap), brushName (shape, i) /*tooltip*/);
        }

        startNewOptionRow ();
    }

    finishConstruction (0, 0);
}

//---------------------------------------------------------------------

QString WidgetBrush::brushName (int shape, int whichSize) const
{
    int s = ::BrushSizes [shape][whichSize];

    if (s == 1)
        return tr ("1x1");

    QString shapeName;

    // sync: <brushes>
    switch (shape)
    {
        case 0:
            shapeName = tr ("Circle");
            break;
        case 1:
            shapeName = tr ("Square");
            break;
        case 2:

            // TODO: is this really the name of a shape? :)
            shapeName = tr ("Slash");
            break;
        case 3:

            // TODO: is this really the name of a shape? :)
            shapeName = tr ("Backslash");
            break;
    }

    if ( shapeName.isEmpty () )
        return QString();

    return QString ("%1x%2 %3").arg(s).arg(s).arg(shapeName);
}

//---------------------------------------------------------------------

int WidgetBrush::brushSize () const
{
    return ::BrushSizes [selectedRow ()][selectedCol ()];
}

//---------------------------------------------------------------------

bool WidgetBrush::brushIsDiagonalLine () const
{
    return (selectedRow () >= 2);
}

//---------------------------------------------------------------------

WidgetBrush::DrawPackage WidgetBrush::drawFunctionDataForRowCol (
    const QColor &color, int row, int col)
{
    Q_ASSERT (row >= 0 && col >= 0);

    DrawPackage pack;

    pack.row = row;
    pack.col = col;
    pack.color = color;

    return pack;
}

//---------------------------------------------------------------------

WidgetBrush::DrawPackage WidgetBrush::drawFunctionData (
    const QColor &color) const
{
    return drawFunctionDataForRowCol ( color, selectedRow (), selectedCol () );
}

//---------------------------------------------------------------------

bool WidgetBrush::setSelected (int row, int col)
{
    const bool ret = WidgetBase::setSelected (row, col);
    if (ret)
        emit brushChanged ();
    return ret;
}
