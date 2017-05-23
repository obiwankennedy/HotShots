
#ifndef _PAYPALBUTTON_H_
#define _PAYPALBUTTON_H_


// Based on PaypalButton from fresh library (https://github.com/pasnox/fresh)

/*!
    \file PaypalButton.h
    \brief A custom button providing paypal donation.
    \author Filipe Azevedo aka Nox P\@sNox <pasnox@gmail.com>
*/

#include <QPushButton>
#include <QtCore/QUrl>
#include <QtCore/QHash>

#define PAYPAL_DOMAIN "https://www.paypal.com"
#define PAYPAL_MASK "https://www.paypal.com/%1/i/btn/btn_donate_LG.gif"

/*!
    \ingroup FreshGui
    \class PaypalButton
    \brief A custom button providing paypal donation.

    This button allow your application to easily receives paypal donations.
    
*/
class PaypalButton : public QPushButton
{
    Q_OBJECT
    
public:
    /*!
        Create an instance of button having \a parent as parent.
    */
    PaypalButton( QWidget* parent = 0 );
    /*!
        Reimplemented.
    */
    virtual QSize minimumSizeHint() const;
    /*!
        Reimplemented.
    */
    virtual QSize sizeHint() const;

    /*!
        Return the http post action url.
    */
    QString actionPost() const;
    /*!
        Return your donation item business id.
    */
    QString businessId() const;
    /*!
        Return your donation item name.
    */
    QString itemName() const;
    /*!
        Return your donation item id.
    */
    QString itemId() const;
    /*!
        Return the currency used by your donation item.
    */
    QString currencyCode() const;
    /*!
        Return true if the http request should be automatically opened using QDesktopServices::openUrl() else false.
    */
    bool autoOpenUrl() const;
    /*!
        Return the final generated url that will be opened when clicking the button.
    */
    QUrl url() const;
    /*!
        Return the pixmap url for \a locale.
    */
    static QUrl pixmapUrl( const QString& locale );

public slots:
    /*!
        Set the http post action to \a url.
    */
    void setActionPost( const QString& value );
    /*!
        Set the donation item business id to \a value.
    */
    void setBusinessId( const QString& value );
    /*!
        Set the donation item name to \a value.
    */
    void setItemName( const QString& value );
    /*!
        Set the donation item id to \a value.
    */
    void setItemId( const QString& value );
    /*!
        Set the donation item currency code to \a value..
    */
    void setCurrencyCode( const QString& value );
    /*!
        Set if the button should automatically open url() when the user click on the button.
        Give true to automatically open it, else give false and handle the clicked( const QUrl& ) signal.
    */
    void setAutoOpenUrl( bool open );

signals:

    /*!
        This signal is emit when the user click the button and that autoOpenUrl() is false.
    */
    void clicked( const QUrl& url );

protected slots:

    void _q_clicked();

protected:

    /*!
        Reimplemented.
    */
    virtual void paintEvent( QPaintEvent* event );
   
    void localeChanged();

    QHash<QString, QString> m_QueryItems;
    QPixmap m_pixmap;
    bool m_AutoOpenUrl;


};

#endif // _PAYPALBUTTON_H_
