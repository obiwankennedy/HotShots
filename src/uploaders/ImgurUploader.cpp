
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
#include <QtCore/QUrl>

#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkAccessManager>

#include "ImgurUploader.h"
#include "LogHandler.h"

#include "json.h"

const QString ClientID = "aa60e0177b2dd0f";
const QString UploadUrl = "https://api.imgur.com/3/image";
const QString ServiceUrl = "http://www.imgur.com";

ImgurUploader::ImgurUploader(QObject *parent) : BaseUploader(parent)
{
    m_name = "Imgur.com";
    m_settingsUI = createSettingsInterface(getLogo(),ServiceUrl);

    // limitations
    m_sizeLimit = 1 * 1024 * 1024; // see in http://imgur.com/help/uploading
    m_acceptedFormats << "PNG" << "JPG" << "GIF" << "BMP";

    initNetworkManager();
}

ImgurUploader::~ImgurUploader()
{
}

QPixmap ImgurUploader::getLogo() const
{
    return QPixmap(":/providers/providers/imgur.png");
}

void ImgurUploader::transfert()
{
    // Read the file as a base-64 encoded byte array
    QFile file(m_fileToUpload);
    file.open(QIODevice::ReadOnly);
    QByteArray fileData = file.readAll().toBase64();
    file.close();

    // Create the request body
    QByteArray requestBody;

    requestBody.append( QString("image=").toUtf8() );
    requestBody.append( QUrl::toPercentEncoding(fileData) );

    QNetworkRequest request;
    request.setUrl( QUrl(UploadUrl) );
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    request.setRawHeader( "Authorization", "Client-ID " + QByteArray( ClientID.toLatin1() ) );

    // Start network access
    QNetworkReply * reply = m_manager->post(
        request,
        requestBody);

    connect( reply, SIGNAL( uploadProgress(qint64, qint64) ),
             this, SLOT( updateDataTransferProgress(qint64, qint64) ) );
}

void ImgurUploader::requestFinished(QNetworkReply * reply)
{
    if (reply->error() == QNetworkReply::NoError)
    {
        parseReply( reply->readAll() );
        LogHandler::getInstance()->reportInfo( tr("%1 transfert end without error.").arg(m_name) );
    }
    else
    {
        LogHandler::getInstance()->reportError( tr("%1 transfert end with error! (%2)").arg(m_name).arg( reply->errorString() ) );
    }

    reply->deleteLater();
}

void ImgurUploader::parseReply(const QByteArray &text)
{
//     QFile file("d:/result.txt");
//     file.open(QIODevice::WriteOnly);
//     file.write(text);
//     file.close();

    bool ok;

    //json is a QString containing the JSON data
    QVariantMap result = QtJson::parse(text, ok).toMap();

    if(!ok)
    {
        LogHandler::getInstance()->reportError( tr("An error occurred during parsing of service response") );
        return;
    }

    m_url = result["data"].toMap()["link"].toString();
    emit newUrl(m_url);
}
