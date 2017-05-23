
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

#include <QBitmap>
#include <QPainter>
#include <QPixmap>
#include <QColor>

#include "WidgetLineWidth.h"

static int lineWidths [] = {1, 2, 3, 5, 8};

WidgetLineWidth::WidgetLineWidth (QWidget *parent, const QString &name)
    : WidgetBase (parent, name)
{
    init();
}

WidgetLineWidth::~WidgetLineWidth ()
{
}

void WidgetLineWidth::init()
{
    reset();

    int numLineWidths = sizeof (lineWidths) / sizeof (lineWidths [0]);

    int w = (width () - 2 /*margin*/) * 3 / 4;
    int h = (height () - 2 /*margin*/ - (numLineWidths - 1) /*spacing*/) * 3 / (numLineWidths * 4);

    for (int i = 0; i < numLineWidths; i++)
    {
        QImage pixmap ( (w <= 0 ? width () : w),
            (h <= 0 ? height () : h), QImage::Format_ARGB32 );
        QPainter painter(&pixmap);
        pixmap.fill (Qt::transparent);

        painter.fillRect (
            0, (pixmap.height () - lineWidths [i]) / 2,
            pixmap.width (), lineWidths [i],
            m_fgColor);

        addOption ( QPixmap::fromImage(pixmap), QString::number (lineWidths [i]) );
        startNewOptionRow ();
    }

    finishConstruction (0, 0);
}

int WidgetLineWidth::lineWidth () const
{
    return lineWidths [selectedRow ()];
}

void WidgetLineWidth::setCurrentLineWidth(int w)
{
    int numLineWidths = sizeof (lineWidths) / sizeof (lineWidths [0]);
    for (int i = 0; i < numLineWidths; i++)
        if (lineWidths[i] == w)
        {
            setSelected(i,0);
            return;
        }
}

bool WidgetLineWidth::setSelected (int row, int col)
{
    const bool ret = WidgetBase::setSelected (row, col);
    if (ret)
        emit lineWidthChanged ( lineWidth () );
    return ret;
}
