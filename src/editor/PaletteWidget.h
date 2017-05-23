
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

#ifndef _PALETTEWIDGET_H_
#define _PALETTEWIDGET_H_

#include <QColor>
#include <QWidget>

struct ColorSet
{
    ColorSet(const QColor &f, const QColor &b) : fg(f),bg(b){}
    ColorSet(const ColorSet &cs ) : fg(cs.fg),bg(cs.bg){}
    ColorSet(){}
    bool operator== (const ColorSet &other) const
    {
        return (other.fg == fg && other.bg == bg);
    }

    QColor fg;
    QColor bg;
};

class ColorHistory : public QObject
{
    Q_OBJECT

public:

    ColorHistory(){}
    virtual ~ColorHistory(){}

    const QList<ColorSet>  & getHistory() const {return m_history; }
    QList<ColorSet>  & getHistory()  {return m_history; }
    void addToHistory(const ColorSet &cs);
    int getHistorySize() const {return m_history.size(); }
    ColorSet getColor(int index) {return m_history.value(index); }

signals:

    void updated();

private:

    QList<ColorSet> m_history;
};

class QColorSet;

class PaletteWidget : public QWidget
{
    Q_OBJECT

public:

    PaletteWidget(QWidget *parent = 0);
    virtual ~PaletteWidget();

    //! Returns maximal scrolling offset
    int maximalScrollOffset() const;

    //! Returns the current scrolling offset
    int currentScrollOffset() const;

    /// Sets the color palette to display
    void setColorHistory(ColorHistory *ch);

    /// Returns the current palette
    ColorHistory * colorHistory() const;

public slots:

    void scrollForward();
    void scrollBackward();

signals:

    /// Emitted whenever a color was clicked
    void colorSelected(const QColor &fgColor,const QColor &bgColor);

    /// Emitted whenever the scroll offset changed
    void scrollOffsetChanged();

protected:

    virtual void paintEvent(QPaintEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void wheelEvent(QWheelEvent *event);
    virtual void keyPressEvent(QKeyEvent *event);
    virtual bool event(QEvent *event);

private:

    /// Returns color index from given position
    int indexFromPosition(const QPoint &position);

    /// Returns patch size
    QSize patchSize() const;

    /// Apply scrolling
    void applyScrolling(int delta);
    void saveSettings();
    void loadSettings();
    void initHistoryPalette(ColorHistory *h);

    int m_scrollOffset;
    ColorHistory *m_cHistory;
    int m_pressedIndex;
    bool m_hasDragged;
};

#endif // _PALETTEWIDGET_H_
