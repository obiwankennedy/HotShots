
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

#ifndef _WIDGET_BASE_H_
#define _WIDGET_BASE_H_

#include <QtCore/QPair>
#include <QtCore/QRect>
#include <QtCore/QList>

#include <QPixmap>
#include <QFrame>
#include <QWidget>

class QMouseEvent;

class WidgetBase : public QFrame
{
    Q_OBJECT

public:

    WidgetBase (QWidget *parent, const QString &name);
    virtual ~WidgetBase ();

public:

    void addOption ( const QPixmap &pixmap, const QString &toolTip = QString() );
    void startNewOptionRow ();

    // Call this at the end of your constructor.
    // If the default row & col could not be read from the config,
    // <fallBackRow> & <fallBackCol> are passed to setSelected().
    void finishConstruction (int fallBackRow, int fallBackCol);

    // (only have to use these if you don't use finishConstruction())
    // (rereads from config file)
    QPair <int, int> defaultSelectedRowAndCol () const;
    int defaultSelectedRow () const;
    int defaultSelectedCol () const;

    void relayoutOptions ();

    int selectedRow () const;
    int selectedCol () const;

    int selected () const;

    bool hasPreviousOption (int *row = 0, int *col = 0) const;
    bool hasNextOption (int *row = 0, int *col = 0) const;

public slots:

    virtual bool setSelected (int row, int col);
    bool selectPreviousOption ();
    bool selectNextOption ();
    void setBaseColorDeprecated(const QColor &);
    void setFgColor(const QColor &);
    void setBgColor(const QColor &);

signals:

    void optionSelected (int row, int col);

protected:

    virtual bool event (QEvent *e);
    virtual void mousePressEvent (QMouseEvent *e);
    virtual void paintEvent (QPaintEvent *e);
    virtual void init() {}
    void reset();

    QColor m_baseColorDeprecated;

    QColor m_fgColor;
    QColor m_bgColor;

private:

    QList <int> spreadOutElements (const QList <int> &sizes, int maxSize);
    QString getToolTip (const QPoint & pos);

    QWidget *m_baseWidget;

    QList < QList <QPixmap> > m_pixmaps;
    QList < QList <QString> > m_toolTips;
    QList < QList <QRect> > m_pixmapRects;

    int m_selectedRow;
    int m_selectedCol;
};

#endif  // _WIDGET_BASE_H_
