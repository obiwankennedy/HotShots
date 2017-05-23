
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

#ifndef _MAINWINDOW_H_
#define _MAINWINDOW_H_

#include <QtCore/QTimer>

#include <QMainWindow>
#include <QSystemTrayIcon>

namespace Ui
{
class MainWindowClass;
}

class AboutDialog;
class GrabManager;
#ifndef Q_OS_PM // OS/2
//class QxtGlobalShortcut;
#endif
class EditorWidget;
class QLabel;
class Action;

struct ProxySetting
{
    bool useProxy;
    QString proxyHostname;
    int proxyPort;
    bool useProxyAuthentification;
    QString proxyUsername;
    QString proxyPassword;
    bool showProxyPassword;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    enum typScaleSnapshotMode
    {
        SCALE_BY_PERCENT = 0,
        SCALE_BY_SIZE
    };

    enum typAfterShotMode
    {
        DO_NOTHING = 0,
        DO_AUTOSAVE,
        DO_SAVE,
        DO_EDITOR,
        NB_AFTER_SHOT_MODE
    };

    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    bool openEditor(const QString &);

public slots:

    void on_actionQuit_triggered();
    void on_actionPreferences_triggered();
    void on_actionAbout_triggered();
    void on_pushButtonOutputPath_pressed();
    void on_pushButtonSnapshot_clicked();

    void on_actionSaveSnapshot_triggered();
    void on_actionCopyToClipboard_triggered();
    void copyToClipboard( const QPixmap &pix = QPixmap() );

    void on_actionShowUI_triggered();
    void iconActivated(QSystemTrayIcon::ActivationReason reason);

    void restart();
    void setPreferences();

    void resizeEvent(QResizeEvent *event);
    void closeEvent(QCloseEvent *event);

    void processNewSnapshot(const QPixmap &);

    void on_spinBoxDelay_valueChanged(int val);
    void on_comboBoxMode_activated(int val);
    void on_comboBoxAfterShot_activated(int val);

    void blink();

    void on_actionAddAnnotations_triggered();

    void on_actionGrabScreen_triggered();
    void on_actionGrabAllScreens_triggered();
    void on_actionGrabWindow_triggered();
    void on_actionGrabRegion_triggered();
    void on_actionGrabFreehandRegion_triggered();

    void on_actionUpload_triggered();
    void on_actionPrint_triggered();

    void restoreDisplayState();
    void saveDisplayState();

    void on_actionHelp_triggered();

    void uploadToWeb( const QPixmap &, const QString &preferredName = QString() );
    void exportToDisk( const QPixmap &, const QString &preferredName = QString() );
    void readResultUrl(const QString&);

    void wakeUp(const QString &mess);
    void updateDefaultPath(const QString &);
    void exportToPrinter(const QPixmap &pix);

private:

    void updateUi();
    void updateApplicationIdentity();
    void setupPreferences();

    void saveSettings();
    void loadSettings();
    void updateScreenshotLabel(const QPixmap &pix);

    // tray icon
    void createTrayIcon();
    void createActionsSystemTray();
    void changeTrayIcon();

    void initGlobalKeys();
    void applyGlobalKeys(bool);

    //QString getProposedFilename();
    bool saveSnapshot(const QPixmap &pix, const QString &file);

    bool openEditor(const QPixmap &pixmap);

    void initEditor();

    QString  formatUrl(const QString &url, int type);
    void playSound(const QString &file);
    QAction *getActionByName(const QString &actName);

    Ui::MainWindowClass *ui;

    QString m_version;
    QString m_appName;

    // preferences
    QString m_currentLanguage;
    bool m_splashscreenAtStartup;
    bool m_splashscreenTransparentBackground;
    bool m_startInTray;

    bool m_autoCopyToClipboard;
    bool m_startWithSystem;

    int m_delay;
    bool m_captureWindowWithDecoration;
    bool m_playSound;
    bool m_hideUIForSnapshot;

    AboutDialog *m_about;
    QPixmap m_currPixmap;

    //system tray
    QSystemTrayIcon *m_trayIcon;
    QMenu *m_trayIconMenu;
    bool m_firstCloseMessage;

    GrabManager *m_grabber;

    // global key variables
    bool m_useGlobalHotKey;
    QString m_globalHotkeyGrabScreen;
    QString m_globalHotkeyGrabAllScreens;
    QString m_globalHotkeyGrabWindow;
    QString m_globalHotkeyGrabRegion;
    QString m_globalHotkeyGrabFreehand;
/*#ifndef Q_OS_PM // OS/2
    QxtGlobalShortcut *m_globShortCutGrabScreen;
    QxtGlobalShortcut *m_globShortCutGrabAllScreens;
    QxtGlobalShortcut *m_globShortCutGrabWindow;
    QxtGlobalShortcut *m_globShortCutGrabRegion;
    QxtGlobalShortcut *m_globShortCutGrabFreehand;
#endif*/

    QTimer m_blinkTimer;
    int m_grabMode;
    int m_doAfterShot;

    bool m_scaleImage;
    int m_scaleValue;
    QSize m_maxSnapshotSize;
    int m_modeScaleSnapshot;

    int m_postEffect;
    bool m_rotateImage;
    double m_angle;

    EditorWidget *m_snapshotEditor;

    bool m_wasHidden;
    QLabel *m_labelSize;

    // network proxy
    ProxySetting m_proxiParams;

    // URL to Clipboard parameters
    bool m_copyUploadedUrlToClipboard;
    int m_clipboardUrlFormat;
    QString m_userUrlFormat;
    int m_defaultUploader;

    bool m_forceBgColor;
    QColor m_bgColor;
    bool m_launchBrowserWhenDone;
    bool m_showDialogBeforeDownload;
    bool m_showDialogAfterDownload;
    bool m_grabWithMouseCursor;
    QString m_defaultActionSystrayName;
    QStringList m_systrayUserActionList;
    QStringList m_systrayUserActionBlackList;
};

#endif // _MAINWINDOW_H_
