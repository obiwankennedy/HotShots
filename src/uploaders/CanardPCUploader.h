
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

#ifndef _CANARDPCUPLOADER_H_
#define _CANARDPCUPLOADER_H_

#include "BaseUploader.h"

class CanardPCUploader : public BaseUploader
{
    Q_OBJECT

public:

    explicit CanardPCUploader(QObject *parent = 0);
    virtual ~CanardPCUploader();

    virtual QPixmap getLogo() const;

protected slots:

    virtual void requestFinished(QNetworkReply * reply);

protected:

    virtual void transfert();

private:

    void parseReply(const QByteArray &text);
    bool m_readUrlAnswer;
};

#endif // _CANARDPCUPLOADER_H_
