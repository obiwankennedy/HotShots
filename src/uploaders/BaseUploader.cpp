
#include <QtCore/QDebug>
#include <QtCore/QDir>

#include <QProgressDialog>

#include <QtNetwork/QNetworkAccessManager>

#include "BaseUploader.h"
#include "LogHandler.h"
#include "DefaultUploaderSettings.h"

// constant
int BaseUploader::defaultUrlFormat = BaseUploader::FORMAT_NONE;
QString BaseUploader::defaultUserFormat = "@url@";
int BaseUploader::defaultProxyPort = 8080;

BaseUploader::BaseUploader(QObject *parent) : QObject(parent),
    m_settingsUI(NULL),
    m_modeAspectRatio(Qt::IgnoreAspectRatio),
    m_sizeLimit(0),
    m_manager(NULL),
    m_progressDialog(NULL)
{
    m_progressDialog = new QProgressDialog();
    connect( m_progressDialog, SIGNAL( canceled() ), this, SLOT( cancelDownload() ) );
}

BaseUploader::~BaseUploader()
{
    delete m_progressDialog;
    delete m_settingsUI;
}

bool BaseUploader::isValid() const
{
    return true;
}

QPixmap BaseUploader::getLogo() const
{
    return QPixmap(":/providers/providers/unknown.png");
}

const QString & BaseUploader::getName() const
{
    return m_name;
}

QWidget * BaseUploader::getSettingsInterface() const
{
    return m_settingsUI;
}

QWidget * BaseUploader::createSettingsInterface(const QPixmap &logo, const QString &url)
{
    QWidget *w = new DefaultUploaderSettings(NULL,logo,url);
    return w;
}

void BaseUploader::upload(const QImage &image, const QString &name)
{
    m_image = image;
    m_basename = name;
    m_url = "";

    // check uploader constraints
    checkImageSizeContraint();

    checkFileSizeConstraint();

    transfert();
}

void BaseUploader::checkImageSizeContraint()
{
    // no size limit ...
    if ( m_authorizedSize.isEmpty() )
        return;

    QSize foundSize;
    QSize imgSize( m_image.size() );
    foreach (const QSize &sz, m_authorizedSize)
    {
        // find a first size larger
        if ( sz.width() > imgSize.width() && sz.height() > imgSize.height() )
        {
            foundSize = sz;
            break;
        }
    }

    if ( !foundSize.isValid() )
        foundSize = m_authorizedSize.last();
    LogHandler::getInstance()->reportDebug( tr("Image has been resized according to uploading service constraint (%1x%2)").arg( foundSize.width() ).arg( foundSize.height() ) );
    m_image = m_image.scaled(foundSize,(Qt::AspectRatioMode)m_modeAspectRatio,Qt::SmoothTransformation);
}

void BaseUploader::checkFileSizeConstraint()
{
    QString ext("PNG");

    if ( !m_acceptedFormats.isEmpty() )
        ext = m_acceptedFormats.first();

    m_fileToUpload = QDir::tempPath() + QDir::separator() + m_basename + "." + ext.toLower();
    if ( !m_image.save(m_fileToUpload) )
    {
        LogHandler::getInstance()->reportError( tr("Can't save tmp file %1").arg(m_fileToUpload) );
        return;
    }

    if (m_sizeLimit)
    {
        qint64 l = QFileInfo(m_fileToUpload).size();
        if (l > m_sizeLimit)
        {
            LogHandler::getInstance()->reportDebug( tr("reduce file size according to limit (%1/%2 bytes)").arg(l).arg(m_sizeLimit) );

            // must change the format
            // try jpg format
            if ( m_acceptedFormats.isEmpty() || m_acceptedFormats.contains("JPG") )
            {
                m_fileToUpload = QDir::tempPath() + QDir::separator() + m_basename + "." + "jpg";
                if ( !m_image.save(m_fileToUpload) )
                {
                    LogHandler::getInstance()->reportError( tr("Can't save tmp file %1").arg(m_fileToUpload) );
                    return;
                }
                l = QFileInfo(m_fileToUpload).size();
            }

            int watchdog = 0;
            while (l > m_sizeLimit && watchdog < 3)
            {
                // must resize the file
                float factorResize = m_sizeLimit / (float)l;
                m_image = m_image.scaled(m_image.size() * factorResize,(Qt::AspectRatioMode)m_modeAspectRatio,Qt::SmoothTransformation);
                m_image.save(m_fileToUpload);
                l = QFileInfo(m_fileToUpload).size();
                watchdog++;
            }
        }
    }
}

const QString & BaseUploader::getUploadedUrl() const
{
    return m_url;
}

void BaseUploader::initNetworkManager()
{
    m_manager = new QNetworkAccessManager(this);

    connect( m_manager, SIGNAL( finished(QNetworkReply *) ),
             this, SLOT( requestFinished(QNetworkReply *) ) );
}

void BaseUploader::requestFinished(QNetworkReply * reply)
{
    Q_UNUSED(reply);
}

void BaseUploader::updateDataTransferProgress(qint64 readBytes,
                                              qint64 totalBytes)
{
    if (!m_progressDialog)
        return;

    //qDebug() << "readBytes = " << readBytes << " totalBytes = " << totalBytes;

    if (readBytes != totalBytes)
        m_progressDialog->show();
    m_progressDialog->setMaximum(totalBytes);
    m_progressDialog->setValue(readBytes);
}

void BaseUploader::cancelDownload()
{
    // nothing to do here
}
