
#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtCore/QUrl>
#include <QtCore/QFileInfo>

#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkAccessManager>

#include "QSgml.h"

#include "CanardPCUploader.h"
#include "LogHandler.h"
#include "MiscFunctions.h"

const QString UploadUrl = "http://tof.canardpc.com/";
const QString ServiceUrl = "http://tof.canardpc.com/";

CanardPCUploader::CanardPCUploader(QObject *parent) : BaseUploader(parent),
    m_readUrlAnswer(false)
{
    m_name = "CanardPC";
    m_settingsUI = createSettingsInterface(getLogo(),ServiceUrl);

    // limitations
    m_sizeLimit = 4 * 1000 * 1024; // see in http://tof.canardpc.com/
    m_acceptedFormats << "PNG" << "JPG" << "GIF";

    initNetworkManager();
}

CanardPCUploader::~CanardPCUploader()
{
}

QPixmap CanardPCUploader::getLogo() const
{
    return QPixmap(":/providers/providers/canardpc.png");
}

void CanardPCUploader::transfert()
{
    // Read the file
    QFile file(m_fileToUpload);
    file.open(QIODevice::ReadOnly);
    QByteArray fileData = file.readAll();
    file.close();

    QString boundary = "---------------------------18284119267027";

    QString suffix( QFileInfo(m_fileToUpload).suffix().toLower() );
    if (suffix == "jpg")
        suffix = "jpeg";

    QByteArray requestBody;

    requestBody += QString("--" + boundary + "\r\n").toLatin1();
    requestBody += QString("Content-Disposition: form-data; name=\"fichier\"; filename=\"%1\"\r\n").arg( QFileInfo(m_fileToUpload).fileName() ).toLatin1();

    requestBody += QString("Content-Type: image/%1\r\n\r\n").arg(suffix).toLatin1();
    requestBody += fileData;
    requestBody += "\r\n";
    requestBody += QString("--" + boundary + "\r\n").toLatin1();
    requestBody += QString("Content-Disposition: form-data; name=\"envoyer\"\r\n\r\n");
    requestBody += "\r\n";
    requestBody += QString("--" + boundary + "\r\n").toLatin1();
    requestBody += QString("Content-Disposition: form-data; name=\"email\"\r\n\r\n");
    requestBody += "\r\n";
    requestBody += QString("--" + boundary + "--\r\n").toLatin1();

    QNetworkRequest request = QNetworkRequest( QUrl(UploadUrl) );
    request.setUrl( QUrl(UploadUrl) );
    request.setHeader(QNetworkRequest::ContentTypeHeader, "multipart/form-data; boundary=" + boundary );
    request.setHeader( QNetworkRequest::ContentLengthHeader, QString::number( requestBody.length() ).toLatin1() );

    m_readUrlAnswer = false;

    // Start network access
    QNetworkReply * reply = m_manager->post(
        request,
        requestBody);

    connect( reply, SIGNAL( uploadProgress(qint64, qint64) ),
             this, SLOT( updateDataTransferProgress(qint64, qint64) ) );
}

void CanardPCUploader::requestFinished(QNetworkReply * reply)
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

void CanardPCUploader::parseReply(const QByteArray &text)
{
//      QFile file(QString("d:/result%1.txt").arg(m_readUrlAnswer));
//      file.open(QIODevice::WriteOnly);
//      file.write(text);
//      file.close();

    // read the real answer file
    if (m_readUrlAnswer)
    {
        QSgml parser;
        parser.String2Sgml(text);

        for(QSgml::iterator i = parser.begin(); i!=parser.end(); ++i )
            if (i->Name == "input")
            {
                QRegExp rx("\\[url=(.+)\\]");
                rx.setMinimal(true);

                if (rx.indexIn( (i->Attributes["value"]) )!= -1)
                {
                    m_url = rx.cap(1);
                    emit newUrl(m_url);
                    break;
                }
            }
        return;
    }

//     QByteArray text;
//     QFile file(QString("d:/result0.txt"));
//     if (file.open(QIODevice::ReadOnly))
//         text = file.readAll();
//     file.close();

    // Result to parse
    // <META HTTP-EQUIV="Refresh" CONTENT="0; URL=http://tof.canardpc.com/show/67f07fc4-4f92-40f3-9a89-4fde270d99bd.html">

    QSgml parser;
    parser.String2Sgml(text);

    // first: extract the answer URL
    QString answerURL;
    for(QSgml::iterator i = parser.begin(); i!=parser.end(); ++i )
        if (i->Name == "meta" && i->Attributes["http-equiv"] == "Refresh")
        {
            answerURL = i->Attributes["content"].split("=")[1];
            break;
        }

    // read the result URL for image path extraction
    if ( !answerURL.isEmpty() )
    {
        m_readUrlAnswer = true;
        m_manager->get( QNetworkRequest( QUrl(answerURL) ) );
    }
    else
        LogHandler::getInstance()->reportError( tr("Can't extract URL from reply") );
}
