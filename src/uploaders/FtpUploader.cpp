
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
#include <QtCore/QFile>
#include <QtCore/QFileInfo>

//#include <QtNetwork/QFtp>

#include <QProgressDialog>

#include "FtpUploader.h"
#include "FtpUploaderSettings.h"

#include "MiscFunctions.h"
#include "LogHandler.h"

#include "AppSettings.h"

const int defaultFtpPort = 21;

FtpUploader::FtpUploader(QObject *parent) : BaseUploader(parent),
    m_ftpPort(defaultFtpPort),
    //m_ftp(NULL),
    m_ioFile(NULL)
{
    m_name = "FTP";
    m_settingsUI = createSettingsInterface(getLogo(),"");

   /* connect( m_settings->buttonBox,SIGNAL( accepted () ),this,SLOT( saveSettings() ) );
    connect( m_settings->buttonBox,SIGNAL( rejected () ),this,SLOT( loadSettings() ) );

    // init settings
    loadSettings();*/
}

FtpUploader::~FtpUploader()
{
  /*  delete m_ftp;
    delete m_ioFile;*/
}

void FtpUploader::ftpCommandFinished(int, bool error)
{
   /* if (m_ftp->currentCommand() == QFtp::ConnectToHost)
    {
        if (error)
        {
            LogHandler::getInstance()->reportError( tr("Unable to connect to the FTP server at %1. Please check that the hostname is correct.").arg(m_ftpHostname) );
            cancelDownload();
            return;
        }
        LogHandler::getInstance()->reportInfo( tr("Logged onto %1.").arg(m_ftpHostname) );
    }
    else if (m_ftp->currentCommand() == QFtp::Put)
    {
        if (error)
        {
            LogHandler::getInstance()->reportError( tr("Canceled upload of %1").arg(m_fileToUpload) );
        }
        cancelDownload();
        LogHandler::getInstance()->reportInfo( tr("Uploaded successfully onto %1.").arg(m_ftpHostname) );

        // validate the ouput url
        m_url = QString("ftp://%1/%2/%3").arg(m_ftpHostname).arg(m_ftpDirectory).arg( QFileInfo(m_fileToUpload).fileName() );

        // take care of user URL format if needed
        if (m_ftpUserURL)
        {
            m_url = m_ftpUserURLTemplate;
            m_url.replace( "@filename@",QFileInfo(m_fileToUpload).fileName() );
        }
        emit newUrl(m_url);
        LogHandler::getInstance()->reportInfo( tr("%1 is available").arg(m_url) );
    }
    else if (m_ftp->currentCommand() == QFtp::Cd)
    {
        if (error)
        {
            LogHandler::getInstance()->reportError( tr("%1 is'nt available on server %2").arg(m_ftpDirectory).arg(m_ftpHostname) );
            cancelDownload();
            return;
        }
    }*/
}

QPixmap FtpUploader::getLogo() const
{
    return QPixmap(":/providers/providers/ftp.png");
}

QWidget * FtpUploader::createSettingsInterface(const QPixmap &logo,const QString &)
{
    Q_UNUSED(logo);
    m_settings = new FtpUploaderSettings();
    return m_settings;
}

void FtpUploader::loadSettings()
{
/*    AppSettings settings;

    settings.beginGroup("Uploaders");

    settings.beginGroup("FTP");

    m_ftpHostname = settings.value("ftpHostname","").toString();
    m_ftpPort = settings.value("ftpPort",defaultFtpPort).toInt();
    m_ftpDirectory = settings.value("ftpDirectory","").toString();
    m_ftpUsername = settings.value("ftpUsername","").toString();
    m_ftpPassword = MiscFunctions::base64Decode( settings.value("ftpPassword","").toString() );
    m_ftpShowPassword = settings.value("ftpShowPassword",false).toBool();
    m_ftpUserURL = settings.value("ftpUserURL",false).toBool();
    m_ftpUserURLTemplate = settings.value("ftpUserURLTemplate","ftp://whatyouwant/@filename@").toString();

    settings.endGroup();
    settings.endGroup();

    // set settings to interface
    m_settings->lineEditServer->setText(m_ftpHostname);
    m_settings->spinBoxPort->setValue(m_ftpPort);
    m_settings->LineEditUsername->setText(m_ftpUsername);
    m_settings->lineEditPassword->setText(m_ftpPassword);
    m_settings->lineEditDirectory->setText(m_ftpDirectory);
    m_settings->checkBoxShowPassword->setChecked(m_ftpShowPassword);
    m_settings->checkBoxUserURL->setChecked(m_ftpUserURL);
    m_settings->lineEditUserUrl->setText(m_ftpUserURLTemplate);*/
}

void FtpUploader::saveSettings()
{
    // read settings from interface
/*    m_ftpHostname = m_settings->lineEditServer->text();
    m_ftpPort = m_settings->spinBoxPort->value();
    m_ftpUsername = m_settings->LineEditUsername->text();
    m_ftpPassword = m_settings->lineEditPassword->text();
    m_ftpDirectory = m_settings->lineEditDirectory->text();
    m_ftpShowPassword = m_settings->checkBoxShowPassword->isChecked();
    m_ftpUserURL = m_settings->checkBoxUserURL->isChecked();
    m_ftpUserURLTemplate = m_settings->lineEditUserUrl->text();

    AppSettings settings;

    settings.beginGroup("Uploaders");

    settings.beginGroup("FTP");

    settings.setValue("ftpHostname", m_ftpHostname);
    settings.setValue("ftpPort", m_ftpPort);
    settings.setValue("ftpUsername", m_ftpUsername);
    settings.setValue("ftpDirectory", m_ftpDirectory);
    settings.setValue( "ftpPassword",  MiscFunctions::base64Encode(m_ftpPassword) ); // in order to store readable password
    settings.setValue("ftpShowPassword", m_ftpShowPassword);
    settings.setValue("ftpUserURL", m_ftpUserURL);
    settings.setValue("ftpUserURLTemplate", m_ftpUserURLTemplate);

    settings.endGroup();
    settings.endGroup();*/
}

bool FtpUploader::isValid() const
{
   /* if ( m_ftpHostname.isEmpty() || m_ftpUsername.isEmpty() || m_ftpPassword.isEmpty() )
        return false;*/

    return false;
}

void FtpUploader::createFtp()
{
   /* delete m_ftp;
    m_ftp = new QFtp(this);
    connect( m_ftp, SIGNAL( commandFinished(int,bool) ),this, SLOT( ftpCommandFinished(int,bool) ) );
    connect( m_ftp, SIGNAL( dataTransferProgress(qint64,qint64) ),this, SLOT( updateDataTransferProgress(qint64,qint64) ) );

    connect( m_ftp, SIGNAL( done(bool) ), SLOT( ftpDone(bool) ) );
    connect( m_ftp, SIGNAL( stateChanged(int) ), SLOT( ftpStateChanged(int) ) );*/
}

void FtpUploader::transfert()
{
    createFtp();
   /* m_ftp->connectToHost(m_ftpHostname,m_ftpPort);
    m_ftp->login(m_ftpUsername,m_ftpPassword);
    if ( !m_ftpDirectory.isEmpty() )
        m_ftp->cd(m_ftpDirectory);
    m_progressDialog->show();

    m_ioFile = new QFile(m_fileToUpload);
    m_ioFile->open(QIODevice::ReadOnly);
    m_ftp->put( m_ioFile, QFileInfo(m_fileToUpload).fileName() );*/
}

void FtpUploader::cancelDownload()
{
   /* m_ftp->abort();
    m_ftp->close();
    m_progressDialog->hide();

    if (m_ioFile)
        m_ioFile->close();

    delete m_ioFile;
    m_ioFile = NULL;*/
}

void FtpUploader::ftpDone(bool error)
{
 /*   if (error)
        LogHandler::getInstance()->reportError( tr("%1 transfert end with error!").arg(m_name) );
    else
        LogHandler::getInstance()->reportInfo( tr("%1 transfert end without error.").arg(m_name) );*/
}

void FtpUploader::ftpStateChanged(int state)
{
 /*   QString text;
    switch (state)
    {
        case QFtp::Unconnected:
            text = "QFtp::Unconnected";
            break;
        case QFtp::HostLookup:
            text = "QFtp::HostLookup";
            break;
        case QFtp::Connecting:
            text = "QFtp::Connecting";
            break;
        case QFtp::Connected:
            text = "QFtp::Connected";
            break;
        case QFtp::LoggedIn:
            text = "QFtp::LoggingIn";
            break;
        case QFtp::Closing:
            text = "QFtp::Closing";
            break;
        default:
            break;
    }
    LogHandler::getInstance()->reportDebug(text);*/
}
