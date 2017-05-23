
#ifndef _BASEUPLOADER_H_
#define _BASEUPLOADER_H_

#include <QtCore/QObject>
#include <QPixmap>

class QWidget;
class QNetworkAccessManager;
class QNetworkReply;
class QProgressDialog;

class BaseUploader : public QObject
{
    Q_OBJECT

public:

    enum typFormatEncapsulation
    {
        FORMAT_NONE = 0,
        FORMAT_IM, // instant messaging
        FORMAT_HTML, // hmm, HTML ?
        FORMAT_BBCODE, //  Bulletin Board Code
        FORMAT_USER // defined by user
    };

    explicit BaseUploader(QObject *parent = 0);
    virtual ~BaseUploader();

    QWidget *getSettingsInterface() const;

    virtual bool isValid() const;

    // default parameters
    static int defaultUrlFormat;
    static QString defaultUserFormat;
    static int defaultProxyPort;

    virtual QPixmap getLogo() const;

    const QString & getName() const;
    const QString & getUploadedUrl() const;

    /*!
     * \brief   upload function
     *
     * upload the qimage on the uploading service
     *
     * \param[in] image: qimage of the screenshot
     * \param[in] name: prefer basename (without extension) of the imahe to upload
     * \return void
     */
    void upload(const QImage&image, const QString &name);

signals:

    void newUrl(const QString &);

public slots:

protected slots:

    virtual void requestFinished(QNetworkReply * reply);
    void updateDataTransferProgress(qint64 readBytes,qint64 totalBytes);
    virtual void cancelDownload();

protected:

    virtual QWidget *createSettingsInterface(const QPixmap &logo, const QString &url);
    void checkImageSizeContraint();
    void checkFileSizeConstraint();
    void initNetworkManager();

    virtual void transfert() = 0;

    QString m_name;
    QWidget *m_settingsUI;

    QImage m_image;
    QString m_basename;
    QList<QSize> m_authorizedSize;
    int m_modeAspectRatio;
    int m_sizeLimit;
    QStringList m_acceptedFormats;
    QString m_fileToUpload;
    QString m_url;

    QNetworkAccessManager * m_manager;
    QProgressDialog *m_progressDialog;
};

#endif // _BASEUPLOADER_H_
