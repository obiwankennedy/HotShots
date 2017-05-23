
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

// Based on pPaypalButton from fresh library (https://github.com/pasnox/fresh)


#include <QtCore/QEvent>
#include <QDesktopServices>
#include <QtCore/QLocale>
#include <QStyleOptionButton>
#include <QPainter>
#if QT_VERSION >= 0x050000
#include <QtCore/QUrlQuery>
#endif

#include <QtCore/QDebug>

#include "PaypalButton.h"


PaypalButton::PaypalButton( QWidget* parent )
    : QPushButton( parent ),
    m_pixmap(QPixmap(":/hotshots/icon_paypal.png"))
{
    setCursor( Qt::PointingHandCursor );
    setSizePolicy( QSizePolicy( QSizePolicy::Maximum, QSizePolicy::Maximum ) );

    m_QueryItems[ "path" ] = QString( "%1/cgi-bin/webscr" ).arg( PAYPAL_DOMAIN );
    m_QueryItems[ "cmd" ] = "_donations";
    m_QueryItems[ "bn" ] = QUrl::fromPercentEncoding( "PP%2dDonationsBF%3abtn_donate_SM%2egif%3aNonHosted" );
    m_AutoOpenUrl = true;

    localeChanged();

    connect( this, SIGNAL( clicked() ), this, SLOT( _q_clicked() ) );
}

QSize PaypalButton::minimumSizeHint() const
{
    return sizeHint();
}

QSize PaypalButton::sizeHint() const
{
    return m_pixmap.isNull() ? QPushButton::sizeHint() : m_pixmap.size() +QSize( 1, 1 );
}

void PaypalButton::paintEvent( QPaintEvent* event )
{
    if ( m_pixmap.isNull() ) {
        QPushButton::paintEvent( event );
    }
    else {
        QPainter painter( this );

        QStyleOptionButton option;
        initStyleOption( &option );
        option.rect = option.rect.adjusted( 0, 0, -1, -1 );
        option.iconSize = size();
        option.icon = m_pixmap;
        option.text.clear();

        style()->drawControl( QStyle::CE_PushButtonLabel, &option, &painter, this );
    }
}

QUrl PaypalButton::pixmapUrl( const QString& locale )
{
    return QUrl( QString( PAYPAL_MASK ).arg( locale ) );
}

QString PaypalButton::actionPost() const
{
    return m_QueryItems.value( "path" );
}

void PaypalButton::setActionPost( const QString& value )
{
    m_QueryItems[ "path" ] = value;
}

QString PaypalButton::businessId() const
{
    return m_QueryItems.value( "business" );
}

void PaypalButton::setBusinessId( const QString& value )
{
    m_QueryItems[ "business" ] = value;
}

QString PaypalButton::itemName() const
{
    return m_QueryItems.value( "item_name" );
}

void PaypalButton::setItemName( const QString& value )
{
    m_QueryItems[ "item_name" ] = value;
}

QString PaypalButton::itemId() const
{
    return m_QueryItems.value( "item_number" );
}

void PaypalButton::setItemId( const QString& value )
{
    m_QueryItems[ "item_number" ] = value;
}

QString PaypalButton::currencyCode() const
{
    return m_QueryItems.value( "currency_code" );
}

void PaypalButton::setCurrencyCode( const QString& value )
{
    m_QueryItems[ "currency_code" ] = value;
}

bool PaypalButton::autoOpenUrl() const
{
    return m_AutoOpenUrl;
}

void PaypalButton::setAutoOpenUrl( bool open )
{
    m_AutoOpenUrl = open;
}

QUrl PaypalButton::url() const
{
    QUrl url( m_QueryItems.value( "path" ) );
    QList<QPair<QString, QString> > queryItems;

    foreach ( const QString& key, m_QueryItems.keys() ) {
        if ( key == "path" ) {
            continue;
        }

        queryItems << qMakePair( key, m_QueryItems[ key ] );
    }

#if QT_VERSION < 0x050000
    url.setQueryItems( queryItems );
#else
    QUrlQuery query;
    query.setQueryItems( queryItems );
    url.setQuery( query );
#endif

    return url;
}

void PaypalButton::localeChanged()
{
    m_QueryItems[ "lc" ] = locale().name().section( "_", 1 );

    setText( tr( "Donation" ) );
    setToolTip( tr( "Make a donation via Paypal" ) );
}

void PaypalButton::_q_clicked()
{
    if ( m_AutoOpenUrl ) {
        QDesktopServices::openUrl( url() );
    }
    else {
        emit clicked( url() );
    }
}



