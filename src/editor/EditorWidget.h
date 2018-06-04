
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

#ifndef _EDITORWIDGET_H_
#define _EDITORWIDGET_H_

#include <QtCore/QMap>
#include <QMainWindow>

namespace Ui
{
class EditorWidgetClass;
}

class EditorScene;
class QLabel;

class EditorWidget : public QMainWindow
{
    Q_OBJECT

public:

    explicit EditorWidget(QWidget *parent = 0);
    ~EditorWidget();

    bool load(const QString &);
    void load(const QPixmap &);

signals:

    void requestUpload(const QPixmap &, const QString &name);
    void requestExport(const QPixmap &, const QString &name);
    void requestPrint(const QPixmap &);
    void requestClipboard(const QPixmap &);
    void defaultDirectoryChanged(const QString &);

private slots:

    void on_actionSelect_triggered();
    void on_actionText_triggered();
    void on_actionLine_triggered();
    void on_actionLineArrow_triggered();
    void on_actionPolyline_triggered();
    void on_actionEncircle_triggered();
    void on_actionPolygon_triggered();
    void on_actionEllipse_triggered();
    void on_actionRectangle_triggered();
    void on_actionArrow_triggered();
    void on_actionTag_triggered();
    void on_actionImage_triggered();
    void on_actionBlurring_triggered();
    void on_actionMagnifier_triggered();
    void on_actionHighlighter_triggered();
    void on_actionCrop_triggered();
    void on_actionCurve_triggered();

    void on_actionQuit_triggered();
    void on_actionFontSizeUp_triggered();
    void on_actionFontSizeDown_triggered();
    void on_actionFitToView_triggered();
    void on_actionResetScale_triggered();

    void on_actionUndo_triggered();
    void on_actionRedo_triggered();

    void on_actionDelete_triggered();
    void on_actionItemUp_triggered();
    void on_actionItemDown_triggered();
    void on_actionItemTop_triggered();
    void on_actionItemBottom_triggered();

    void on_actionShadow_triggered();
    void on_actionFont_triggered();
    void on_actionClear_triggered();
    void on_actionExport_triggered();
    void on_actionDuplicate_triggered();
    void on_actionCopyToClipboard_triggered();

    void on_actionNewDocument_triggered();
    void on_actionLoad_triggered();
    void on_actionSave_triggered();
    void on_actionSaveAs_triggered();
    void on_actionUpload_triggered();
    void on_actionPrint_triggered();

    void on_actionRescaleBackgroundImage_triggered();

    void on_actionImportClipboard_triggered();

    void on_labelFontSize_clicked();

    void displayCurrentItemData(bool force = false);

    void displayStatus(QAction*);
    void displayMousePosition(const QPointF &);
    void displayZoomFactor(qreal);

    void checkSceneMode(const QString &);

    void initSelectors();
    void changePostEffect(int);
    void updateBackground();

    void on_actionAddFrame_triggered();
    void on_actionNoPostEffect_triggered();
    void on_actionSimpleDropShadow_triggered();
    void on_actionDropShadowWithBorders_triggered();
    void on_actionProgressiveOpacity_triggered();
    void on_actionTornEdge_triggered();

    void on_actionAlignGroupLeft_triggered();
    void on_actionAlignHorizontalCenter_triggered();
    void on_actionAlignVerticalCenter_triggered();
    void on_actionAlignGroupRight_triggered();
    void on_actionAlignGroupTop_triggered();
    void on_actionAlignGroupBottom_triggered();
    void on_actionSetAlignment_triggered();

    void on_pushButtonResetColor_pressed();
    void on_pushButtonSwapColor_pressed();
    void setupRecentFilesMenu();
    void recentFileActivated();
    void updateColorPair(const QColor &fg,const QColor &bg);

protected slots:

    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);
    void closeEvent(QCloseEvent *event);
    void keyPressEvent ( QKeyEvent * event );

private:

    void loadSettings();
    void saveSettings();
    void updateTitle(const QString &file);
    void updateFontButton(const QFont &);
    void updateToolButtons();
    void restorePostEffectAction();

    void updateUi();
    void updateHelp();

    void updatePostEffectDefaultAction(QAction *);
    void updateAlignmentDefaultAction(QAction *);
    void addRecentlyOpenedFile(const QString &fn, QStringList &lst);

    Ui::EditorWidgetClass *m_ui;
    EditorScene *m_scene;

    QString m_lastImageDirectory;
    QString m_currentFile;

    QMap<QString, QString> m_commandHelp;
    QPixmap m_originalPixmap;
    int m_lastPostEffect;

    QLabel *m_labelPos;
    QLabel *m_labelHelp;
    QLabel *m_labelZoom;

    // recent files
    QMenu *m_recentFilesMenu;
    QStringList m_recentFiles;
};

#endif // _EDITORWIDGET_H_
