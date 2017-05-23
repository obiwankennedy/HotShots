
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

#include "RescaleDialog.h"


RescaleDialog::RescaleDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUi(this);
}

RescaleDialog::~RescaleDialog()
{

}

void RescaleDialog::setOriginalSize(const QSize &sz)
{
    m_oriSize = sz;
    labelOriSize->setText(QString("%1x%2").arg(sz.width()).arg(sz.height()));
    labelTargetSize->setText(QString("%1x%2").arg(sz.width()).arg(sz.height()));
    spinBoxScaleWidth->setValue(sz.width());
    spinBoxScaleHeight->setValue(sz.height());
}

void RescaleDialog::on_spinBoxScalePercent_valueChanged(int val)
{
    QSize newSize(m_oriSize);
    newSize*=(val/100.);
    labelTargetSize->setText(QString("%1x%2").arg(newSize.width()).arg(newSize.height()));
}

void RescaleDialog::on_spinBoxScaleWidth_valueChanged(int)
{
    Qt::AspectRatioMode ar = checkBoxKeepAspectRatio->isChecked()? Qt::KeepAspectRatio : Qt::IgnoreAspectRatio ;
    QSize newSize(m_oriSize);
    newSize.scale(QSize(spinBoxScaleWidth->value(),spinBoxScaleHeight->value()),ar);
    labelTargetSize->setText(QString("%1x%2").arg(newSize.width()).arg(newSize.height()));
}

void RescaleDialog::on_spinBoxScaleHeight_valueChanged(int)
{
    Qt::AspectRatioMode ar = checkBoxKeepAspectRatio->isChecked()? Qt::KeepAspectRatio :  Qt::IgnoreAspectRatio;
    QSize newSize(m_oriSize);
    newSize.scale(QSize(spinBoxScaleWidth->value(),spinBoxScaleHeight->value()),ar);
    labelTargetSize->setText(QString("%1x%2").arg(newSize.width()).arg(newSize.height()));
}

QSize RescaleDialog::getFinalSize() const
{
    QSize newSize(m_oriSize);

    if (radioButtonPercent->isChecked())
    {
        newSize*=(spinBoxScalePercent->value()/100.);
    }
    else
    {
        Qt::AspectRatioMode ar = checkBoxKeepAspectRatio->isChecked()? Qt::KeepAspectRatio :  Qt::IgnoreAspectRatio;
        newSize.scale(QSize(spinBoxScaleWidth->value(),spinBoxScaleHeight->value()),ar);
    }

    return newSize;
}