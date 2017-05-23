
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

#include <QPainterPath>

#include "PostEffect.h"
#include "MiscFunctions.h"

const int shadowBorderSize = 10;
const int defaultBorderSize = 10;
const int shadowStrength = 127;

const int progressiveBorderSize = 10;

const int defaultToothHeight = 12; // How large (height) is each tooth
const int defaultHorizontalToothRange = 20; // How wide is a horizontal tooth
const int defaultVerticalToothRange = 20; // How wide is a vertical tooth

const QColor defaultBorderColor(255,255,255);

const int fsize = 4;
static double filter[9][9] = { {1, 1, 1, 1, 1, 1, 1, 1, 1},
                               {1, 1, 1, 1, 1, 1, 1, 1, 1},
                               {1, 1, 1, 1, 1, 1, 1, 1, 1},
                               {1, 1, 1, 1, 1, 1, 1, 1, 1},
                               {1, 1, 1, 1, 1, 1, 1, 1, 1},
                               {1, 1, 1, 1, 1, 1, 1, 1, 1},
                               {1, 1, 1, 1, 1, 1, 1, 1, 1},
                               {1, 1, 1, 1, 1, 1, 1, 1, 1},
                               {1, 1, 1, 1, 1, 1, 1, 1, 1}};

// draw shadow on translucent image
//  function from wysota (http://www.qtcentre.org/threads/6673-How-to-draw-soft-shadow-with-QPainter)

static QPixmap drawShadowedPixmap(const QPixmap &pix, int dx = 0, int dy = 0, bool onlyshdw = false)
{
    QPixmap res(pix.width() + 2 * fsize, pix.height() + 2 * fsize);
    res.fill(Qt::transparent);
    QPainter p(&res);

    QImage msk = pix.toImage();
    QImage shadow(msk.width() + 2 * fsize, msk.height() + 2 * fsize, QImage::Format_ARGB32);
    double divisor = 0.0;
    for(int i = 0; i<2 * fsize + 1; i++)
        for(int j = 0; j<2 * fsize + 1; j++)
            divisor += filter[i][j];

    for(int y = 0; y<shadow.height(); y++)
        for(int x = 0; x<shadow.width(); x++)
        {
            int l = 0;
            for(int sy = -fsize; sy<=fsize; sy++)
                for(int sx = -fsize; sx<=fsize; sx++)
                {
                    int tx = x + sx - fsize;
                    int ty = y + sy - fsize;
                    if( tx<0 || ty<0 || tx>=msk.width() || ty>=msk.height() )
                        l += (int)(filter[sx + fsize][sy + fsize] * 255);
                    else
                        l += (int)( filter[sx + fsize][sy + fsize] * ( 255 - qAlpha( msk.pixel(tx, ty) ) ) );
                }
            l = (int)(l / divisor);
            l = 255 - l;
            l = qBound(0, l, shadowStrength);

            shadow.setPixel( x, y,qRgba(0, 0, 0, l) );
        }

    p.drawImage(dx, dy, shadow);
    if(!onlyshdw)
        p.drawPixmap(dx, dy, pix);
    return res;
}

/*
   This function is based on  the Ofi Labs X2 project.
   Copyright (C) 2011 Ariya Hidayat <ariya.hidayat@gmail.com>
 */

QImage createRectGradient(int width, int height, const QGradient &gradient)
{
    QImage::Format format = QImage::Format_ARGB32_Premultiplied;
    QImage buffer(width, height, format);
    buffer.fill( qRgba(255, 255,255, 255) );

    QPainter painter(&buffer);
    painter.setPen(Qt::NoPen);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setCompositionMode(QPainter::CompositionMode_DestinationAtop);

    // Divide the rectangle into four triangles, each facing North, South,
    // East, and West. Every triangle will have its bounding box and linear
    // gradient.
    QLinearGradient linearGradient;

    // from center going North
    linearGradient = QLinearGradient(0, 0, width, height / 2);
    linearGradient.setStops( gradient.stops() );
    linearGradient.setStart(width / 2, height / 2);
    linearGradient.setFinalStop(width / 2, 0);
    painter.setBrush(linearGradient);
    painter.drawRect(0, 0, width, height / 2);

    // from center going South
    linearGradient = QLinearGradient(0, 0, width, height / 2);
    linearGradient.setStops( gradient.stops() );
    linearGradient.setStart(width / 2, height / 2);
    linearGradient.setFinalStop(width / 2, height);
    painter.setBrush(linearGradient);
    painter.drawRect(0, height / 2, width, height / 2);

    // clip the East and West portion
    QPainterPath clip;
    clip.moveTo(width, 0);
    clip.lineTo(width, height);
    clip.lineTo(0, 0);
    clip.lineTo(0, height);
    clip.closeSubpath();
    painter.setClipPath(clip);

    // from center going East
    linearGradient = QLinearGradient(0, 0, width / 2, height);
    linearGradient.setStops( gradient.stops() );
    linearGradient.setStart(width / 2, height / 2);
    linearGradient.setFinalStop(width, height / 2);
    painter.setBrush(linearGradient);
    painter.drawRect(width / 2, 0, width, height);

    // from center going West
    linearGradient = QLinearGradient(0, 0, width / 2, height);
    linearGradient.setStops( gradient.stops() );
    linearGradient.setStart(width / 2, height / 2);
    linearGradient.setFinalStop(0, height / 2);
    painter.setBrush(linearGradient);
    painter.drawRect(0, 0, width / 2, height);

    painter.end();
    return buffer;
}

QPixmap PostEffect::process(const QPixmap &in, int effect)
{
    QPixmap pix;
    switch (effect)
    {
        case EFFECT_DROP_SHADOW:
        {
            //pix = shadoizePixmap(in);
            pix = drawShadowedPixmap(in);
            break;
        }
        case EFFECT_POLAROID_DROP_SHADOW:
        {
            //pix = shadoizePixmapWithBorder(in);
            pix = drawShadowedPixmap( addBorderToPixmap(in,defaultBorderColor,defaultBorderSize) );
            break;
        }
        case EFFECT_PROGRESSIVE_EDGE:
        {
            pix = progressivePixmap(in);
            break;
        }
        case EFFECT_TORN_EDGE:
        {
            pix = tornEdgePixmap(in);
            break;
        }
        case EFFECT_NONE:
        default:
        {
            pix = in;
            break;
        }
    }
    return pix;
}

QPixmap PostEffect::generateFuzzyRect(const QSize& size, const QColor& color, int radius)
{
    QPixmap pix(size);
    const QColor transparent(0, 0, 0, 0);
    pix.fill(transparent);

    QPainter painter(&pix);
    painter.setRenderHint(QPainter::Antialiasing, true);

    // Fill middle
    painter.fillRect(pix.rect().adjusted(radius, radius, -radius, -radius), color);

    // Corners
    QRadialGradient gradient;
    gradient.setColorAt(0, color);
    gradient.setColorAt(1, transparent);
    gradient.setRadius(radius);
    QPoint center;

    // Top Left
    center = QPoint(radius, radius);
    gradient.setCenter(center);
    gradient.setFocalPoint(center);
    painter.fillRect(0, 0, radius, radius, gradient);

    // Top right
    center = QPoint(size.width() - radius, radius);
    gradient.setCenter(center);
    gradient.setFocalPoint(center);
    painter.fillRect(center.x(), 0, radius, radius, gradient);

    // Bottom left
    center = QPoint(radius, size.height() - radius);
    gradient.setCenter(center);
    gradient.setFocalPoint(center);
    painter.fillRect(0, center.y(), radius, radius, gradient);

    // Bottom right
    center = QPoint(size.width() - radius, size.height() - radius);
    gradient.setCenter(center);
    gradient.setFocalPoint(center);
    painter.fillRect(center.x(), center.y(), radius, radius, gradient);

    // Borders
    QLinearGradient linearGradient;
    linearGradient.setColorAt(0, color);
    linearGradient.setColorAt(1, transparent);

    // Top
    linearGradient.setStart(0, radius);
    linearGradient.setFinalStop(0, 0);
    painter.fillRect(radius, 0, size.width() - 2 * radius, radius, linearGradient);

    // Bottom
    linearGradient.setStart(0, size.height() - radius);
    linearGradient.setFinalStop( 0, size.height() );
    painter.fillRect(radius, int( linearGradient.start().y() ), size.width() - 2 * radius, radius, linearGradient);

    // Left
    linearGradient.setStart(radius, 0);
    linearGradient.setFinalStop(0, 0);
    painter.fillRect(0, radius, radius, size.height() - 2 * radius, linearGradient);

    // Right
    linearGradient.setStart(size.width() - radius, 0);
    linearGradient.setFinalStop(size.width(), 0);
    painter.fillRect(int( linearGradient.start().x() ), radius, radius, size.height() - 2 * radius, linearGradient);
    return pix;
}

// algorithm from Greenshot - http://getgreenshot.org/
static QPixmap CreateTornEdge(const QPixmap& sourceImage, int toothHeight, int horizontalToothRange, int verticalToothRange)
{
    QPixmap returnImage( sourceImage.width(), sourceImage.height() );
    returnImage.fill (Qt::transparent);
    QPainterPath path;

    int horizontalRegions = (int)(sourceImage.width() / horizontalToothRange);
    int verticalRegions = (int)(sourceImage.height() / verticalToothRange);

    // Start
    QPointF previousEndingPoint = QPointF( horizontalToothRange, MiscFunctions::random(1, toothHeight) );
    QPointF newEndingPoint;

    // Top
    path.moveTo(previousEndingPoint);
    for (int i = 0; i < horizontalRegions; i++)
    {
        int x = (int)previousEndingPoint.x() + horizontalToothRange;
        int y = MiscFunctions::random(1, toothHeight);
        newEndingPoint = QPointF(x, y);

        //path.lineTo(previousEndingPoint, newEndingPoint);
        path.lineTo(newEndingPoint);
        previousEndingPoint = newEndingPoint;
    }

    // Right
    for (int i = 0; i < verticalRegions; i++)
    {
        int x = sourceImage.width() - MiscFunctions::random(1, toothHeight);
        int y = (int)previousEndingPoint.y() + verticalToothRange;
        newEndingPoint = QPointF(x, y);
        path.lineTo(newEndingPoint);
        previousEndingPoint = newEndingPoint;
    }

    //       Bottom
    for (int i = 0; i < horizontalRegions; i++)
    {
        int x = (int)previousEndingPoint.x() - horizontalToothRange;
        int y = sourceImage.height() - MiscFunctions::random(1, toothHeight);
        newEndingPoint = QPointF(x, y);
        path.lineTo(newEndingPoint);
        previousEndingPoint = newEndingPoint;
    }

    // Left
    for (int i = 0; i < verticalRegions; i++)
    {
        int x = MiscFunctions::random(1, toothHeight);
        int y = (int)previousEndingPoint.y() - verticalToothRange;
        newEndingPoint = QPointF(x, y);
        path.lineTo(newEndingPoint);
        previousEndingPoint = newEndingPoint;
    }
    path.closeSubpath();

    QPainter painter(&returnImage);
    painter.setPen(Qt::NoPen);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setClipPath(path);
    painter.drawPixmap(0,0,sourceImage);

    return drawShadowedPixmap(returnImage);
}

QPixmap PostEffect::tornEdgePixmap(const QPixmap &pm)
{
    return CreateTornEdge(pm,defaultToothHeight,defaultHorizontalToothRange,defaultVerticalToothRange);
}

QPixmap PostEffect::progressivePixmap(const QPixmap &pm)
{
    int maxSize = qMax( pm.width(),pm.height() );
    float factor = (maxSize - 2 * progressiveBorderSize) / (float)maxSize;
    QGradient gradient;
    gradient.setColorAt( 1, QColor(0,0,0,0) );
    gradient.setColorAt( factor, QColor(255,255,255,255) );
    gradient.setColorAt( 0, QColor(255,255,255,255) );
    QPixmap res = QPixmap::fromImage( createRectGradient(pm.width(),pm.height(), gradient) );

    QPainter painter(&res);
    painter.setCompositionMode(QPainter::CompositionMode_SourceAtop);
    painter.drawPixmap(0,0,pm);
    painter.end();
    return res;
}

QPixmap PostEffect::addBorderToPixmap(const QPixmap &pm, const QColor &color, int size)
{
    QPixmap newPm = QPixmap( pm.rect().adjusted(-size,-size,size,size).size() );
    newPm.fill( color );
    QPainter painter(&newPm);
    painter.drawPixmap(size,size,pm);
    return newPm;
}

QPixmap PostEffect::shadoizePixmapWithBorder(const QPixmap &pm)
{
    QPixmap icon = QPixmap( pm.width() + 2 * shadowBorderSize + 2 * defaultBorderSize,pm.height() + 2 * shadowBorderSize + 2 * defaultBorderSize);

    QPainter painter(&icon);
    icon.fill( QColor(244,244,244) );

    // fill the "photo border"
    painter.fillRect(shadowBorderSize,shadowBorderSize,pm.width() + 2 * defaultBorderSize,pm.height() + 2 * defaultBorderSize,Qt::white);

    // fill the center pixmap
    painter.drawPixmap( shadowBorderSize + defaultBorderSize,shadowBorderSize + defaultBorderSize,pm );

    painter.end();

    return icon;
}
