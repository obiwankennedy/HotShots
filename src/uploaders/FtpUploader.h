
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

#ifndef _FTPUPLOADER_H_
#define _FTPUPLOADER_H_

#include "BaseUploader.h"

class FtpUploaderSettings;
//class QFtp;
class QFile;

class FtpUploader : public BaseUploader
{
    Q_OBJECT

public:

    explicit FtpUploader(QObject *parent = 0);
    virtual ~FtpUploader();

    virtual QPixmap getLogo() const;
    virtual bool isValid() const;

signals:

protected:

    virtual QWidget * createSettingsInterface(const QPixmap &logo, const QString &);

    virtual void transfert();

private slots:

    void saveSettings();
    void loadSettings();
    void ftpCommandFinished(int, bool error);
    virtual void cancelDownload();
    void ftpStateChanged(int state);
    void ftpDone(bool error);

private:

    void createFtp();
    FtpUploaderSettings *m_settings;

    QString m_ftpHostname;
    int m_ftpPort;
    QString m_ftpUsername;
    QString m_ftpPassword;
    QString m_ftpDirectory;
    bool m_ftpShowPassword;
    bool m_ftpUserURL;
    QString m_ftpUserURLTemplate;

//    QFtp *m_ftp;
    QFile *m_ioFile;
};

#endif // _FTPUPLOADER_H_
