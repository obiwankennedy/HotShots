
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

#include "PaletteBarWidget.h"
#include "PaletteWidget.h"

#include <QToolButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMenu>

const int FixedWidgetSize = 20;
const int ScrollUpdateIntervall = 25;

PaletteBarWidget::PaletteBarWidget(QWidget *parent)
    : QWidget(parent),
    m_prevButton(0),
    m_nextButton(0),
    m_colorBar(0)
{
    m_prevButton = new QToolButton(this);
    m_prevButton->setAutoRepeat(true);
    m_prevButton->setAutoRepeatInterval(ScrollUpdateIntervall);
    m_nextButton = new QToolButton(this);
    m_nextButton->setAutoRepeat(true);
    m_nextButton->setAutoRepeatInterval(ScrollUpdateIntervall);

    m_colorBar = new PaletteWidget(this);
    connect( m_prevButton, SIGNAL( clicked() ), m_colorBar, SLOT( scrollBackward() ) );
    connect( m_nextButton, SIGNAL( clicked() ), m_colorBar, SLOT( scrollForward() ) );
    connect( m_colorBar, SIGNAL( colorSelected(const QColor &,const QColor &) ), this, SIGNAL( colorSelected(const QColor &,const QColor &) ) );
    connect( m_colorBar, SIGNAL( scrollOffsetChanged() ), this, SLOT( updateButtons() ) );

    setMinimumSize(FixedWidgetSize, FixedWidgetSize);
    m_colorBar->setMinimumSize(FixedWidgetSize, FixedWidgetSize);

    createLayout();
}

PaletteBarWidget::~PaletteBarWidget()
{
}

void PaletteBarWidget::addToHistory(const QColor&fg, const QColor &bg)
{
    ColorSet cs(fg,bg);
    m_colorBar->colorHistory()->addToHistory(cs);
}

void PaletteBarWidget::createLayout()
{
    m_prevButton->setArrowType(Qt::LeftArrow);
    m_nextButton->setArrowType(Qt::RightArrow);
    QHBoxLayout *h = new QHBoxLayout();
    h->addWidget(m_colorBar, 1, Qt::AlignVCenter);
    h->addWidget(m_prevButton);
    h->addWidget(m_nextButton);
    setLayout(h);
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    m_colorBar->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);

    layout()->setMargin(0);
    layout()->setSpacing(2);
}

void PaletteBarWidget::updateButtons()
{
    m_prevButton->setEnabled(m_colorBar->currentScrollOffset() > 0);
    m_nextButton->setEnabled( m_colorBar->currentScrollOffset() < m_colorBar->maximalScrollOffset() );
}
