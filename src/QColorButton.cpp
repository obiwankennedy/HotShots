
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
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *******************************************************************************/

#include <QLabel>
#include <QColorDialog>

#include "QColorButton.h"

#define COLOR_LABEL_SPACING 5

QColorButton::QColorButton(QWidget * parent) : QPushButton(parent)
{
    this->setText("");
    colorLabel = new QLabel(this);
    colorLabel->setText("");
    colorLabel->setStyleSheet("background: rgb(127,127,127); border: solid 1px #000000;");
    updateColorLabelSize();
    connect( this, SIGNAL( clicked() ), this, SLOT( click() ) );
}

void QColorButton::resizeEvent(QResizeEvent *)
{
    updateColorLabelSize();
}

QColorButton::~QColorButton()
{
    delete colorLabel;
}

void QColorButton::updateColorLabelSize()
{
    colorLabel->move(COLOR_LABEL_SPACING, COLOR_LABEL_SPACING);
    colorLabel->resize( this->width() - (COLOR_LABEL_SPACING * 2),this->height() - (COLOR_LABEL_SPACING * 2) );
}

void QColorButton::setColor(const QColor &color)
{
    colorLabel->setStyleSheet("background: rgb("
                              + QString::number( color.red() ) + ", "
                              + QString::number( color.green() ) + ", "
                              + QString::number( color.blue() ) + ");"
                              + "border: solid 1px #000000;");
    emit colorChanged(color);
}

QColor QColorButton::getColor()
{
    return colorLabel->palette().color(QPalette::Background);
}

void QColorButton::currentColorChanged(const QColor &color)
{
    setColor(color);
}

void QColorButton::click()
{
    QColorDialog * dialog = new QColorDialog(this);
    QColor savedColor = getColor();
    connect( dialog, SIGNAL( currentColorChanged(const QColor &) ), this, SLOT( currentColorChanged(const QColor &) ) );
    dialog->setCurrentColor( getColor() );
    if (dialog->exec() != QDialog::Accepted)
        setColor(savedColor);
    delete dialog;
}
