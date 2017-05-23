
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
#include <QtCore/QDebug>

#include <QColor>

#include "WidgetDashStyle.h"

static int dashStyles [] = {1, 2, 3, 4, 5}; // Qt::SolidLine, Qt::DashLine, Qt::DotLine, Qt::DashDotLine, Qt::DashDotDotLine

WidgetDashStyle::WidgetDashStyle (QWidget *parent, const QString &name)
    : WidgetBase (parent, name)
{
    init();
}

WidgetDashStyle::~WidgetDashStyle ()
{
}

void WidgetDashStyle::init()
{
    reset();

    int numDashStyles = sizeof (dashStyles) / sizeof (dashStyles [0]);

    int w = (width () - 2 /*margin*/) * 3 / 4;
    int h = (height () - 2 /*margin*/ - (numDashStyles - 1) /*spacing*/) * 3 / (numDashStyles * 4);

    for (int i = NoDash; i < DashStyleNum; i++)
    {
        QImage pixmap ( (w <= 0 ? width () : w),
            (h <= 0 ? height () : h), QImage::Format_ARGB32 );
        QPainter painter(&pixmap);
        pixmap.fill (Qt::transparent);

        QPen pen(m_fgColor);
        int lineWidth = 3;
        pen.setWidth(lineWidth);
        pen.setStyle( (Qt::PenStyle)i );

        painter.setPen(pen);

        painter.drawLine (
            0, (pixmap.height () - lineWidth) / 2,
            pixmap.width (), (pixmap.height () - lineWidth) / 2
            );

        addOption ( QPixmap::fromImage(pixmap),fillStyleName( (DashStyle)i ) );
        startNewOptionRow ();
    }

    finishConstruction (0, 0);
}

int WidgetDashStyle::lineDashStyle () const
{
    return dashStyles [selectedRow ()];
}

void WidgetDashStyle::setCurrentDashStyle(int dash)
{
    setSelected(dash - 1,0);
}

// virtual protected slot [base WidgetBase]
bool WidgetDashStyle::setSelected (int row, int col)
{
    const bool ret = WidgetBase::setSelected (row, col);
    if (ret)
        emit lineDashStyleChanged ( lineDashStyle () );
    return ret;
}

QString WidgetDashStyle::fillStyleName (DashStyle fs) const
{
    // do not complain about the "useless" breaks
    // as the return statements might not be return statements one day

    switch (fs)
    {
        case NoDash:
            return tr ("Solid Line");

            break;
        case DashedLine:
            return tr ("Dash pattern");

            break;
        case DottedLine:
            return tr ("Dot pattern");

            break;
        case DashDot:
            return tr ("Dash Dot pattern");

            break;
        case DashDotDot:
            return tr ("Dash Dot Dot pattern");

            break;
        default:
            return QString();

            break;
    }
}
