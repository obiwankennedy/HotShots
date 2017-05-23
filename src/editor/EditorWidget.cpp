
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

#include <QtCore/QUrl>
#include <QtCore/QTimer>
#include <QtCore/QVariant>
#include <QtCore/QDebug>

#include <QButtonGroup>
#include <QActionGroup>
#include <QFontDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QDragEnterEvent>
#include <QClipboard>
#include <QDesktopServices>
#include <QMenu>
#include <QUndoStack>
#include <QStandardPaths>
#include <QDesktopWidget>
#include <QPushButton>
#include <QProxyStyle>

#include "EditorWidget.h"
#include "ui_EditorWidget.h"

#include "EditorScene.h"
#include "AppSettings.h"
#include "FileParser.h"
#include "MiscFunctions.h"

#include "NameManager.h"
#include "PostEffect.h"
#include "RescaleDialog.h"

const int maxRecentlyOpenedFile = 10;

EditorWidget::EditorWidget(QWidget *parent) :
QMainWindow(parent),
m_ui(new Ui::EditorWidgetClass),
m_lastImageDirectory( QStandardPaths::writableLocation(QStandardPaths::HomeLocation) ),
m_lastPostEffect(PostEffect::EFFECT_NONE),
m_recentFilesMenu(NULL)
{
    m_ui->setupUi(this);
    updateUi();
    loadSettings();
}

EditorWidget::~EditorWidget()
{
    saveSettings();
    delete m_ui;
}

void EditorWidget::updateUi()
{
    m_scene = new EditorScene(this);
    m_ui->graphicsView->setScene(m_scene);

    // trace change in the scene (in order to view more than only scene rect
    connect( m_scene,SIGNAL( sceneRectChanged ( const QRectF &) ),m_ui->graphicsView,SLOT( updateSceneRect(const QRectF &) ) );

    // connect "style" widget to the scene
    connect( m_ui->frameLineWidth,SIGNAL( lineWidthChanged(int) ),m_scene,SLOT( setLineWidth(int) ) );
    connect( m_ui->frameDashType,SIGNAL( lineDashStyleChanged(int) ),m_scene,SLOT( setDashStyle(int) ) );
    connect( m_ui->frameFillStyle,SIGNAL( fillStyleChanged(int) ),m_scene,SLOT( setFillStyle(int) ) );

    connect( m_ui->qwwTwoColorIndicator,SIGNAL( fgChanged(const QColor &) ),m_scene,SLOT( setFgColor(const QColor &) ) );
    connect( m_ui->qwwTwoColorIndicator,SIGNAL( bgChanged(const QColor &) ),m_scene,SLOT( setBgColor(const QColor &) ) );

    // connect scene selection change to reflect item data values
    connect( m_scene,SIGNAL( selectionChanged() ),this,SLOT( displayCurrentItemData() ) );
    connect( m_scene,SIGNAL( currentMode(const QString &) ),this,SLOT( checkSceneMode(const QString &) ) );

    // update display with new color
    connect( m_ui->qwwTwoColorIndicator,SIGNAL( fgChanged(const QColor &) ),m_ui->frameLineWidth,SLOT( setFgColor(const QColor &) ) );
    connect( m_ui->qwwTwoColorIndicator,SIGNAL( fgChanged(const QColor &) ),m_ui->frameDashType,SLOT( setFgColor(const QColor &) ) );
    connect( m_ui->qwwTwoColorIndicator,SIGNAL( fgChanged(const QColor &) ),m_ui->frameFillStyle,SLOT( setFgColor(const QColor &) ) );
    connect( m_ui->qwwTwoColorIndicator,SIGNAL( bgChanged(const QColor &) ),m_ui->frameLineWidth,SLOT( setBgColor(const QColor &) ) );
    connect( m_ui->qwwTwoColorIndicator,SIGNAL( bgChanged(const QColor &) ),m_ui->frameDashType,SLOT( setBgColor(const QColor &) ) );
    connect( m_ui->qwwTwoColorIndicator,SIGNAL( bgChanged(const QColor &) ),m_ui->frameFillStyle,SLOT( setBgColor(const QColor &) ) );

    updateToolButtons();

    connect( m_ui->graphicsView,SIGNAL( mouseMoved(const QPointF &) ),this,SLOT( displayMousePosition(const QPointF &) ) );
    connect( m_ui->graphicsView,SIGNAL( scaleChanged(qreal) ),this,SLOT( displayZoomFactor(qreal) ) );

    updateHelp();

    // add post effect menu
    QMenu *peMenu = new QMenu(this);
    m_ui->actionAddFrame->setMenu(peMenu);
    peMenu->addAction(m_ui->actionNoPostEffect);
    peMenu->addAction(m_ui->actionSimpleDropShadow);
    peMenu->addAction(m_ui->actionDropShadowWithBorders);
    peMenu->addAction(m_ui->actionProgressiveOpacity);
    peMenu->addAction(m_ui->actionTornEdge);

    // add alignment menu
    QMenu *aMenu = new QMenu(this);
    m_ui->actionSetAlignment->setMenu(aMenu);
    aMenu->addAction(m_ui->actionAlignGroupLeft);
    aMenu->addAction(m_ui->actionAlignVerticalCenter);
    aMenu->addAction(m_ui->actionAlignGroupRight);
    aMenu->addAction(m_ui->actionAlignGroupTop);
    aMenu->addAction(m_ui->actionAlignHorizontalCenter);
    aMenu->addAction(m_ui->actionAlignGroupBottom);

    // trace change in the background image with crop item
    connect( m_scene,SIGNAL( backgroundChanged ( ) ),this,SLOT( updateBackground() ) );

    m_labelZoom = new QLabel(this);
    QMainWindow::statusBar()->addPermanentWidget(m_labelZoom);
    m_labelHelp = new QLabel(this);
    QMainWindow::statusBar()->addPermanentWidget(m_labelHelp);
    m_labelPos = new QLabel(this);
    QMainWindow::statusBar()->addPermanentWidget(m_labelPos);

    // recent files init
    m_recentFilesMenu = new QMenu(this);
    m_ui->actionLoad->setMenu( m_recentFilesMenu );
    m_recentFilesMenu->setDefaultAction (m_ui->actionLoad);

    connect( m_recentFilesMenu,SIGNAL( aboutToShow() ),this,SLOT( setupRecentFilesMenu() ) );

    // connection palette widget
    connect( m_scene,SIGNAL( newColorPair(const QColor &, const QColor &) ),m_ui->widgetPalette,SLOT( addToHistory(const QColor &, const QColor &) ) );
    connect( m_ui->widgetPalette,SIGNAL( colorSelected(const QColor &, const QColor &) ),this,SLOT( updateColorPair(const QColor &, const QColor &) ) );
}

void EditorWidget::updateToolButtons()
{
    m_ui->toolButtonSelect->setDefaultAction(m_ui->actionSelect);
    m_ui->toolButtonText->setDefaultAction(m_ui->actionText);
    m_ui->toolButtonLine->setDefaultAction(m_ui->actionLine);
    m_ui->toolButtonEncircle->setDefaultAction(m_ui->actionEncircle);

    m_ui->toolButtonPolyline->setDefaultAction(m_ui->actionPolyline);
    m_ui->toolButtonPolygon->setDefaultAction(m_ui->actionPolygon);
    m_ui->toolButtonCurve->setDefaultAction(m_ui->actionCurve);
    m_ui->toolButtonCrop->setDefaultAction(m_ui->actionCrop);
    m_ui->toolButtonRectangle->setDefaultAction(m_ui->actionRectangle);
    m_ui->toolButtonEllipse->setDefaultAction(m_ui->actionEllipse);
    m_ui->toolButtonArrow->setDefaultAction(m_ui->actionArrow);
    m_ui->toolButtonTag->setDefaultAction(m_ui->actionTag);
    m_ui->toolButtonHighlighter->setDefaultAction(m_ui->actionHighlighter);
    m_ui->toolButtonMagnifier->setDefaultAction(m_ui->actionMagnifier);
    m_ui->toolButtonImage->setDefaultAction(m_ui->actionImage);
    m_ui->toolButtonBlurring->setDefaultAction(m_ui->actionBlurring);
    m_ui->toolButtonFontSizeDown->setDefaultAction(m_ui->actionFontSizeDown);
    m_ui->toolButtonFontSizeUp->setDefaultAction(m_ui->actionFontSizeUp);

    QActionGroup *groupAction = new QActionGroup(this);
    groupAction->setExclusive(true);
    groupAction->addAction(m_ui->actionSelect);
    groupAction->addAction(m_ui->actionLine);
    groupAction->addAction(m_ui->actionEncircle);
    groupAction->addAction(m_ui->actionPolygon);
    groupAction->addAction(m_ui->actionPolyline);
    groupAction->addAction(m_ui->actionRectangle);
    groupAction->addAction(m_ui->actionEllipse);
    groupAction->addAction(m_ui->actionText);
    groupAction->addAction(m_ui->actionArrow);
    groupAction->addAction(m_ui->actionImage);
    groupAction->addAction(m_ui->actionTag);
    groupAction->addAction(m_ui->actionBlurring);
    groupAction->addAction(m_ui->actionHighlighter);
    groupAction->addAction(m_ui->actionMagnifier);
    groupAction->addAction(m_ui->actionCrop);
    groupAction->addAction(m_ui->actionCurve);

    connect( groupAction,SIGNAL( triggered (QAction*) ),this,SLOT( displayStatus(QAction*) ) );
}

void EditorWidget::updateHelp()
{
    m_commandHelp["Select"] = tr("Left-click to select item, Ctrl+Left Click then drag to select a group of items");
    m_commandHelp["EditorTextItem"] = tr("Left-click to set center");
    m_commandHelp["EditorLineItem"] = tr("Left-click then drag to create the shape");
    m_commandHelp["EditorEncircleItem"] = tr("Left-click then drag to create the shape");
    m_commandHelp["EditorPolylineItem"] = tr("Creation: left-click to create a point, middle-click to delete the last one and right-click to create a new shape. Modification: select vertex, left-click then drag to move, middle-click to delete and right-click to add a new one");
    m_commandHelp["EditorPolygonItem"] = tr("Creation: left-click to create a point, middle-click to delete the last one and right-click to create a new shape. Modification: select vertex, left-click then drag to move, middle-click to delete and right-click to add a new one");
    m_commandHelp["EditorEllipseItem"] = tr("Left-click then drag to create the shape");
    m_commandHelp["EditorRectangleItem"] = tr("Left-click then drag to create the shape");
    m_commandHelp["EditorArrowItem"] = tr("Left-click then drag to create the shape");
    m_commandHelp["EditorTagItem"] = tr("Left-click to set center");
    m_commandHelp["EditorImageItem"] = tr("Left-click to set image center and drag if you want change the size");
    m_commandHelp["EditorBlurItem"] = tr("Left-click then drag to create the shape");
    m_commandHelp["EditorHighlighterItem"] = tr("Left-click then drag to create the shape");
    m_commandHelp["EditorMagnifierItem"] = tr("Left-click then drag to create the shape");
    m_commandHelp["EditorCurveItem"] = tr("Creation: left-click to create a point, middle-click to delete the last one and right-click to create a new shape. Modification: select vertex, left-click then drag to move, middle-click to delete and right-click to add a new one");
    m_commandHelp["EditorCropItem"] = tr("Left-click then drag to create the shape, select the area and press <Enter> to perform the operation");
}

void EditorWidget::displayStatus(QAction*)
{
    QString help;
    if ( m_commandHelp.contains( m_scene->getMode() ) )
        help = m_commandHelp[m_scene->getMode()];

    m_labelHelp->setText(help);
}

void EditorWidget::initSelectors()
{
    displayCurrentItemData(true);
}

void EditorWidget::displayCurrentItemData(bool force)
{
    QList<QGraphicsItem*> items = m_scene->selectedItems();

    // enable/disable alignment actions
    m_ui->actionSetAlignment->setEnabled(items.count()>1);

    if ( items.count() !=1 && !force )
        return;

    m_ui->frameLineWidth->setCurrentLineWidth( m_scene->getLineWidth() );
    m_ui->frameDashType->setCurrentDashStyle( m_scene->getDashStyle() );
    m_ui->frameFillStyle->setCurrentFillStyle( m_scene->getFillStyle() );

    // update widget color
    m_ui->frameLineWidth->setFgColor( m_scene->fgColor() );
    m_ui->frameDashType->setFgColor( m_scene->fgColor() );
    m_ui->frameFillStyle->setFgColor( m_scene->fgColor() );
    m_ui->qwwTwoColorIndicator->setFgColor( m_scene->fgColor() );

    m_ui->frameLineWidth->setBgColor( m_scene->bgColor() );
    m_ui->frameDashType->setBgColor( m_scene->bgColor() );
    m_ui->frameFillStyle->setBgColor( m_scene->bgColor() );
    m_ui->qwwTwoColorIndicator->setBgColor( m_scene->bgColor() );

    updateFontButton( m_scene->font() );
}

void EditorWidget::load(const QPixmap &pix)
{
    if ( m_scene->hasItems() )
    {
        QMessageBox msgBox;
        msgBox.setText( tr("There is existing annotation items, what do you want to do?") );
        QPushButton *clearButton = msgBox.addButton(tr("clear annotation items ..."), QMessageBox::ActionRole);
        QPushButton *updateButton = msgBox.addButton(tr("update background image ..."), QMessageBox::ActionRole);

        msgBox.exec();

        if (msgBox.clickedButton() == clearButton)
        {
            // clear
            m_scene->clearItems();
        }
        else if (msgBox.clickedButton() == updateButton)
        {
            // update
        }
    }

    m_originalPixmap = pix;
    m_scene->setUnderlayImage( pix.toImage() );
    m_ui->graphicsView->fitScale();
    m_currentFile = "";
    updateTitle("");
}

bool EditorWidget::load(const QString &file)
{
    NameManager::lastSnapshotDirectory = QFileInfo(file).absolutePath();
    NameManager::lastSnapshotName = QFileInfo(file).baseName();
    // reset the snapshot num
    NameManager::currentSnapshotNum=0;

    if (QFileInfo(file).suffix().toLower() == "hot")
    {
        FileParser parser;
        m_scene->clearScene();

        if ( !parser.load(file,m_scene) )
        {
            QMessageBox::warning( this, tr("Loading HotShots file failed"), tr("Error loading HotShots file %1").arg( file ) );
            m_currentFile = "";
            updateTitle("");
            return false;
        }
        m_currentFile = file;
        updateTitle(m_currentFile);
    }
    else // simple image file
    {
        load( QPixmap(file) );
    }
    addRecentlyOpenedFile(file, m_recentFiles);


    return true;
}

//--------------------------------------------------------------------------------------

void EditorWidget::closeEvent(QCloseEvent * /* event */)
{
}

void EditorWidget::updateFontButton(const QFont &font)
{
    qDebug() << "EditorWidget::updateFontButton";

    m_ui->labelFontSize->setFont(font);

    QPixmap fontPreview( m_ui->labelFontSize->width(),1.5 * m_ui->labelFontSize->fontMetrics().height() );
    fontPreview.fill(Qt::transparent);
    QPainter painter(&fontPreview);
    painter.setFont(font);
    painter.drawText(fontPreview.rect(),Qt::AlignCenter,"a1Ab2Bc3");

    m_ui->labelFontSize->setPixmap(fontPreview);
}

void EditorWidget::on_labelFontSize_clicked()
{
    QFont font = m_ui->labelFontSize->font();

    bool ok;
    font = QFontDialog::getFont(&ok,font);
    if (ok)
    {
        updateFontButton(font);
        m_scene->setFont(font);
    }
}

void EditorWidget::updateTitle(const QString &file)
{
    QString title = tr("Snapshot editor: ") + file;
    if ( !m_scene->getUnderlayImage().isNull() )
        title += QString(" - %1x%2").arg( m_scene->getUnderlayImage().width() ).arg( m_scene->getUnderlayImage().height() );
    setWindowTitle(title);
}

//-----------------------------------------------------------------------
// Save/Restore parameters functions
//-----------------------------------------------------------------------

void EditorWidget::saveSettings()
{
    AppSettings settings;

    settings.beginGroup("Editor");

    settings.beginGroup("geom");

    settings.setValue( "MainWindowState",saveState(0) );

    settings.setValue( "size", size() );
    settings.setValue( "pos", pos() );

    settings.setValue( "fullScreen", isFullScreen() );
    settings.setValue( "maximized", isMaximized ());

    settings.endGroup();

    settings.setValue("lastImageDirectory", m_lastImageDirectory);

    settings.beginGroup("lastData");

    settings.setValue( "fgColor", m_scene->fgColor() );
    settings.setValue( "bgColor", m_scene->bgColor() );
    settings.setValue( "lineWidth", m_scene->getLineWidth() );
    settings.setValue( "dashStyle", m_scene->getDashStyle() );
    settings.setValue( "fillStyle", m_scene->getFillStyle() );
    settings.setValue( "font-family", m_scene->font().family() );
    settings.setValue( "font-size", m_scene->font().pointSize() );

    // last effect
    settings.setValue( "lastPostEffect", m_lastPostEffect );

    // recent files
    settings.setValue("recentlyOpenedFiles", m_recentFiles);
    settings.endGroup();

    settings.beginGroup("Shortcuts");

    // Save shortcuts
    QList<QObject*> childrens;
    childrens = children();
    QListIterator<QObject*> iterator(childrens);
    while( iterator.hasNext() )
    {
        QObject *object = iterator.next();
        QString classe = object->metaObject()->className();
        if( classe == "QAction" )
        {
            QString text = object->objectName();
            QString shortcut = ( (QAction *)object )->shortcut().toString();
            if( !shortcut.isEmpty() )
                settings.setValue(text, shortcut);
        }
    }
    settings.endGroup();

    settings.endGroup();
}

void EditorWidget::loadSettings()
{
    AppSettings settings;

    settings.beginGroup("Editor");
    settings.beginGroup("geom");

    restoreState(settings.value("MainWindowState").toByteArray(), 0);

    resize( settings.value( "size", QSize(400, 400) ).toSize() );
    move( settings.value( "pos", QPoint(200, 200) ).toPoint() );

    // check if restore position isn't outside the current screen definitions (additional screen has been unplugged)
    if (QApplication::desktop()->screenNumber( pos() ) < 0)
        move( QPoint(200, 200) );

    bool fullScreen = settings.value("fullScreen",false).toBool();

    if (fullScreen)
        showFullScreen ();

    bool maximized = settings.value("maximized",false).toBool();
    if (maximized)
        showMaximized ();

    settings.endGroup();

    m_lastImageDirectory = settings.value( "lastImageDirectory",QStandardPaths::writableLocation(QStandardPaths::HomeLocation) ).toString();

    settings.beginGroup("lastData");

    // scene
    QColor fgColor = settings.value( "fgColor",QColor(0,0,0) ).value<QColor>();
    m_scene->setFgColor(fgColor);

    QColor bgColor = settings.value( "bgColor",QColor(0,0,0) ).value<QColor>();
    m_scene->setBgColor(bgColor);

    int lineWidth = settings.value("lineWidth",1).toInt();
    m_scene->setLineWidth(lineWidth);
    int dashStyle = settings.value("dashStyle",1).toInt();
    m_scene->setDashStyle(dashStyle);
    int fillStyle = settings.value("fillStyle",1).toInt();
    m_scene->setFillStyle(fillStyle);
    QString fontFamily = settings.value("font-family",1).toString();
    int fontSize = settings.value("font-size",1).toInt();

    // last effect
    m_lastPostEffect = settings.value("lastPostEffect",PostEffect::EFFECT_NONE).toInt();
    restorePostEffectAction();

    QFont f(fontFamily,fontSize);
    m_scene->setFont(f);

    // for font button must have a correct for the button to ellide text, so we have to wait that the UI is already constructed
    // Nota: yes I known, it's kitchen english ...
    QTimer::singleShot( 200, this, SLOT( initSelectors() ) );

    // recent files
    m_recentFiles = settings.value( "recentlyOpenedFiles").toStringList();

    settings.endGroup();

    settings.beginGroup("Shortcuts");

    // Load shortcuts
    QList<QObject*> childrens;
    childrens = children();
    QListIterator<QObject*> iterator(childrens);
    while( iterator.hasNext() )
    {
        QObject *object = iterator.next();
        QString classe = object->metaObject()->className();
        if( classe == "QAction" )
        {
            QString text = object->objectName();
            QString shortcut = ( (QAction *)object )->shortcut().toString();
            if( !text.isEmpty() )
            {
                shortcut = settings.value(text, shortcut).toString();
                ( (QAction *)object )->setShortcut( shortcut );

                // update the  tooltip
                if ( !shortcut.isEmpty() )
                {
                    // strip the (...) if needed
                    QString tooltip = ( (QAction *)object )->toolTip().replace(QRegExp(" \\(.*\\)$"),"");
                    ( (QAction *)object )->setToolTip(tooltip + " (" + shortcut + ")");
                }
            }
        }
    }
    settings.endGroup();

    settings.endGroup();
}

void EditorWidget::restorePostEffectAction()
{
    switch (m_lastPostEffect)
    {
    case PostEffect::EFFECT_DROP_SHADOW:
        {
            updatePostEffectDefaultAction(m_ui->actionSimpleDropShadow);
            break;
        }
    case PostEffect::EFFECT_POLAROID_DROP_SHADOW:
        {
            updatePostEffectDefaultAction(m_ui->actionDropShadowWithBorders);
            break;
        }
    case PostEffect::EFFECT_PROGRESSIVE_EDGE:
        {
            updatePostEffectDefaultAction(m_ui->actionProgressiveOpacity);
            break;
        }
    case PostEffect::EFFECT_TORN_EDGE:
        {
            updatePostEffectDefaultAction(m_ui->actionTornEdge);
            break;
        }
    case PostEffect::EFFECT_NONE:
    default:
        {
            break;
        }
    }
}

// ------------- Drag and drop -----------
#include <QMimeData>
void EditorWidget::dragEnterEvent(QDragEnterEvent *event)
{
    const QMimeData* mimeData = event->mimeData();

    if ( !mimeData->hasUrls() )
    {
        event->ignore();
        return;
    }

    QList<QUrl> urls = mimeData->urls();

    if(urls.count() != 1)
    {
        event->ignore();
        return;
    }

    QUrl url = urls.at(0);
    QString filename = url.toLocalFile();

    // We don't test extension
    if ( !QFileInfo(filename).exists() )
    {
        event->ignore();
        return;
    }

    if ( !QFileInfo(filename).isFile() )
    {
        event->ignore();
        return;
    }

    event->acceptProposedAction();
}

void EditorWidget::dragMoveEvent(QDragMoveEvent *event)
{
    if ( event->source() == this )
    {
        event->setDropAction(Qt::MoveAction);
        event->accept();
    }
    else
    {
        event->acceptProposedAction();
    }
}

void EditorWidget::dropEvent(QDropEvent *event)
{
    const QMimeData* mimeData = event->mimeData();
    QList<QUrl> urls = mimeData->urls();
    QUrl url = urls.at(0);

    QString filename = url.toLocalFile();
    load( filename );
}

void EditorWidget::displayZoomFactor(qreal zoom)
{
    m_labelZoom->setText( tr("Scale x%1").arg( zoom, 3, 'g', 3 ) );
}

void EditorWidget::displayMousePosition(const QPointF &pt)
{
    if ( pt.isNull() )
        m_labelPos->setText("");
    else
        m_labelPos->setText( QString("%1x%2").arg( qRound( pt.x() ), 4 ).arg( qRound( pt.y() ),4 ) );
}

void EditorWidget::checkSceneMode(const QString &m)
{
    // a bit ugly, no ? But I've no time now to find a finiest solution, I'm tired
    if ( m == "Select" && !m_ui->actionSelect->isChecked() )
    {
        m_ui->actionSelect->setChecked(true);
        displayStatus(0); // update the help text
    }
}

void EditorWidget::changePostEffect(int type)
{
    if ( m_originalPixmap.isNull() )
        m_originalPixmap = QPixmap::fromImage( m_scene->getUnderlayImage() );

    m_scene->setUnderlayImage( PostEffect::process( m_originalPixmap,type ).toImage() );
}

void EditorWidget::updateBackground()
{
    m_originalPixmap = QPixmap::fromImage( m_scene->getUnderlayImage() );
}

void EditorWidget::keyPressEvent ( QKeyEvent * event )
{
    if ( event->key() == Qt::Key_Escape )
    {
        m_ui->actionSelect->activate(QAction::Trigger);
    }
    else if (event->key() == Qt::Key_Plus)
    {
        m_ui->graphicsView->zoomPlus();
    }
    else if (event->key() == Qt::Key_Minus)
    {
        m_ui->graphicsView->zoomMinus();
    }
}

// new actions

//------------------------------------------------------------------------------------------
// Create new Items
//------------------------------------------------------------------------------------------

void EditorWidget::on_actionSelect_triggered()
{
    m_scene->setMode("Select");
}

void EditorWidget::on_actionText_triggered()
{
    m_scene->setMode("EditorTextItem");
}

void EditorWidget::on_actionLine_triggered()
{
    m_scene->setMode("EditorLineItem");
}

void EditorWidget::on_actionPolyline_triggered()
{
    m_scene->setMode("EditorPolylineItem");
}

void EditorWidget::on_actionEncircle_triggered()
{
    m_scene->setMode("EditorEncircleItem");
}

void EditorWidget::on_actionPolygon_triggered()
{
    m_scene->setMode("EditorPolygonItem");
}

void EditorWidget::on_actionEllipse_triggered()
{
    m_scene->setMode("EditorEllipseItem");
}

void EditorWidget::on_actionRectangle_triggered()
{
    m_scene->setMode("EditorRectangleItem");
}

void EditorWidget::on_actionArrow_triggered()
{
    m_scene->setMode("EditorArrowItem");
}

void EditorWidget::on_actionTag_triggered()
{
    m_scene->setMode("EditorTagItem");
}

void EditorWidget::on_actionImage_triggered()
{
    QString formats = MiscFunctions::getAvailablesImageFormats();
    QString file = QFileDialog::getOpenFileName(
        this,
        "Select one image to add in the scene",
        m_lastImageDirectory,
        formats);

    if ( file.isEmpty() )
    {
        m_ui->actionSelect->activate(QAction::Trigger);
        return;
    }

    m_lastImageDirectory = QFileInfo(file).absolutePath();

    // add image file to item settings
    m_scene->setItemSetting("pixmap",file);
    m_scene->setMode("EditorImageItem");
}

void EditorWidget::on_actionBlurring_triggered()
{
    m_scene->setMode("EditorBlurItem");
}

void EditorWidget::on_actionMagnifier_triggered()
{
    m_scene->setMode("EditorMagnifierItem");
}

void EditorWidget::on_actionHighlighter_triggered()
{
    m_scene->setMode("EditorHighlighterItem");
}

void EditorWidget::on_actionCurve_triggered()
{
    m_scene->setMode("EditorCurveItem");
}

void EditorWidget::on_actionCrop_triggered()
{
    m_scene->setMode("EditorCropItem");
}

//--------------------------------------------------------------------------------------

void EditorWidget::on_actionFitToView_triggered()
{
    m_ui->graphicsView->fitScale();
}

void EditorWidget::on_actionResetScale_triggered()
{
    m_ui->graphicsView->resetScale();
}

void EditorWidget::on_actionItemTop_triggered()
{
    m_scene->placeSelectedLayers(EditorScene::POSITION_TOP);
}

void EditorWidget::on_actionItemBottom_triggered()
{
    m_scene->placeSelectedLayers(EditorScene::POSITION_BOTTOM);
}

void EditorWidget::on_actionItemUp_triggered()
{
    m_scene->placeSelectedLayers(EditorScene::POSITION_UP);
}

void EditorWidget::on_actionItemDown_triggered()
{
    m_scene->placeSelectedLayers(EditorScene::POSITION_DOWN);
}

void EditorWidget::on_actionDelete_triggered()
{
    m_scene->deleteSelection();
}

void EditorWidget::on_actionDuplicate_triggered()
{
    m_scene->cloneSelection();
}

void EditorWidget::on_actionQuit_triggered()
{
    m_scene->setMode("Select");
    close();
}

void EditorWidget::on_actionFontSizeUp_triggered()
{
    QFont font = m_ui->labelFontSize->font();
    font.setPointSize(font.pointSize() + 1);
    updateFontButton(font);
    m_scene->increaseFontSize();
}

void EditorWidget::on_actionFontSizeDown_triggered()
{
    QFont font = m_ui->labelFontSize->font();
    font.setPointSize(font.pointSize() - 1);
    updateFontButton(font);
    m_scene->decreaseFontSize();
}

void EditorWidget::on_actionShadow_triggered()
{
    m_scene->toggleShadow();
}

void EditorWidget::on_actionFont_triggered()
{
    QFont font = m_ui->actionFont->font();

    bool ok;
    font = QFontDialog::getFont(&ok,font);
    if (ok)
    {
        updateFontButton(font);
        m_scene->setFont(font);
    }
}

void EditorWidget::on_actionClear_triggered()
{
    if ( !m_scene->hasItems() )
        return;

    if (QMessageBox::question(this,
        tr("Clear edit"),
        tr("Clear all the exiting items ? (no possible undo)"),
        QMessageBox::Ok | QMessageBox::Cancel,
        QMessageBox::Cancel)
        == QMessageBox::Ok)
    {
        m_scene->clearItems();
    }
}

void EditorWidget::on_actionExport_triggered()
{
    emit requestExport(m_scene->getRenderToPixmap(),"");
}

void EditorWidget::on_actionCopyToClipboard_triggered()
{
    QPixmap pix = m_scene->getRenderToPixmap();
    emit requestClipboard(pix);
}

void EditorWidget::on_actionNewDocument_triggered()
{
    if (QMessageBox::question(this,
        tr("Clear document"),
        tr("Create a new document?"),
        QMessageBox::Ok | QMessageBox::Cancel,
        QMessageBox::Cancel)
        == QMessageBox::Ok)
    {
        m_scene->clearScene();
        updateTitle("");
    }
}

void EditorWidget::on_actionLoad_triggered()
{
    qDebug() << "on_actionLoad_triggered";
    QString formats = MiscFunctions::getAvailablesImageFormats();
    QString file = QFileDialog::getOpenFileName(
        this,
        "Select one file to open",
        NameManager::lastSnapshotDirectory,
        "HotShots Editor files (*.hot);;" + formats);

    if ( !file.isEmpty() )
        load(file);
}

void EditorWidget::on_actionSaveAs_triggered()
{
    QString proposedFile = NameManager::getProposedFilename();

    QFileInfo fi(proposedFile);
    QFileInfo exported(fi.absolutePath() + "/" + fi.baseName() + ".hot");
    proposedFile = exported.absoluteFilePath();

    // ask for a filename ...
    QString savefilename = QFileDialog::getSaveFileName( this, tr("Save HotShots File"),
        proposedFile,
        tr("HotShots files (*.hot)") );

    if ( savefilename.isEmpty() )
        return;

    m_currentFile = savefilename;

    on_actionSave_triggered();
}

void EditorWidget::on_actionSave_triggered()
{
    if ( m_currentFile.isEmpty() )
    {
        on_actionSaveAs_triggered();
        return;
    }

    // check if .hot file defined
    if (QFileInfo( m_currentFile ).suffix().toLower() != "hot")
        m_currentFile += ".hot";

    NameManager::lastSnapshotDirectory = QFileInfo(m_currentFile).absolutePath();

    emit defaultDirectoryChanged(NameManager::lastSnapshotDirectory);

    FileParser parser;

    bool ok = parser.save(m_currentFile,m_scene);

    if (!ok)
    {
        QMessageBox::warning( this, tr("Saving HotShots file failed"), tr("Error saving HotShots file %1").arg( m_currentFile ) );
        updateTitle("");
        return;
    }

    NameManager::lastSnapshotName = QFileInfo(m_currentFile).baseName();
    // reset the snapshot num
    NameManager::currentSnapshotNum=0;

    addRecentlyOpenedFile(m_currentFile, m_recentFiles);

    updateTitle(m_currentFile);
}

void EditorWidget::on_actionImportClipboard_triggered()
{
    const QClipboard *clipboard = QApplication::clipboard();
    const QMimeData *mimeData = clipboard->mimeData();

    if ( mimeData->hasImage() )
        load( qvariant_cast<QPixmap>( mimeData->imageData() ) );
}

void EditorWidget::on_actionPrint_triggered()
{
    emit requestPrint( m_scene->getRenderToPixmap() );
}

void EditorWidget::on_actionUpload_triggered()
{
    QString proposedFile;
    if ( !m_currentFile.isEmpty() )
    {
        QFileInfo fi(m_currentFile);
        QFileInfo exported(fi.absolutePath() + "/" + fi.baseName() + ".png");
        proposedFile = exported.absoluteFilePath();
    }
    emit requestUpload(m_scene->getRenderToPixmap(), proposedFile);
}

void EditorWidget::on_actionUndo_triggered()
{
    m_scene->resetSelection();
    m_scene->getUndoStack()->undo();
}

void EditorWidget::on_actionRedo_triggered()
{
    m_scene->resetSelection();
    m_scene->getUndoStack()->redo();
}

void EditorWidget::on_actionAddFrame_triggered()
{
    if ( m_ui->actionAddFrame->menu()->defaultAction() )
        m_ui->actionAddFrame->menu()->defaultAction()->activate(QAction::Trigger);
}

void EditorWidget::on_actionSetAlignment_triggered()
{
    if ( m_ui->actionSetAlignment->menu()->defaultAction() )
        m_ui->actionSetAlignment->menu()->defaultAction()->activate(QAction::Trigger);
}

void EditorWidget::updateAlignmentDefaultAction(QAction *act)
{
    m_ui->actionSetAlignment->menu()->setDefaultAction(act);
    m_ui->actionSetAlignment->menu()->setActiveAction(act);
    m_ui->actionSetAlignment->setIcon( act->icon() );
}


void EditorWidget::updatePostEffectDefaultAction(QAction *act)
{
    m_ui->actionAddFrame->menu()->setDefaultAction(act);
    m_ui->actionAddFrame->menu()->setActiveAction(act);
    m_ui->actionAddFrame->setIcon( act->icon() );
}

void EditorWidget::on_actionNoPostEffect_triggered()
{
    m_lastPostEffect = PostEffect::EFFECT_NONE;
    restorePostEffectAction();
    changePostEffect(m_lastPostEffect);
}

void EditorWidget::on_actionSimpleDropShadow_triggered()
{
    m_lastPostEffect = PostEffect::EFFECT_DROP_SHADOW;
    restorePostEffectAction();
    changePostEffect(m_lastPostEffect);
}

void EditorWidget::on_actionDropShadowWithBorders_triggered()
{
    m_lastPostEffect = PostEffect::EFFECT_POLAROID_DROP_SHADOW;
    restorePostEffectAction();
    changePostEffect(m_lastPostEffect);
}

void EditorWidget::on_actionProgressiveOpacity_triggered()
{
    m_lastPostEffect = PostEffect::EFFECT_PROGRESSIVE_EDGE;
    restorePostEffectAction();
    changePostEffect(m_lastPostEffect);
}

void EditorWidget::on_actionTornEdge_triggered()
{
    m_lastPostEffect = PostEffect::EFFECT_TORN_EDGE;
    restorePostEffectAction();
    changePostEffect(m_lastPostEffect);
}

void EditorWidget::on_pushButtonResetColor_pressed()
{
    m_ui->qwwTwoColorIndicator->setFgColor(Qt::black);
    m_ui->qwwTwoColorIndicator->setBgColor(Qt::white);
}

void EditorWidget::on_pushButtonSwapColor_pressed()
{
    m_ui->qwwTwoColorIndicator->switchColors();
}

void EditorWidget::setupRecentFilesMenu()
{
    QAction *of = m_recentFilesMenu->defaultAction();
    m_recentFilesMenu->clear();
    m_recentFilesMenu->setDefaultAction(of);

    if (m_recentFiles.count() > 0)
    {
        QStringList::Iterator it = m_recentFiles.begin();

        for (; it != m_recentFiles.end(); ++it)
        {
            QAction *act = m_recentFilesMenu->addAction(*it);
            connect( act,SIGNAL( triggered() ), SLOT( recentFileActivated() ) );
        }
    }
}

void EditorWidget::recentFileActivated()
{
    QAction *action = qobject_cast<QAction*>( sender() );
    if ( action && !action->text().isEmpty() )
    {
        load( action->text() );
    }
}

void EditorWidget::addRecentlyOpenedFile(const QString &fn, QStringList &lst)
{
    QFileInfo fi(fn);

    if ( lst.contains( fi.absoluteFilePath() ) )
        return;

    if ( lst.count() >= maxRecentlyOpenedFile )
        lst.removeLast();

    lst.prepend( fi.absoluteFilePath() );
}

void EditorWidget::updateColorPair(const QColor &fg,const QColor &bg)
{
    m_scene->setColorPair(fg, bg);
    m_ui->qwwTwoColorIndicator->setFgColor(fg);
    m_ui->qwwTwoColorIndicator->setBgColor(bg);
}

void EditorWidget::on_actionAlignGroupLeft_triggered()
{
    updateAlignmentDefaultAction(m_ui->actionAlignGroupLeft);
    m_scene->setAlignment(EditorScene::ALIGN_LEFT);
}

void EditorWidget::on_actionAlignHorizontalCenter_triggered()
{
    updateAlignmentDefaultAction(m_ui->actionAlignHorizontalCenter);
    m_scene->setAlignment(EditorScene::ALIGN_H_CENTER);
}

void EditorWidget::on_actionAlignVerticalCenter_triggered()
{
    updateAlignmentDefaultAction(m_ui->actionAlignVerticalCenter);
    m_scene->setAlignment(EditorScene::ALIGN_V_CENTER);
}

void EditorWidget::on_actionAlignGroupRight_triggered()
{
    updateAlignmentDefaultAction(m_ui->actionAlignGroupRight);
    m_scene->setAlignment(EditorScene::ALIGN_RIGHT);
}

void EditorWidget::on_actionAlignGroupTop_triggered()
{
    updateAlignmentDefaultAction(m_ui->actionAlignGroupTop);
    m_scene->setAlignment(EditorScene::ALIGN_TOP);
}

void EditorWidget::on_actionAlignGroupBottom_triggered()
{
    updateAlignmentDefaultAction(m_ui->actionAlignGroupBottom);
    m_scene->setAlignment(EditorScene::ALIGN_BOTTOM);
}

void EditorWidget::on_actionRescaleBackgroundImage_triggered()
{
    RescaleDialog dia;

    dia.setOriginalSize(m_originalPixmap.size());

    if (dia.exec() == QDialog::Accepted)
    {
        m_originalPixmap = m_originalPixmap.scaled(dia.getFinalSize(),Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
        m_scene->setUnderlayImage( m_originalPixmap.toImage() );
    }
}

