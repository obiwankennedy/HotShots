
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

#include <QBrush>
#include <QPainter>
#include <QPixmap>
#include <QColor>

#include "WidgetFillStyle.h"

//---------------------------------------------------------------------

WidgetFillStyle::WidgetFillStyle (QWidget *parent, const QString &name)
    : WidgetBase (parent, name)
{
    init();
}

//---------------------------------------------------------------------

WidgetFillStyle::~WidgetFillStyle ()
{
}

//---------------------------------------------------------------------

void WidgetFillStyle::init()
{
    reset();
    for (int i = 0; i <  FillStyleNum; i++)
    {
        QPixmap pixmap;

        pixmap = fillStylePixmap ( (FillStyle) i,
                                   (width () - 2 /*margin*/) * 3 / 4,
                                   (height () - 2 /*margin*/ - 2 /*spacing*/) * 3 / (3 * 4) );
        addOption (pixmap, fillStyleName ( (FillStyle) i ) /*tooltip*/);

        startNewOptionRow ();
    }
    finishConstruction (0, 0);
}

//---------------------------------------------------------------------

QPixmap WidgetFillStyle::fillStylePixmap (FillStyle fs, int w, int h)
{
    QPixmap pixmap ( (w <= 0 ? width () : w), (h <= 0 ? height () : h) );
    pixmap.fill( palette().color(QPalette::Window) );

    const int penWidth = 2;

    const QRect rectRect(1, 1, w - 2, h - 2);

    QPainter painter(&pixmap);
    QPen pen(m_fgColor);
    pen.setWidth(penWidth);
    painter.setPen(pen);

    switch ( fs )
    {
        case NoFill:
        {
            painter.setBrush(Qt::NoBrush);
            break;
        }
        case FillWithTranslucentBackground:
        {
            painter.setBrush( QColor(m_bgColor.red(),m_bgColor.green(),m_bgColor.blue(),100) );
            break;
        }
        case FillWithBackground:
        {
            painter.setBrush(m_bgColor);
            break;
        }
        default:
            ;
    }

    painter.drawRect(rectRect);
    painter.end();

    return pixmap;
}

//---------------------------------------------------------------------

QString WidgetFillStyle::fillStyleName (FillStyle fs) const
{
    switch (fs)
    {
        case NoFill:
            return tr ("No Fill");

            break;
        case FillWithTranslucentBackground:
            return tr ("Fill with transparent background color");

            break;
        case FillWithBackground:
            return tr ("Fill with background color");

            break;
        default:
            return QString();

            break;
    }
}

//---------------------------------------------------------------------

WidgetFillStyle::FillStyle WidgetFillStyle::fillStyle () const
{
    return (FillStyle) selectedRow ();
}

//---------------------------------------------------------------------

void WidgetFillStyle::setCurrentFillStyle(int fill)
{
    setSelected(fill,0);
}

//---------------------------------------------------------------------

QColor WidgetFillStyle::drawingBackgroundColor (
    const QColor &foregroundColor, const QColor &backgroundColor) const
{
    switch ( fillStyle () )
    {
        default:
        case NoFill:
            return QColor::Invalid;

        case FillWithTranslucentBackground:
            return backgroundColor;

        case FillWithBackground:
            return foregroundColor;
    }
}

//---------------------------------------------------------------------

bool WidgetFillStyle::setSelected (int row, int col)
{
    const bool ret = WidgetBase::setSelected (row, col);
    if (ret)
        emit fillStyleChanged ( fillStyle () );
    return ret;
}
