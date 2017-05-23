
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

#include <QBitmap>
#include <QColor>
#include <QtCore/QEvent>
#include <QHelpEvent>
#include <QImage>
#include <QPainter>
#include <QPixmap>
#include <QToolTip>
#include <QtCore/QtDebug>

#include "WidgetBase.h"

//---------------------------------------------------------------------

WidgetBase::WidgetBase (QWidget *parent, const QString &name)
    : QFrame (parent),
    m_baseColorDeprecated( QColor(Qt::black) ),
    m_fgColor( QColor(Qt::black) ),
    m_bgColor( QColor(Qt::black) ),
    m_baseWidget(NULL),
    m_selectedRow (-1),
    m_selectedCol (-1)
{
    setObjectName (name);
    setFrameStyle (QFrame::Panel | QFrame::Sunken);
    setFixedSize (66, 66);
    setSizePolicy (QSizePolicy::Minimum, QSizePolicy::Minimum);
}

//---------------------------------------------------------------------

WidgetBase::~WidgetBase ()
{
}

//---------------------------------------------------------------------

void WidgetBase::setBaseColorDeprecated(const QColor &c)
{
    m_baseColorDeprecated = c;
    init();
}

//---------------------------------------------------------------------

void WidgetBase::setFgColor(const QColor &c)
{
    m_fgColor = c;
    init();
}

//---------------------------------------------------------------------

void WidgetBase::setBgColor(const QColor &c)
{
    m_bgColor = c;
    init();
}

//---------------------------------------------------------------------

void WidgetBase::reset()
{
    m_pixmaps.clear();
    m_toolTips.clear();
    m_pixmapRects.clear();
}

//---------------------------------------------------------------------

void WidgetBase::addOption (const QPixmap &pixmap, const QString &toolTip)
{
    if ( m_pixmaps.isEmpty () )
        startNewOptionRow ();

    m_pixmaps.last ().append (pixmap);
    m_pixmapRects.last ().append ( QRect () );
    m_toolTips.last ().append (toolTip);
}

//---------------------------------------------------------------------

void WidgetBase::startNewOptionRow ()
{
    m_pixmaps.append ( QList <QPixmap> () );
    m_pixmapRects.append ( QList <QRect> () );
    m_toolTips.append ( QList <QString> () );
}

//---------------------------------------------------------------------

// public
void WidgetBase::finishConstruction (int fallBackRow, int fallBackCol)
{
    relayoutOptions ();

    setMaximumSize (QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);

    const QPair <int, int> rowColPair = defaultSelectedRowAndCol ();
    if ( !setSelected (rowColPair.first, rowColPair.second) )
    {
        if ( !setSelected (fallBackRow, fallBackCol) )
        {
            if ( !setSelected (0, 0) )
            {
                qDebug() << "WidgetBase::finishConstruction() "
                            "can't even fall back to setSelected(row=0,col=0)" << endl;
            }
        }
    }
}

//---------------------------------------------------------------------

QList <int> WidgetBase::spreadOutElements (const QList <int> &sizes, int max)
{
    if (sizes.count () == 0)
        return QList <int> ();
    else if (sizes.count () == 1)
    {
        QList <int> ret;
        ret.append (sizes.first () > max ? 0 : 1 /*margin*/);
        return ret;
    }

    QList <int> retOffsets;
    for (int i = 0; i < sizes.count (); i++)
        retOffsets.append (0);

    int totalSize = 0;
    for (int i = 0; i < (int) sizes.count (); i++)
        totalSize += sizes [i];

    int margin = 1;

    // if don't fit with margin, then just return elements
    // packed right next to each other
    if (totalSize + margin * 2 > max)
    {
        retOffsets [0] = 0;
        for (int i = 1; i < (int) sizes.count (); i++)
            retOffsets [i] = retOffsets [i - 1] + sizes [i - 1];

        return retOffsets;
    }

    int maxLeftOver = max - ( totalSize + margin * 2 * sizes.count() );

    int startCompensating = -1;
    int numCompensate = 0;

    int spacing = 0;

    spacing = maxLeftOver / (sizes.count () - 1);
    if (spacing * int (sizes.count () - 1) < maxLeftOver)
    {
        numCompensate = maxLeftOver - spacing * (sizes.count () - 1);
        startCompensating = ( (sizes.count () - 1) - numCompensate ) / 2;
    }

    retOffsets [0] = margin;
    for (int i = 1; i < (int) sizes.count (); i++)
        retOffsets [i] += retOffsets [i - 1] +
                          sizes [i - 1] +
                          spacing +
                          ( (numCompensate &&
                             i >= startCompensating &&
                             i < startCompensating + numCompensate) ? 1 : 0 );

    return retOffsets;
}

//---------------------------------------------------------------------

QPair <int, int> WidgetBase::defaultSelectedRowAndCol () const
{
    //int row = -1, col = -1;

    return qMakePair (m_selectedRow, m_selectedCol);
}

//---------------------------------------------------------------------

int WidgetBase::defaultSelectedRow () const
{
    return defaultSelectedRowAndCol ().first;
}

//---------------------------------------------------------------------

int WidgetBase::defaultSelectedCol () const
{
    return defaultSelectedRowAndCol ().second;
}

//---------------------------------------------------------------------

void WidgetBase::relayoutOptions ()
{
    while (!m_pixmaps.isEmpty () && m_pixmaps.last ().count () == 0)
    {
        m_pixmaps.removeLast ();
        m_pixmapRects.removeLast ();
        m_toolTips.removeLast ();
    }

    if ( m_pixmaps.isEmpty () )
        return;

    QList <int> maxHeightOfRow;
    for (int r = 0; r < m_pixmaps.count (); r++)
        maxHeightOfRow.append (0);

    for (int r = 0; r < (int) m_pixmaps.count (); r++)
        for (int c = 0; c < (int) m_pixmaps [r].count (); c++)
            if (c == 0 || m_pixmaps [r][c].height () > maxHeightOfRow [r])
                maxHeightOfRow [r] = m_pixmaps [r][c].height ();

    QList <int> rowYOffset = spreadOutElements ( maxHeightOfRow, height () );

    for (int r = 0; r < (int) m_pixmaps.count (); r++)
    {
        QList <int> widths;
        for (int c = 0; c < (int) m_pixmaps [r].count (); c++)
            widths.append ( m_pixmaps [r][c].width () );
        QList <int> colXOffset = spreadOutElements ( widths, width () );

        for (int c = 0; c < (int) colXOffset.count (); c++)
        {
            int x = colXOffset [c];
            int y = rowYOffset [r];
            int w, h;

            if (c == (int) colXOffset.count () - 1)
            {
                if ( x + m_pixmaps [r][c].width () >= width () )
                    w = m_pixmaps [r][c].width ();
                else
                    w = width () - 1 - x;
            }
            else
                w = colXOffset [c + 1] - x;

            if (r == (int) m_pixmaps.count () - 1)
            {
                if ( y + m_pixmaps [r][c].height () >= height () )
                    h = m_pixmaps [r][c].height ();
                else
                    h = height () - 1 - y;
            }
            else
                h = rowYOffset [r + 1] - y;

            m_pixmapRects [r][c] = QRect (x, y, w, h);
        }
    }

    update ();
}

//---------------------------------------------------------------------

int WidgetBase::selectedRow () const
{
    return m_selectedRow;
}

//---------------------------------------------------------------------

int WidgetBase::selectedCol () const
{
    return m_selectedCol;
}

//---------------------------------------------------------------------

int WidgetBase::selected () const
{
    if (m_selectedRow < 0 ||
        m_selectedRow >= (int) m_pixmaps.count () ||
        m_selectedCol < 0)
    {
        return -1;
    }

    int upto = 0;
    for (int y = 0; y < m_selectedRow; y++)
        upto += m_pixmaps [y].count ();

    if ( m_selectedCol >= (int) m_pixmaps [m_selectedRow].count () )
        return -1;

    upto += m_selectedCol;

    return upto;
}

//---------------------------------------------------------------------

bool WidgetBase::hasPreviousOption (int *row, int *col) const
{
    if (row)
        *row = -1;
    if (col)
        *col = -1;

    if (m_selectedRow < 0 || m_selectedCol < 0)
        return false;

    int newRow = m_selectedRow,
        newCol = m_selectedCol;

    newCol--;
    if (newCol < 0)
    {
        newRow--;
        if (newRow < 0)
            return false;

        newCol = m_pixmaps [newRow].count () - 1;
        if (newCol < 0)
            return false;
    }

    if (row)
        *row = newRow;
    if (col)
        *col = newCol;

    return true;
}

//---------------------------------------------------------------------

bool WidgetBase::hasNextOption (int *row, int *col) const
{
    if (row)
        *row = -1;
    if (col)
        *col = -1;

    if (m_selectedRow < 0 || m_selectedCol < 0)
        return false;

    int newRow = m_selectedRow,
        newCol = m_selectedCol;

    newCol++;
    if ( newCol >= (int) m_pixmaps [newRow].count () )
    {
        newRow++;
        if ( newRow >= (int) m_pixmaps.count () )
            return false;

        newCol = 0;
        if ( newCol >= (int) m_pixmaps [newRow].count () )
            return false;
    }

    if (row)
        *row = newRow;
    if (col)
        *col = newCol;

    return true;
}

//---------------------------------------------------------------------

bool WidgetBase::setSelected (int row, int col)
{
    if ( row < 0 || col < 0 ||
         row >= (int) m_pixmapRects.count () || col >= (int) m_pixmapRects [row].count () )
    {
        return false;
    }

    if (row == m_selectedRow && col == m_selectedCol)
    {
        return true;
    }

    const int wasSelectedRow = m_selectedRow;
    const int wasSelectedCol = m_selectedCol;

    m_selectedRow = row, m_selectedCol = col;

    if (wasSelectedRow >= 0 && wasSelectedCol >= 0)
    {
        // unhighlight old option
        update (m_pixmapRects [wasSelectedRow][wasSelectedCol]);
    }

    // highlight new option
    update (m_pixmapRects [row][col]);

    emit optionSelected (row, col);
    return true;
}

//---------------------------------------------------------------------

bool WidgetBase::selectPreviousOption ()
{
    int newRow, newCol;
    if ( !hasPreviousOption (&newRow, &newCol) )
        return false;

    return setSelected (newRow, newCol);
}

//---------------------------------------------------------------------

bool WidgetBase::selectNextOption ()
{
    int newRow, newCol;
    if ( !hasNextOption (&newRow, &newCol) )
        return false;

    return setSelected (newRow, newCol);
}

//---------------------------------------------------------------------

bool WidgetBase::event (QEvent *e)
{
    if (e->type () == QEvent::ToolTip)
    {
        QHelpEvent *he = (QHelpEvent *) e;

        QString tip = getToolTip( he->pos() );
        e->accept ();
        if ( !tip.isEmpty () )
        {
            QToolTip::showText (he->globalPos (), tip, this);
        }
        else
        {
            QToolTip::hideText ();
        }
        return true;
    }
    else
        return QWidget::event (e);
}

//---------------------------------------------------------------------

void WidgetBase::mousePressEvent (QMouseEvent *e)
{
    e->ignore ();

    if (e->button () != Qt::LeftButton)
        return;

    for (int i = 0; i < (int) m_pixmapRects.count (); i++)
    {
        for (int j = 0; j < (int) m_pixmapRects [i].count (); j++)
            if ( m_pixmapRects [i][j].contains ( e->pos () ) )
            {
                setSelected (i, j);
                e->accept ();
                return;
            }
    }
}

//---------------------------------------------------------------------

void WidgetBase::paintEvent (QPaintEvent *e)
{
    // Draw frame first.
    QFrame::paintEvent (e);

    QPainter painter (this);

    for (int i = 0; i < (int) m_pixmaps.count (); i++)
        for (int j = 0; j < (int) m_pixmaps [i].count (); j++)
        {
            QRect rect = m_pixmapRects [i][j];
            QPixmap pixmap = m_pixmaps [i][j];

            if (i == m_selectedRow && j == m_selectedCol)
            {
                painter.fillRect( rect, palette().color(QPalette::Highlight).rgb() );
            }

            painter.drawPixmap(QPoint(rect.x () + ( rect.width () - pixmap.width () ) / 2,
                                      rect.y () + ( rect.height () - pixmap.height () ) / 2),
                               pixmap);
        }
}

//---------------------------------------------------------------------

QString WidgetBase::getToolTip (const QPoint & pos)
{
    for (int r = 0; r < m_pixmapRects.count (); r++)
        for (int c = 0; c < m_pixmapRects [r].count (); c++)
            if ( m_pixmapRects [r][c].contains (pos) )
                return m_toolTips [r][c];

    return "";
}

//---------------------------------------------------------------------
