
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

#ifndef _PALETTEBARWIDGET_H_
#define _PALETTEBARWIDGET_H_

#include <QWidget>

class QToolButton;

class PaletteWidget;

class PaletteBarWidget : public QWidget
{
    Q_OBJECT

public:

    explicit PaletteBarWidget(QWidget *parent = 0);
    virtual ~PaletteBarWidget();

public slots:

    void addToHistory(const QColor&, const QColor &);

signals:

    /// Emitted whenever a color was clicked
    void colorSelected(const QColor &fgColor,const QColor &bgColor);

private slots:

    void updateButtons();

private:

    void createLayout();

    QToolButton *m_prevButton;
    QToolButton *m_nextButton;
    PaletteWidget * m_colorBar;
};

#endif // _PALETTEBARWIDGET_H_
