
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

#ifndef _POSTEFFECT_H_
#define _POSTEFFECT_H_

#include <QtCore/QObject>
#include <QtCore/QMap>

#include <QPainter>
#include <QPixmap>

class PostEffect
{
public:

    enum typPostEffect
    {
        EFFECT_NONE = 0,
        EFFECT_DROP_SHADOW,
        EFFECT_POLAROID_DROP_SHADOW,
        EFFECT_PROGRESSIVE_EDGE,
        EFFECT_TORN_EDGE
    };

    static QPixmap process(const QPixmap &, int effect);

private:

    static QPixmap tornEdgePixmap(const QPixmap &pm);
    static QPixmap shadoizePixmapWithBorder(const QPixmap &pm);
    static QPixmap progressivePixmap(const QPixmap &pm);
    static QPixmap addBorderToPixmap(const QPixmap &pm, const QColor &color, int size);

    static QPixmap generateFuzzyRect(const QSize& size, const QColor& color, int radius);
};

#endif // _POSTEFFECT_H_
