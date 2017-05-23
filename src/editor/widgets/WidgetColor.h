
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

#ifndef _WIDGET_COLOR_H_
#define _WIDGET_COLOR_H_

#include <QPixmap>
#include <QColor>

#include "WidgetBase.h"

class WidgetColor : public WidgetBase
{
    Q_OBJECT

public:

    WidgetColor ( QWidget *parent, const QString &name = QString() );
    virtual ~WidgetColor ();

public:

    QColor color () const;

    struct DrawPackage
    {
        int row;
        int col;
        QColor color;
    };

    static DrawPackage drawFunctionDataForRowCol (const QColor &color,
                                                  int row, int col);
    DrawPackage drawFunctionData (const QColor &color) const;
    void setCurrentColor(const QColor &);

signals:

    void colorChanged (const QColor &);

protected slots:

    virtual bool setSelected (int row, int col);

private:

    QString colorName (int color) const;
};

#endif  // _WIDGET_COLOR_H_
