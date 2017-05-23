
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

#ifndef _WIDGET_FILL_STYLE_H_
#define _WIDGET_FILL_STYLE_H_

#include "WidgetBase.h"

class QBrush;
class QPixmap;

class QColor;

class WidgetFillStyle : public WidgetBase
{
    Q_OBJECT

public:

    WidgetFillStyle ( QWidget *parent, const QString &name = QString() );
    virtual ~WidgetFillStyle ();

    enum FillStyle
    {
        NoFill,
        FillWithTranslucentBackground,
        FillWithBackground,
        FillStyleNum  /* not (a valid FillStyle) */
    };

private:

    QPixmap fillStylePixmap (FillStyle fs, int width, int height);
    QString fillStyleName (FillStyle fs) const;
    void init();

public:

    FillStyle fillStyle () const;
    void setCurrentFillStyle(int);

    QColor drawingBackgroundColor (
        const QColor &foregroundColor, const QColor &backgroundColor) const;

signals:

    void fillStyleChanged (int fillStyle);

protected slots:

    virtual bool setSelected (int row, int col );
};

#endif  // _WIDGET_FILL_STYLE_H_
