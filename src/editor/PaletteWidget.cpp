
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

#include "PaletteWidget.h"

#include "AppSettings.h"

#include <QPainter>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QToolTip>

const int maxHistorySize = 50; // max history

void ColorHistory::addToHistory(const ColorSet &cs)
{
    // add only if not already present
    if ( !m_history.contains(cs) )
    {
        if (m_history.size() >= maxHistorySize)
            m_history.takeFirst();
        m_history << cs;
        emit updated();
    }
}

PaletteWidget::PaletteWidget(QWidget *parent)
    : QWidget(parent),
    m_scrollOffset(0),
    m_cHistory( NULL ),
    m_pressedIndex(-1),
    m_hasDragged(false)
{
    setFocusPolicy(Qt::ClickFocus);
    setColorHistory( new ColorHistory() );
    loadSettings();
}

PaletteWidget::~PaletteWidget()
{
    saveSettings();
    delete m_cHistory;
}

int PaletteWidget::maximalScrollOffset() const
{
    if (!m_cHistory)
        return 0;

    const int colorCount = m_cHistory->getHistorySize();

    const int colorSize = patchSize().width();
    return qMax(0, colorCount - width() / colorSize);
}

int PaletteWidget::currentScrollOffset() const
{
    return m_scrollOffset;
}

void PaletteWidget::scrollForward()
{
    applyScrolling(+1);
}

void PaletteWidget::scrollBackward()
{
    applyScrolling(-1);
}

void PaletteWidget::setColorHistory(ColorHistory *ch)
{
    delete m_cHistory;
    m_cHistory = ch;
    if (!m_cHistory)
        return;

    m_scrollOffset = 0;
    connect( m_cHistory,SIGNAL( updated() ),this,SLOT( update() ) );
    update();
    emit scrollOffsetChanged();
}

ColorHistory *PaletteWidget::colorHistory() const
{
    return m_cHistory;
}

void PaletteWidget::paintEvent(QPaintEvent *event)
{
    if (!m_cHistory)
    {
        QWidget::paintEvent(event);
        return;
    }

    QSize colorSize = patchSize();
    QPoint colorOffset = QPoint(colorSize.width(), 0);
    QPoint colorPos(0, 0);

    QPainter painter(this);

    const int colorCount = m_cHistory->getHistorySize();
    for (int i = m_scrollOffset; i < colorCount; ++i)
    {
        // draw color background
        painter.setBrush( m_cHistory->getColor(i).bg );
        painter.drawRect( QRect(colorPos, colorSize) );

        // draw color foreground
        painter.setPen(Qt::NoPen);
        painter.setBrush( m_cHistory->getColor(i).fg );
        painter.drawRect( QRect(colorPos, colorSize).adjusted(5,5,-5,-5) );

        // advance drawing position
        colorPos += colorOffset;

        // break when widget border is reached
        if ( colorPos.x() > width() || colorPos.y() > height() )
            break;
    }
}

void PaletteWidget::mousePressEvent(QMouseEvent *event)
{
    m_pressedIndex = indexFromPosition( event->pos() );
}

void PaletteWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (m_pressedIndex != -1)
    {
        int index = indexFromPosition( event->pos() );
        if (index != m_pressedIndex)
        {
            m_hasDragged = true;
            applyScrolling(m_pressedIndex - index);
            m_pressedIndex = indexFromPosition( event->pos() );
        }
    }
}

void PaletteWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (!m_cHistory)
        return;

    const int releasedIndex = indexFromPosition( event->pos() );
    if (!m_hasDragged && releasedIndex == m_pressedIndex && releasedIndex != -1)
    {
        emit colorSelected(m_cHistory->getColor(releasedIndex).fg,m_cHistory->getColor(releasedIndex).bg);
    }
    m_pressedIndex = -1;
    m_hasDragged = false;
}

void PaletteWidget::wheelEvent(QWheelEvent *event)
{
    applyScrolling(-event->delta() / 10);
}

void PaletteWidget::keyPressEvent(QKeyEvent *event)
{
    switch( event->key() )
    {
        case Qt::Key_Left:
            applyScrolling(-1);
            break;
        case Qt::Key_Right:
            applyScrolling(1);
            break;
        case Qt::Key_PageDown:
            applyScrolling( width() / patchSize().width() );
            break;
        case Qt::Key_PageUp:
            applyScrolling( -width() / patchSize().width() );
            break;
    }
}

bool PaletteWidget::event(QEvent *event)
{
    if (event->type() == QEvent::ToolTip)
    {
        QHelpEvent *helpEvent = static_cast<QHelpEvent *>(event);
        int index = indexFromPosition( helpEvent->pos() );
        if (index != -1)
        {
            ColorSet entry = m_cHistory->getColor(index);
            QString text;
            if ( !entry.fg.name().isEmpty() )
                text += "<center><b>" + entry.fg.name() + "/" + entry.bg.name() + "</b></center>";

            QToolTip::showText(helpEvent->globalPos(), text);
        }
        else
        {
            QToolTip::hideText();
            event->ignore();
        }

        return true;
    }
    return QWidget::event(event);
}

int PaletteWidget::indexFromPosition(const QPoint &position)
{
    if (!m_cHistory)
        return -1;

    QSize colorSize = patchSize();
    int index = position.x() / colorSize.width() + m_scrollOffset;

    if ( index < 0 || index >= m_cHistory->getHistorySize() )
        return -1;

    return index;
}

QSize PaletteWidget::patchSize() const
{
    const int patchSize = height();
    return QSize(2 * patchSize - 1, patchSize - 1);
}

void PaletteWidget::applyScrolling(int delta)
{
    int newScrollOffset = qBound( 0, m_scrollOffset + delta, maximalScrollOffset() );
    if (newScrollOffset != m_scrollOffset)
    {
        m_scrollOffset = newScrollOffset;
        update();
        emit scrollOffsetChanged();
    }
}

//-----------------------------------------------------------------------
// Save/Restore parameters functions
//-----------------------------------------------------------------------

void PaletteWidget::saveSettings()
{
    if (!m_cHistory)
        return;

    AppSettings settings;

    settings.beginGroup("Editor");
    settings.beginGroup("colorSet");

    settings.beginWriteArray( "colorPairs", m_cHistory->getHistorySize() );
    for (int i = 0; i < m_cHistory->getHistorySize(); ++i)
    {
        settings.setArrayIndex(i);
        settings.setValue( "fgColor", m_cHistory->getColor(i).fg );
        settings.setValue( "bgColor", m_cHistory->getColor(i).bg );
    }
    settings.endArray();

    settings.endGroup();
    settings.endGroup();
}

void PaletteWidget::loadSettings()
{
    if (!m_cHistory)
        return;

    AppSettings settings;

    settings.beginGroup("Editor");
    settings.beginGroup("colorSet");

    int size = settings.beginReadArray("colorPairs");
    for (int i = 0; i < size; ++i)
    {
        settings.setArrayIndex(i);
        ColorSet cs;
        cs.fg = settings.value( "fgColor",QColor(0,0,0) ).value<QColor>();
        cs.bg = settings.value( "bgColor",QColor(255,255,255) ).value<QColor>();
        m_cHistory->addToHistory(cs);
    }

    if (size == 0)
        initHistoryPalette(m_cHistory);

    settings.endArray();

    settings.endGroup();
    settings.endGroup();
}

void PaletteWidget::initHistoryPalette(ColorHistory *h)
{
    if (!h)
        return;

    QStringList colors;

    colors << "#ffffff";
    colors << "#ff0000";

    colors << "#191919";
    colors << "#DF3D82";

    colors << "#C1E1A6";
    colors << "#FF9009";

    colors << "#585858";
    colors << "#118C4E";

    colors << "#404040";
    colors << "#6DBDD6";
    colors << "#B71427";
    colors << "#FFE658";

    colors << "#E44424";
    colors << "#67BCDB";

    colors << "#A2AB58";
    colors << "#FFFFFF";

    colors << "#7D1935";
    colors << "#4A96AD";

    colors << "#F5F3EE";
    colors << "#FFFFFF";

    colors << "#2B2B2B";
    colors << "#DE1B1B";

    colors << "#F6F6F6";
    colors << "#E9E581";

    colors << "#558C89";
    colors << "#74AFAD";

    colors << "#D9853B";
    colors << "#ECECEA";

    colors << "#005A31";
    colors << "#A8CD1B";

    colors << "#CBE32D";
    colors << "#F3FAB6";

    colors << "#C63D0F";
    colors << "#3B3738";

    colors << "#FDF3E7";
    colors << "#7E8F7C";

    colors << "#191919";
    colors << "#DFE2DB";

    colors << "#FFF056";
    colors << "#FFFFFF";

    for (int i = 0; i < colors.size(); i += 2)
    {
        ColorSet cs( QColor( colors.at(i) ),QColor( colors.at(i + 1) ) );
        h->addToHistory(cs);
    }
}
