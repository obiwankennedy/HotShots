
#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtCore/QUrl>
#include <QtCore/QFileInfo>

#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkAccessManager>

#include "QSgml.h"

#include "FreeImageHostingUploader.h"
#include "LogHandler.h"
#include "MiscFunctions.h"

const QString UploadUrl = "http://www.freeimagehosting.net/upload.php";
const QString ServiceUrl = "http://www.freeimagehosting.net/";

FreeImageHostingUploader::FreeImageHostingUploader(QObject *parent) : BaseUploader(parent)
{
    m_name = "FreeImageHosting.net";
    m_settingsUI = createSettingsInterface(getLogo(),ServiceUrl);

    // limitations
    m_sizeLimit = 3 * 1000 * 1024; // see in http://www.freeimagehosting.net/
    m_acceptedFormats << "PNG" << "JPG" << "GIF" << "BMP";

    initNetworkManager();
}

FreeImageHostingUploader::~FreeImageHostingUploader()
{
}

QPixmap FreeImageHostingUploader::getLogo() const
{
    return QPixmap(":/providers/providers/freeimagehosting.png");
}

void FreeImageHostingUploader::transfert()
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
    requestBody += QString("Content-Disposition: form-data; name=\"attached\"; filename=\"%1\"\r\n").arg( QFileInfo(m_fileToUpload).fileName() ).toLatin1();

    requestBody += QString("Content-Type: image/%1\r\n\r\n").arg(suffix).toLatin1();
    requestBody += fileData;
    requestBody += "\r\n";
    requestBody += QString("--" + boundary + "\r\n").toLatin1();

    QNetworkRequest request = QNetworkRequest( QUrl(UploadUrl) );
    request.setUrl( QUrl(UploadUrl) );
    request.setHeader(QNetworkRequest::ContentTypeHeader, "multipart/form-data; boundary=" + boundary );
    request.setHeader( QNetworkRequest::ContentLengthHeader, QString::number( requestBody.length() ).toLatin1() );

    // Start network access
    QNetworkReply * reply = m_manager->post(
        request,
        requestBody);

    connect( reply, SIGNAL( uploadProgress(qint64, qint64) ),
             this, SLOT( updateDataTransferProgress(qint64, qint64) ) );
}

void FreeImageHostingUploader::requestFinished(QNetworkReply * reply)
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

void FreeImageHostingUploader::parseReply(const QByteArray &text)
{
/*
     QFile file("d:/result.txt");
     file.open(QIODevice::WriteOnly);
     file.write(text);
     file.close();
 */

    /* Result to parse
       <tr><td>
       <b>&nbsp; Your Image Link</b><br><input name="link" class="linkcode" spellcheck="false" onclick="s(this);" size="50" type="text" value="http://www.freeimagehosting.net/yipwu">
       </td></tr>
       <tr><td>
       <b>&nbsp; HTML Thumbnail Link for Websites & Blogs<!--HTML code for a Thumbnail Link to the Fullsize Image:--></b><br><input name="htmlthumb" class="othercode" spellcheck="false" onclick="s(this);" size="50" type="text" value="&lt;a href=&quot;http://www.freeimagehosting.net/yipwu&quot;&gt;&lt;img src=&quot;http://www.freeimagehosting.net/t/yipwu.jpg&quot;&gt;&lt;/a&gt;">
       </td></tr>
       <tr><td>
       <b>&nbsp; HTML Fullsize Link for Websites & Blogs<!--HTML code for the Fullsize Image:--></b><br><input name="htmlfull" class="othercode" spellcheck="false" onclick="s(this);" size="50" type="text" value="&lt;a href=&quot;http://www.freeimagehosting.net/yipwu&quot;&gt;&lt;img src=&quot;http://www.freeimagehosting.net/newuploads/yipwu.png&quot;&gt;&lt;/a&gt;">
       </td></tr>
       <tr><td>
       <b>&nbsp; Forum & Message Board Link <!--BB code for a Forum Post Thumbnail Link:--></b><br><input name="forumlink" class="othercode" spellcheck="false" onclick="s(this);" size="50" type="text" value="[url=http://www.freeimagehosting.net/yipwu][img]http://www.freeimagehosting.net/t/yipwu.jpg[/img][/url]">
       </td></tr>
     */

    QSgml parser;
    parser.String2Sgml(text);

    for(QSgml::iterator i = parser.begin(); i!=parser.end(); ++i )
        if (i->Name == "input" && i->Attributes["name"] == "htmlfull")
        {
            QRegExp rx("src=\"(.+)\"");

            //qDebug() << MiscFunctions::HTMLToText(i->Attributes["value"] );
            if (rx.indexIn( MiscFunctions::HTMLToText(i->Attributes["value"]) )!= -1)
            {
                m_url = rx.cap(1);
                emit newUrl(m_url);
                break;
            }
        }
}
