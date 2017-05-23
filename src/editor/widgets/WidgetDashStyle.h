
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

#ifndef _WIDGET_DASH_STYLE_H_
#define _WIDGET_DASH_STYLE_H_

#include <QPainter>

#include "WidgetBase.h"

class WidgetDashStyle : public WidgetBase
{
    Q_OBJECT

public:

    enum DashStyle
    {
        NoDash = Qt::SolidLine,
        DashedLine = Qt::DashLine,
        DottedLine = Qt::DotLine,
        DashDot = Qt::DashDotLine,
        DashDotDot = Qt::DashDotDotLine,
        DashStyleNum  /* not (a valid FillStyle) */
    };

    WidgetDashStyle ( QWidget *parent, const QString &name = QString() );
    virtual ~WidgetDashStyle ();

    int lineDashStyle () const;
    void setCurrentDashStyle(int);

signals:

    void lineDashStyleChanged (int width);

protected slots:

    virtual bool setSelected (int row, int col);

private:

    void init();
    QString fillStyleName (DashStyle fs) const;
};

#endif  // _WIDGET_DASH_TYPE_H_
