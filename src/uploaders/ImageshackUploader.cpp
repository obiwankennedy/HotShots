
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
#include <QtCore/QFileInfo>

#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkAccessManager>

#include "QSgml.h"

#include "ImageshackUploader.h"
#include "LogHandler.h"
#include "MiscFunctions.h"

const QString UploadUrl = "http://imageshack.us/upload_api.php";
const QString ServiceUrl = "https://www.imageshack.us";
const QString apiKey = "BLCD9XZH3b384c9e0b5530291f5ebe955819b472";

ImageshackUploader::ImageshackUploader(QObject *parent) : BaseUploader(parent)
{
    m_name = "Imageshack.us";
    m_settingsUI = createSettingsInterface(getLogo(),ServiceUrl);

    // limitations
    m_sizeLimit = 5 * 1024 * 1024; // see in http://kb.imageshack.us/is/article?22=faq#a7
    m_acceptedFormats << "PNG" << "JPG" << "GIF" << "BMP";

    initNetworkManager();
}

ImageshackUploader::~ImageshackUploader()
{
}

QPixmap ImageshackUploader::getLogo() const
{
    return QPixmap(":/providers/providers/imageshack.png");
}

void ImageshackUploader::transfert()
{
    // Read the file
    QFile file(m_fileToUpload);
    file.open(QIODevice::ReadOnly);
    QByteArray fileData = file.readAll();
    file.close();

    QString boundary = "---------------------------161761219329510";

    QString suffix( QFileInfo(m_fileToUpload).suffix().toLower() );
    if (suffix == "jpg")
        suffix = "jpeg";

    QByteArray requestBody;
    requestBody += QString("--" + boundary + "\r\n").toLatin1();
    requestBody += QString("Content-Disposition: form-data; name=\"public\"\r\n\r\nyes\r\n").toLatin1();
    requestBody += QString("--" + boundary + "\r\n").toLatin1();
    requestBody += QString("Content-Disposition: form-data; name=\"key\"\r\n\r\n%1\r\n").arg(apiKey).toLatin1();
    requestBody += QString("--" + boundary + "\r\n").toLatin1();
    requestBody += QString("Content-Disposition: form-data; name=\"fileupload\"; filename=\"%1\"\r\n").arg( QFileInfo(m_fileToUpload).fileName() ).toLatin1();
    requestBody += QString("Content-Type: image/%1\r\n\r\n").arg(suffix).toLatin1();
    requestBody += fileData;
    requestBody += "\r\n";
    requestBody += QString("--" + boundary + "\r\n").toLatin1();

    QNetworkRequest request = QNetworkRequest( QUrl(UploadUrl) );
    request.setUrl( QUrl(UploadUrl) );
    request.setHeader(QNetworkRequest::ContentTypeHeader, "multipart/form-data; boundary=" + boundary );
    request.setHeader( QNetworkRequest::ContentLengthHeader, QString::number( requestBody.length() ).toLatin1() );
    request.setRawHeader("Connection", "Keep-Alive");
    request.setRawHeader("User-Agent", "Fine User-Agent");

    // Start network access
    QNetworkReply * reply = m_manager->post(
        request,
        requestBody);

    connect( reply, SIGNAL( uploadProgress(qint64, qint64) ),
             this, SLOT( updateDataTransferProgress(qint64, qint64) ) );
}

void ImageshackUploader::requestFinished(QNetworkReply * reply)
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

void ImageshackUploader::parseReply(const QByteArray &text)
{
//      QFile file("d:/result.txt");
//      file.open(QIODevice::WriteOnly);
//      file.write(text);
//      file.close();

//       QFile file("d:/result-imageshack.htm");
//       file.open(QIODevice::ReadOnly);
//         QByteArray text = file.readAll();
//       file.close();

    QSgml parser;
    parser.String2Sgml(text);

    if ( text.contains("error id=") )
    {
        QString mess;

        // <error id="auth_error">You must provide a valid auth token or dev key. see http://code.google.com/p/imageshackapi/</error>

        for(QSgml::iterator i = parser.begin(); i!=parser.end(); ++i )
            if (i->Name == "error")
            {
                if ( !i->Children.isEmpty() )
                    mess = i->Children[0]->Value;
                break;
            }

        LogHandler::getInstance()->reportError( tr("%1 service reply with error (%2)").arg(m_name).arg(mess) );
        return;
    }

    //<links>
    //    <image_link>http://img221.imageshack.us/img221/3412/latotale.png</image_link>
    //    <image_html>&lt;a href=&quot;http://img221.imageshack.us/i/latotale.png/&quot; target=&q ...
    // ...
    for(QSgml::iterator i = parser.begin(); i!=parser.end(); ++i )
        if (i->Name == "image_link")
        {
            if ( !i->Children.isEmpty() )
            {
                m_url = i->Children[0]->Value;
                emit newUrl(m_url);
                break;
            }
        }
}
