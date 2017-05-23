
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

#ifndef _WIDGET_LINE_WIDTH_H_
#define _WIDGET_LINE_WIDTH_H_

#include "WidgetBase.h"

class WidgetLineWidth : public WidgetBase
{
    Q_OBJECT

public:

    WidgetLineWidth ( QWidget *parent, const QString &name = QString() );
    virtual ~WidgetLineWidth ();

    int lineWidth () const;

public slots:

    void setCurrentLineWidth(int);

signals:

    void lineWidthChanged (int width);

protected slots:

    virtual bool setSelected (int row, int col);

private:

    void init();
};

#endif  // _WIDGET_LINE_WIDTH_H_
