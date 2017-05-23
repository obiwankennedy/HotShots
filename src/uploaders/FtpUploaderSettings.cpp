
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

 #include "FtpUploaderSettings.h"

FtpUploaderSettings::FtpUploaderSettings(QWidget *parent)
    : QWidget(parent)
{
    //ui = new Ui::FtpUploaderSettings();
    setupUi(this);
}

FtpUploaderSettings::~FtpUploaderSettings()
{
    //delete ui;
}

void FtpUploaderSettings::on_checkBoxShowPassword_stateChanged ( int state )
{
    if (state == Qt::Checked)
        lineEditPassword->setEchoMode (QLineEdit::Normal);
    else
        lineEditPassword->setEchoMode (QLineEdit::Password);
}

void FtpUploaderSettings::on_lineEditServer_textChanged ( const QString &  )
{
    updateOriginalURL();
}

void FtpUploaderSettings::on_lineEditDirectory_textChanged ( const QString &  )
{
    updateOriginalURL();
}

void FtpUploaderSettings::updateOriginalURL()
{
    QString url = QString("ftp://%1/%2/%filename%").arg( lineEditServer->text() ).arg( lineEditDirectory->text() );
    labelResultOriginalURL->setText(url);
}
