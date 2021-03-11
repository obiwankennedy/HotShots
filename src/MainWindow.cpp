
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

#include <QtCore/QDateTime>
#include <QtCore/QFileInfo>
#include <QtCore/QProcess>
#include <QtCore/QtDebug>

#include <QClipboard>
#include <QCloseEvent>
#include <QCompleter>
#include <QDesktopServices>
#include <QDesktopWidget>
#include <QDirModel>
#include <QFileDialog>
#include <QImageReader>
#include <QMenu>
#include <QMessageBox>
#include <QPrintDialog>
#include <QPrinter>

#include <QSound>

#ifndef Q_OS_PM // OS/2
#include <QxtGlobalShortcut>
#endif

#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "AboutDialog.h"
#include "AppSettings.h"
#include "GrabManager.h"
#include "LogHandler.h"
#include "MiscFunctions.h"
#include "PostEffect.h"

#include "EditorWidget.h"

#include "BaseUploader.h"
#include "NameManager.h"
#include "UploaderManager.h"
#include "UploaderRegistering.h"

const int blinkInterval= 500;
const int blinkNumber= 6;
const int minDelayForRepeat= 5;

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindowClass)
    , m_version(PACKAGE_VERSION)
    , m_appName(PACKAGE_NAME)
    , m_splashscreenAtStartup(true)
    , m_splashscreenTransparentBackground(true)
    , m_startInTray(false)
    , m_autoCopyToClipboard(false)
    , m_startWithSystem(false)
    , m_delay(0)
    , m_captureWindowWithDecoration(true)
    , m_playSound(false)
    , m_hideUIForSnapshot(true)
    , m_about(NULL)
    , m_trayIcon(NULL)
    , m_trayIconMenu(NULL)
    , m_firstCloseMessage(false)
    , m_grabber(NULL)
    , m_useGlobalHotKey(false)
    , m_globalHotkeyGrabScreen("F5")
    , m_globalHotkeyGrabAllScreens("Ctrl+Alt+F5")
    , m_globalHotkeyGrabWindow("Ctrl+F5")
    , m_globalHotkeyGrabRegion("Alt+F5")
    , m_globalHotkeyGrabFreehand("Shift+Alt+F5")
    ,
    /*#ifndef Q_OS_PM // OS/2
        m_globShortCutGrabScreen(NULL),
        m_globShortCutGrabAllScreens(NULL),
        m_globShortCutGrabWindow(NULL),
        m_globShortCutGrabRegion(NULL),
        m_globShortCutGrabFreehand(NULL),
    #endif*/
    m_grabMode(GrabManager::GRAB_ALL_SCREENS)
    , m_doAfterShot(DO_NOTHING)
    , m_scaleImage(false)
    , m_scaleValue(100)
    , m_maxSnapshotSize(QSize(800, 600))
    , m_modeScaleSnapshot(SCALE_BY_PERCENT)
    , m_postEffect(PostEffect::EFFECT_NONE)
    , m_rotateImage(false)
    , m_angle(0.0)
    , m_snapshotEditor(NULL)
    , m_wasHidden(false)
    , m_labelSize(NULL)
    , m_copyUploadedUrlToClipboard(true)
    , m_clipboardUrlFormat(BaseUploader::FORMAT_NONE)
    , m_userUrlFormat("@url@")
    , m_defaultUploader(-1)
    , m_forceBgColor(false)
    , m_bgColor(QColor(Qt::white))
    , m_launchBrowserWhenDone(false)
    , m_showDialogBeforeDownload(true)
    , m_showDialogAfterDownload(false)
    , m_grabWithMouseCursor(false)
    , m_defaultActionSystrayName("actionShowUI")
{
    ui->setupUi(this);
    updateUi();
    loadSettings();
    createTrayIcon();

    // setup global key menu
    initGlobalKeys();
}

MainWindow::~MainWindow()
{
    saveSettings();
    delete ui;
    delete m_snapshotEditor;
    UploaderManager::clear();
}

void MainWindow::on_actionQuit_triggered()
{
    QApplication::exit(0);
}

void MainWindow::on_pushButtonOutputPath_pressed()
{
    QString dir= QFileDialog::getExistingDirectory(this, tr("Save Directory"), NameManager::lastSnapshotDirectory,
                                                   QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if(!dir.isEmpty())
    {
        NameManager::lastSnapshotDirectory= dir;
        ui->lineEditOutputPath->setText(NameManager::lastSnapshotDirectory);
    }
}

void MainWindow::updateApplicationIdentity()
{
    QString title= m_appName + " - " + m_version;
    if(!m_currPixmap.isNull())
        title+= QString(" - %1x%2").arg(m_currPixmap.width()).arg(m_currPixmap.height());
    setWindowTitle(title);
}

void MainWindow::updateUi()
{
    setStatusBar(NULL); // remove status bar

    QWidget* spacer= new QWidget();
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->mainToolBar->insertWidget(ui->actionHelp, spacer);

    // setup app name, release, ...
    updateApplicationIdentity();

    // uploader registering
    int nb= UploaderRegistering::registerAll();
    LogHandler::getInstance()->reportDebug(tr("Register of %1 uploaders").arg(nb));
    const QList<BaseUploader*> list= UploaderManager::getUploaders();
    foreach(BaseUploader* up, list)
        connect(up, SIGNAL(newUrl(const QString&)), this, SLOT(readResultUrl(const QString&)));

    // initEditor();

    // add completer to line edit
    QCompleter* completerFile= new QCompleter(this);
    QDirModel* dirModelFile= new QDirModel(completerFile);
    dirModelFile->setFilter(QDir::Dirs);
    completerFile->setModel(dirModelFile);
    ui->lineEditOutputPath->setCompleter(completerFile);

    createActionsSystemTray();

    m_grabber= new GrabManager(this);

    // connect( m_grabber,SIGNAL( showMainInterface() ),this,SLOT( restoreDisplayState() ) );
    connect(m_grabber, SIGNAL(hideMainInterface()), this, SLOT(saveDisplayState()));
    connect(m_grabber, SIGNAL(newSnapshot(const QPixmap&)), this, SLOT(processNewSnapshot(const QPixmap&)));

    // setup "blinking" timer
    m_blinkTimer.setInterval(blinkInterval);
    connect(&m_blinkTimer, SIGNAL(timeout()), this, SLOT(blink()));

    // post process class
    // m_postProcess = new PostEffect(this);

    // add a label to display the size of the current snapshot)
    //     m_labelSize = new QLabel(this);
    //     QMainWindow::statusBar()->addPermanentWidget(m_labelSize);

    // temp storage check
    QString tmpPath= MiscFunctions::getValidStorage();

    if(!QDir(tmpPath).exists())
    {
        if(!QDir::home().mkpath(tmpPath))
            QMessageBox::warning(this, m_appName, tr("Can't create storage location, check why?! (%1)").arg(tmpPath));
    }

    // init combo box
    ui->comboBoxMode->addItem(ui->actionGrabScreen->text(), GrabManager::GRAB_ALL_SCREENS);
    ui->comboBoxMode->addItem(ui->actionGrabAllScreens->text(), GrabManager::GRAB_CURRENT_SCREEN);
    ui->comboBoxMode->addItem(ui->actionGrabWindow->text(), GrabManager::GRAB_WINDOW_UNDER_CURSOR);
    ui->comboBoxMode->addItem(ui->actionGrabRegion->text(), GrabManager::GRAB_RECTANGULAR_REGION);
    ui->comboBoxMode->addItem(ui->actionGrabFreehandRegion->text(), GrabManager::GRAB_FREEHAND_REGION);

#ifdef Q_OS_PM                               // OS/2
    ui->actionGrabWindow->setEnabled(false); // not yet available, sorry !
#endif

    // mandatory systray actions list
    m_systrayUserActionBlackList << "actionQuit";
    m_systrayUserActionBlackList << "actionShowUI";
    m_systrayUserActionBlackList << "actionGrabScreen";
    m_systrayUserActionBlackList << "actionGrabAllScreens";
    m_systrayUserActionBlackList << "actionGrabWindow";
    m_systrayUserActionBlackList << "actionGrabRegion";
    m_systrayUserActionBlackList << "actionGrabFreehandRegion";
}

//-----------------------------------------------------------------------
// Save/Restore parameters functions
//-----------------------------------------------------------------------

void MainWindow::saveSettings()
{
    AppSettings settings;

    settings.beginGroup("MainWindow");

    settings.setValue("MainWindowState", saveState(0));

    settings.setValue("size", size());
    settings.setValue("pos", pos());

    // save the screen number for the splashsreeen ...
    if(QApplication::desktop()->isVirtualDesktop())
        settings.setValue("screenNumber", QApplication::desktop()->screenNumber(pos()));
    else
        settings.setValue("screenNumber", QApplication::desktop()->screenNumber(this));

    settings.setValue("screenNumber", QApplication::desktop()->screenNumber(this));

    settings.beginGroup("Shortcuts");

    // Save shortcuts
    QList<QObject*> childrens;
    childrens= children();
    QListIterator<QObject*> iterator(childrens);
    while(iterator.hasNext())
    {
        QObject* object= iterator.next();
        QString classe= object->metaObject()->className();
        if(classe == "QAction")
        {
            QString text= object->objectName();
            QString shortcut= ((QAction*)object)->shortcut().toString();
            // if( !shortcut.isEmpty() )
            settings.setValue(text, shortcut);
        }
    }
    settings.endGroup();

    settings.endGroup();

    settings.beginGroup("Application");

    settings.setValue("splashscreenAtStartup", m_splashscreenAtStartup);
    settings.setValue("splashscreenTransparentBackground", m_splashscreenTransparentBackground);
    settings.setValue("startInTray", m_startInTray);
    settings.setValue("currentLanguage", m_currentLanguage);

    settings.setValue("lastSnapshotDirectory", NameManager::lastSnapshotDirectory);
    settings.setValue("autoCopyToClipboard", m_autoCopyToClipboard);
    settings.setValue("captureWindowWithoutDecoration", m_captureWindowWithDecoration);
    settings.setValue("playSound", m_playSound);
    settings.setValue("hideUIForSnapshot", m_hideUIForSnapshot);
    settings.setValue("startWithSystem", m_startWithSystem);

    settings.setValue("useGlobalHotKey", m_useGlobalHotKey);
    settings.setValue("globalHotkeyGrabScreen", m_globalHotkeyGrabScreen);
    settings.setValue("globalHotkeyGrabAllScreens", m_globalHotkeyGrabAllScreens);
    settings.setValue("globalHotkeyGrabWindow", m_globalHotkeyGrabWindow);
    settings.setValue("globalHotkeyGrabRegion", m_globalHotkeyGrabRegion);
    settings.setValue("globalHotkeyGrabFreehand", m_globalHotkeyGrabFreehand);

    settings.setValue("lastSnapshotName", NameManager::lastSnapshotName);
    settings.setValue("defaultOutputFormat", NameManager::defaultOutputFormat);

    settings.setValue("addDateTime", NameManager::addDateTime);
    settings.setValue("formatDateTime", NameManager::formatDateTime);

    settings.setValue("delay", m_delay);

    settings.setValue("grabMode", m_grabMode);
    settings.setValue("doAfterShot", m_doAfterShot);

    settings.setValue("scaleImage", m_scaleImage);
    settings.setValue("modeScaleSnapshot", m_modeScaleSnapshot);
    settings.setValue("scaleValue", m_scaleValue);
    settings.setValue("maxSize", m_maxSnapshotSize);

    settings.setValue("postEffect", m_postEffect);
    settings.setValue("rotateImage", m_rotateImage);
    settings.setValue("angle", m_angle);

    settings.setValue("defaultUploader", m_defaultUploader);

    settings.setValue("forceBgColor", m_forceBgColor);
    settings.setValue("bgColor", m_bgColor);

    settings.setValue("launchBrowserWhenDone", m_launchBrowserWhenDone);
    settings.setValue("showDialogBeforeDownload", m_showDialogBeforeDownload);
    settings.setValue("showDialogAfterDownload", m_showDialogAfterDownload);

    settings.setValue("grabWithMouseCursor", m_grabWithMouseCursor);

    settings.setValue("defaultActionSystrayName", m_defaultActionSystrayName);

    settings.setValue("systrayUserActionList", m_systrayUserActionList);

    settings.endGroup();

    settings.beginGroup("Proxy");

    settings.setValue("useProxy", m_proxiParams.useProxy);
    settings.setValue("proxyHostname", m_proxiParams.proxyHostname);
    settings.setValue("proxyPort", m_proxiParams.proxyPort);
    settings.setValue("useProxyAuthentification", m_proxiParams.useProxyAuthentification);
    settings.setValue("proxyUsername", m_proxiParams.proxyUsername);
    settings.setValue("proxyPassword", MiscFunctions::base64Encode(
                                           m_proxiParams.proxyPassword)); // in order to store unreadable password
    settings.setValue("showProxyPassword", m_proxiParams.showProxyPassword);

    settings.endGroup();

    settings.beginGroup("UrlToClipboard");

    settings.setValue("copyUploadedUrlToClipboard", m_copyUploadedUrlToClipboard);
    settings.setValue("clipboardUrlFormat", m_clipboardUrlFormat);
    settings.setValue("userUrlFormat", m_userUrlFormat);

    settings.endGroup();
}

void MainWindow::loadSettings()
{
    AppSettings settings;

    settings.beginGroup("MainWindow");

    restoreState(settings.value("MainWindowState").toByteArray(), 0);

    resize(settings.value("size", QSize(400, 400)).toSize());
    move(settings.value("pos", QPoint(200, 200)).toPoint());

    // check if restore position isn't outside the current screen definitions (additional screen has been unplugged)
    if(QApplication::desktop()->screenNumber(pos()) < 0)
        move(QPoint(200, 200));

    settings.beginGroup("Shortcuts");

    // Load shortcuts
    QList<QObject*> childrens;
    childrens= children();
    QListIterator<QObject*> iterator(childrens);
    while(iterator.hasNext())
    {
        QObject* object= iterator.next();
        QString classe= object->metaObject()->className();
        if(classe == "QAction")
        {
            QString text= object->objectName();
            QString shortcut= ((QAction*)object)->shortcut().toString();
            if(!text.isEmpty())
            {
                shortcut= settings.value(text, shortcut).toString();
                ((QAction*)object)->setShortcut(shortcut);

                // update the  tooltip
                if(!shortcut.isEmpty())
                {
                    // strip the (...) if needed
                    QString tooltip= ((QAction*)object)->toolTip().replace(QRegExp(" \\(.*\\)$"), "");
                    ((QAction*)object)->setToolTip(tooltip + " (" + shortcut + ")");
                }
            }
        }
    }
    settings.endGroup();

    settings.endGroup();

    settings.beginGroup("Application");

    m_splashscreenAtStartup= settings.value("splashscreenAtStartup", true).toBool();
    m_splashscreenTransparentBackground= settings.value("splashscreenTransparentBackground", true).toBool();

    m_startInTray= settings.value("startInTray", false).toBool();

    // start hidden => no message
    if(m_startInTray)
        m_firstCloseMessage= true;

    m_currentLanguage= settings.value("currentLanguage", "auto").toString();

    if(m_currentLanguage.isEmpty())
        m_currentLanguage= QLocale::system().name().left(2);

    NameManager::lastSnapshotDirectory
        = settings.value("lastSnapshotDirectory", QStandardPaths::writableLocation(QStandardPaths::PicturesLocation))
              .toString();
    ui->lineEditOutputPath->setText(NameManager::lastSnapshotDirectory);

    m_autoCopyToClipboard= settings.value("autoCopyToClipboard", false).toBool();
    m_captureWindowWithDecoration= settings.value("captureWindowWithoutDecoration", true).toBool();
    m_grabber->setCaptureDecoration(m_captureWindowWithDecoration);
    m_startWithSystem= settings.value("startWithSystem", false).toBool();

    m_playSound= settings.value("playSound", false).toBool();
    m_hideUIForSnapshot= settings.value("hideUIForSnapshot", true).toBool();

    m_useGlobalHotKey= settings.value("useGlobalHotKey", false).toBool();
    m_globalHotkeyGrabScreen= settings.value("globalHotkeyGrabScreen", "F5").toString();
    m_globalHotkeyGrabAllScreens= settings.value("globalHotkeyGrabAllScreens", "Ctrl+Alt+F5").toString();
    m_globalHotkeyGrabWindow= settings.value("globalHotkeyGrabWindow", "Ctrl+F5").toString();
    m_globalHotkeyGrabRegion= settings.value("globalHotkeyGrabRegion", "Alt+F5").toString();
    m_globalHotkeyGrabFreehand= settings.value("globalHotkeyGrabFreehand", "Shift+Alt+F5").toString();

    NameManager::lastSnapshotName= settings.value("lastSnapshotName", "snapshot").toString();

    NameManager::defaultOutputFormat= settings.value("defaultOutputFormat", "PNG").toString();

    NameManager::addDateTime= settings.value("addDateTime", false).toBool();
    NameManager::formatDateTime= settings.value("formatDateTime", "yyyy-MM-dd-hh-mm-ss").toString();

    m_delay= settings.value("delay", 0).toInt();
    ui->spinBoxDelay->setValue(m_delay);

    m_grabMode= settings.value("grabMode", 0).toInt();
    ui->comboBoxMode->setCurrentIndex(m_grabMode);
    m_grabber->setDefaultMode(m_grabMode);

    m_doAfterShot= settings.value("doAfterShot", 0).toInt();
    ui->comboBoxAfterShot->setCurrentIndex(m_doAfterShot);

    m_scaleImage= settings.value("scaleImage", false).toBool();
    m_modeScaleSnapshot= settings.value("modeScaleSnapshot", SCALE_BY_PERCENT).toInt();

    m_scaleValue= settings.value("scaleValue", 100).toInt();

    m_maxSnapshotSize= settings.value("maxSize", QSize(800, 600)).toSize();

    m_postEffect= settings.value("postEffect", PostEffect::EFFECT_NONE).toInt();
    m_rotateImage= settings.value("rotateImage", false).toBool();
    m_angle= settings.value("angle", false).toFloat();

    m_defaultUploader= settings.value("defaultUploader", -1).toInt();

    m_forceBgColor= settings.value("forceBgColor", false).toBool();
    m_bgColor= settings.value("bgColor", QColor(Qt::white)).value<QColor>();

    m_launchBrowserWhenDone= settings.value("launchBrowserWhenDone", false).toBool();
    m_showDialogBeforeDownload= settings.value("showDialogBeforeDownload", true).toBool();
    m_showDialogAfterDownload= settings.value("showDialogAfterDownload", false).toBool();

    m_grabWithMouseCursor= settings.value("grabWithMouseCursor", false).toBool();
    m_grabber->setAddMouseCursor(m_grabWithMouseCursor);

    m_defaultActionSystrayName= settings.value("defaultActionSystrayName", "actionShowUI").toString();

    m_systrayUserActionList
        = settings.value("systrayUserActionList", QStringList() << "actionAddAnnotations").toStringList();

    settings.endGroup();

    settings.beginGroup("Proxy");

    m_proxiParams.useProxy= settings.value("useProxy", false).toBool();
    m_proxiParams.proxyHostname= settings.value("proxyHostname", "").toString();
    m_proxiParams.proxyPort= settings.value("proxyPort", BaseUploader::defaultProxyPort).toInt();
    m_proxiParams.useProxyAuthentification= settings.value("useProxyAuthentification", false).toBool();
    m_proxiParams.proxyUsername= settings.value("proxyUsername", "").toString();
    m_proxiParams.proxyPassword
        = MiscFunctions::base64Decode(settings.value("proxyPassword", "").toString()); // decode encode password
    m_proxiParams.showProxyPassword= settings.value("showProxyPassword", false).toBool();

    settings.endGroup();

    settings.beginGroup("UrlToClipboard");

    m_copyUploadedUrlToClipboard= settings.value("copyUploadedUrlToClipboard", false).toBool();
    m_clipboardUrlFormat= settings.value("clipboardUrlFormat", BaseUploader::defaultUrlFormat).toInt();
    m_userUrlFormat= settings.value("userUrlFormat", BaseUploader::defaultUserFormat).toString();

    // correct a previously erroneous value
    m_userUrlFormat.replace("%url%", BaseUploader::defaultUserFormat);

    settings.endGroup();
}

void MainWindow::on_actionAbout_triggered()
{
    setupPreferences();

    m_about->show();
    m_about->showAbout();
}

void MainWindow::on_actionPreferences_triggered()
{
    setupPreferences();

    m_about->show();
    m_about->showPreferences();
}

//------------------------------------------------------------------------------------------
// preferences functions
//------------------------------------------------------------------------------------------

void MainWindow::setupPreferences()
{
    if(!m_about)
    {
        m_about= new AboutDialog(this);

        connect(m_about, SIGNAL(restart()), this, SLOT(restart()));
        connect(m_about, SIGNAL(accepted()), this, SLOT(setPreferences()));

        // global hotkey not available for OS/2 (also mouse pointer)
#ifdef Q_OS_PM // OS/2
        m_about->checkBoxUseSystemShortcuts->hide();
        m_about->checkBoxGrabWithMouseCursor->hide();
#endif
    }

    // update prefs if needed
    m_about->setCurrentLanguage(m_currentLanguage);

    // splashscreen params
    m_about->checkBoxSplashscreenAtStartup->setChecked(m_splashscreenAtStartup);
    m_about->checkBoxSplashscreenTransparentBackground->setChecked(m_splashscreenTransparentBackground);
    m_about->checkBoxStartInTray->setChecked(m_startInTray);

    m_about->checkBoxAutoCopyToClipboard->setChecked(m_autoCopyToClipboard);
    m_about->checkBoxCaptureWithDecoration->setChecked(m_captureWindowWithDecoration);
    m_about->checkBoxStartWithSystem->setChecked(m_startWithSystem);
    m_about->checkBoxPlaySound->setChecked(m_playSound);
    m_about->checkBoxHideInterface->setChecked(m_hideUIForSnapshot);

    m_about->checkBoxForceBackgroundColor->setChecked(m_forceBgColor);
    m_about->pushButtonBackgroundColor->setColor(m_bgColor);

    m_about->checkBoxLaunchBrowserWhenDone->setChecked(m_launchBrowserWhenDone);
    m_about->checkBoxShowDialogBeforeDownload->setChecked(m_showDialogBeforeDownload);
    m_about->checkBoxShowDialogAfterDownload->setChecked(m_showDialogAfterDownload);

    m_about->checkBoxGrabWithMouseCursor->setChecked(m_grabWithMouseCursor);

    m_about->spinBoxDelay->setValue(m_delay);

    // global hotkeys
    m_about->checkBoxUseSystemShortcuts->setChecked(m_useGlobalHotKey);

    m_about->lineEditDefaultName->setText(NameManager::lastSnapshotName);

    QList<QByteArray> formats= QImageReader::supportedImageFormats();
    m_about->comboBoxDefaultFormat->clear();
    for(int i= 0; i < formats.size(); ++i)
    {
        m_about->comboBoxDefaultFormat->addItem(formats[i].toUpper());
        if(NameManager::defaultOutputFormat == formats[i].toUpper())
            m_about->comboBoxDefaultFormat->setCurrentIndex(i);
    }

    m_about->checkBoxAddDate->setChecked(NameManager::addDateTime);
    m_about->lineEditDateTimeTemplate->setText(NameManager::formatDateTime);

    m_about->comboBoxPostEffect->clear();
    m_about->comboBoxPostEffect->addItem(tr("No post effect"), PostEffect::EFFECT_NONE);
    m_about->comboBoxPostEffect->addItem(tr("Simple drop shadow"), PostEffect::EFFECT_DROP_SHADOW);
    m_about->comboBoxPostEffect->addItem(tr("Drop shadow with borders"), PostEffect::EFFECT_POLAROID_DROP_SHADOW);
    m_about->comboBoxPostEffect->addItem(tr("Progressive opacity"), PostEffect::EFFECT_PROGRESSIVE_EDGE);
    m_about->comboBoxPostEffect->addItem(tr("Torn edge"), PostEffect::EFFECT_TORN_EDGE);

    m_about->comboBoxPostEffect->setCurrentIndex(m_postEffect);

    m_about->checkBoxRotateImage->setChecked(m_rotateImage);
    m_about->doubleSpinBoxAngle->setValue(m_angle);

    // scale data
    m_about->groupBoxScaleSnapshot->setChecked(m_scaleImage);

    m_about->spinBoxScalePercent->setValue(m_scaleValue);
    m_about->radioButtonPercent->setChecked(m_modeScaleSnapshot == SCALE_BY_PERCENT);

    m_about->lineEditScaleWidth->setText(QString::number(m_maxSnapshotSize.width()));
    m_about->lineEditScaleHeight->setText(QString::number(m_maxSnapshotSize.height()));
    m_about->radioButtonMaxSize->setChecked(m_modeScaleSnapshot == SCALE_BY_SIZE);

    // proxy settings
    m_about->groupBoxProxy->setChecked(m_proxiParams.useProxy);
    m_about->lineEditProxyHost->setText(m_proxiParams.proxyHostname);
    m_about->spinBoxProxyPort->setValue(m_proxiParams.proxyPort);
    m_about->groupBoxProxyAuthentification->setChecked(m_proxiParams.useProxyAuthentification);
    m_about->lineEditProxyUsername->setText(m_proxiParams.proxyUsername);
    m_about->lineEditProxyPassword->setText(m_proxiParams.proxyPassword);
    m_about->checkBoxShowPassword->setChecked(m_proxiParams.showProxyPassword);

    // Url to clipboard settings
    m_about->groupBoxCopyToClipboard->setChecked(m_copyUploadedUrlToClipboard);
    m_about->comboBoxClipboardFormat->setCurrentIndex(m_clipboardUrlFormat);
    m_about->lineEditUserFormat->setText(m_userUrlFormat);

    m_about->comboBoxUploaders->setCurrentIndex(m_defaultUploader);

    // prepare the shortcut edition
    m_about->clearShortcuts();
    m_about->addWidgetToShortcutEditor(this, tr("Main application"));
    m_about->addWidgetToShortcutEditor(m_snapshotEditor, tr("Annotation editor"));

    // prepare default systray action
    m_about->updateDefaultSystrayAction(this, m_defaultActionSystrayName);

    // prepare available function in system tray
    m_about->updateSystrayUserActionList(this, m_systrayUserActionList, m_systrayUserActionBlackList);
}

void MainWindow::setPreferences()
{
    if(!m_about)
        return;

    // splashscreen
    m_splashscreenAtStartup= m_about->checkBoxSplashscreenAtStartup->isChecked();
    m_splashscreenTransparentBackground= m_about->checkBoxSplashscreenTransparentBackground->isChecked();
    m_startInTray= m_about->checkBoxStartInTray->isChecked();

    // get the language
    m_currentLanguage= m_about->comboBoxLanguage->itemData(m_about->comboBoxLanguage->currentIndex()).toString();

    m_autoCopyToClipboard= m_about->checkBoxAutoCopyToClipboard->isChecked();
    m_captureWindowWithDecoration= m_about->checkBoxCaptureWithDecoration->isChecked();
    m_grabber->setCaptureDecoration(m_captureWindowWithDecoration);
    m_playSound= m_about->checkBoxPlaySound->isChecked();
    m_hideUIForSnapshot= m_about->checkBoxHideInterface->isChecked();

    m_forceBgColor= m_about->checkBoxForceBackgroundColor->isChecked();
    m_bgColor= m_about->pushButtonBackgroundColor->getColor();

    m_launchBrowserWhenDone= m_about->checkBoxLaunchBrowserWhenDone->isChecked();
    m_showDialogBeforeDownload= m_about->checkBoxShowDialogBeforeDownload->isChecked();
    m_showDialogAfterDownload= m_about->checkBoxShowDialogAfterDownload->isChecked();

    m_grabWithMouseCursor= m_about->checkBoxGrabWithMouseCursor->isChecked();
    m_grabber->setAddMouseCursor(m_grabWithMouseCursor);

    m_delay= m_about->spinBoxDelay->value();
    ui->spinBoxDelay->setValue(m_delay);
    m_grabber->setDelay(m_delay);

    m_startWithSystem= m_about->checkBoxStartWithSystem->isChecked();
    MiscFunctions::setRunOnStartup(m_startWithSystem);

    // shortcuts
    m_about->applyShortcuts();

    applyGlobalKeys(m_about->checkBoxUseSystemShortcuts->isChecked());

    // applyGlobalShortcut( m_about->checkBoxUseSystemShortcuts->isChecked() );

    NameManager::lastSnapshotName= m_about->lineEditDefaultName->text();
    NameManager::defaultOutputFormat= m_about->comboBoxDefaultFormat->currentText();

    NameManager::addDateTime= m_about->checkBoxAddDate->isChecked();
    NameManager::formatDateTime= m_about->lineEditDateTimeTemplate->text();

    m_postEffect= m_about->comboBoxPostEffect->currentIndex();

    m_rotateImage= m_about->checkBoxRotateImage->isChecked();
    m_angle= m_about->doubleSpinBoxAngle->value();

    // scale
    m_scaleImage= m_about->groupBoxScaleSnapshot->isChecked();

    if(m_about->radioButtonPercent->isChecked())
        m_modeScaleSnapshot= SCALE_BY_PERCENT;
    else
        m_modeScaleSnapshot= SCALE_BY_SIZE;

    m_scaleValue= m_about->spinBoxScalePercent->value();
    m_maxSnapshotSize= QSize(m_about->lineEditScaleWidth->text().toInt(), m_about->lineEditScaleHeight->text().toInt());

    // setup proxy settings
    m_proxiParams.useProxy= m_about->groupBoxProxy->isChecked();
    m_proxiParams.proxyHostname= m_about->lineEditProxyHost->text();
    m_proxiParams.proxyPort= m_about->spinBoxProxyPort->value();
    m_proxiParams.useProxyAuthentification= m_about->groupBoxProxyAuthentification->isChecked();
    m_proxiParams.proxyUsername= m_about->lineEditProxyUsername->text();
    m_proxiParams.proxyPassword= m_about->lineEditProxyPassword->text();
    m_proxiParams.showProxyPassword= m_about->checkBoxShowPassword->isChecked();

    // Url to clipboard settings
    m_copyUploadedUrlToClipboard= m_about->groupBoxCopyToClipboard->isChecked();
    m_clipboardUrlFormat= m_about->comboBoxClipboardFormat->currentIndex();
    m_userUrlFormat= m_about->lineEditUserFormat->text();

    m_defaultUploader= m_about->comboBoxUploaders->currentIndex();

    m_defaultActionSystrayName= m_about->getDefaultSystrayAction();
    m_systrayUserActionList= m_about->getSystrayUserActionList();

    createTrayIcon();

    // save the settings
    saveSettings();
}

void MainWindow::restart()
{
    QProcess::startDetached(qApp->arguments()[0], qApp->arguments().mid(1));
    QApplication::exit(0);
}

void MainWindow::on_pushButtonSnapshot_clicked()
{
    m_grabber->grab();
}

void MainWindow::on_actionGrabScreen_triggered()
{
    m_grabber->grab(GrabManager::GRAB_CURRENT_SCREEN);
}

void MainWindow::on_actionGrabAllScreens_triggered()
{
    m_grabber->grab(GrabManager::GRAB_ALL_SCREENS);
}

void MainWindow::on_actionGrabWindow_triggered()
{
    m_grabber->grab(GrabManager::GRAB_WINDOW_UNDER_CURSOR);
}

void MainWindow::on_actionGrabRegion_triggered()
{
    m_grabber->grab(GrabManager::GRAB_RECTANGULAR_REGION);
}

void MainWindow::on_actionGrabFreehandRegion_triggered()
{
    m_grabber->grab(GrabManager::GRAB_FREEHAND_REGION);
}

void MainWindow::processNewSnapshot(const QPixmap& pix)
{
    if(pix.isNull())
        return;

    m_blinkTimer.start(); // blink tray icon

    m_currPixmap= pix;
    if(m_scaleImage)
    {
        if(m_modeScaleSnapshot == SCALE_BY_PERCENT)
        {
            m_currPixmap
                = pix.scaled(pix.size() * (m_scaleValue / 100.0), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }
        else
        {
            if(pix.size().width() > m_maxSnapshotSize.width() || pix.size().height() > m_maxSnapshotSize.height())
                m_currPixmap= pix.scaled(m_maxSnapshotSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }
    }

    if(m_playSound)
        playSound(":/hotshots/camera.wav");

    // add post effect if needed
    if(m_postEffect != PostEffect::EFFECT_NONE)
        m_currPixmap= PostEffect::process(m_currPixmap, m_postEffect);

    if(m_rotateImage)
    {
        QTransform transform;
        QTransform trans= transform.rotate(m_angle);
        m_currPixmap= m_currPixmap.transformed(trans, Qt::SmoothTransformation);
    }

    // update dislayed image
    updateScreenshotLabel(m_currPixmap);

    // update size label
    updateApplicationIdentity();

    // m_labelSize->setText( tr("Snapshot size: %1x%2").arg( m_currPixmap.width() ).arg( m_currPixmap.height() ) );

    // copy to clipboard if needed
    if(m_autoCopyToClipboard)
        copyToClipboard();

    switch(m_doAfterShot)
    {
    case DO_AUTOSAVE:
    {
        saveSnapshot(m_currPixmap, NameManager::getProposedFilename());
        break;
    }
    case DO_SAVE:
    {
        on_actionSaveSnapshot_triggered();
        break;
    }
    case DO_EDITOR:
    {
        QTimer::singleShot(10, this, SLOT(on_actionAddAnnotations_triggered()));
        break;
    }
    default:
        break;
    }

    restoreDisplayState();
}

void MainWindow::updateScreenshotLabel(const QPixmap& pix)
{
    ui->labelPreview->setPixmap(pix.scaled(ui->labelPreview->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void MainWindow::resizeEvent(QResizeEvent* /* event */)
{
    // if not scrlabel pixmap
    if(!m_currPixmap.isNull())
    {
        updateScreenshotLabel(m_currPixmap);
    }
}

void MainWindow::on_actionUpload_triggered()
{
    uploadToWeb(m_currPixmap);
}

void MainWindow::uploadToWeb(const QPixmap& pix, const QString& preferredName)
{
    if(pix.isNull())
        return;

    if(m_defaultUploader < 0)
    {
        QMessageBox::warning(this, tr("Upload the image to the web"), tr("No default uploading web service defined!"));
        return;
    }

    QString saveName(preferredName);

    if(saveName.isEmpty())
        saveName= NameManager::getProposedFilename();

    const QList<BaseUploader*> list= UploaderManager::getUploaders();

    BaseUploader* up= list.value(m_defaultUploader);

    // Hohoho, it isn't very normal !!
    if(!up)
        return;

    // check uploader config
    if(!up->isValid())
    {
        QMessageBox::information(this, tr("Uploader %1").arg(up->getName()),
                                 tr("The default uploader (%1) does not appear correctly configured, no value given "
                                    "for required parameters!!")
                                     .arg(up->getName()));
        return;
    }

    QMessageBox msgBox;
    int ret= QMessageBox::Ok;

    if(m_showDialogBeforeDownload)
    {
        msgBox.setText(tr("Upload the image to the web"));
        msgBox.setInformativeText(
            tr("Upload image %1 to %2 service?").arg(QFileInfo(saveName).baseName()).arg(up->getName()));
        msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Cancel);
        msgBox.setIconPixmap(up->getLogo().scaled(QSize(32, 32), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        ret= msgBox.exec();
    }

    if(ret == QMessageBox::Ok)
    {
        // clear previous value
        ui->labelOuputUrl->setText("");
        if(m_forceBgColor)
            up->upload(MiscFunctions::applyAlpha(pix, m_bgColor).toImage(), QFileInfo(saveName).baseName());
        else
            up->upload(pix.toImage(), QFileInfo(saveName).baseName());

        // update snapshot number if needed
        if(!NameManager::addDateTime && preferredName.isEmpty())
            NameManager::currentSnapshotNum++;
    }
}

void MainWindow::on_actionSaveSnapshot_triggered()
{
    if(m_currPixmap.isNull())
        return;

    exportToDisk(m_currPixmap);
}

void MainWindow::on_actionPrint_triggered()
{
    exportToPrinter(m_currPixmap);
}

void MainWindow::exportToPrinter(const QPixmap& pix)
{
    if(pix.isNull())
        return;

    QPrinter printer;

    QPrintDialog* dialog= new QPrintDialog(&printer, this);
    dialog->setWindowTitle(tr("Print Document"));
    dialog->addEnabledOption(QAbstractPrintDialog::PrintSelection);

    if(dialog->exec() != QDialog::Accepted)
        return;

    QPainter painter;

    if(!painter.begin(&printer))
        return;

    QSize printSize= pix.size();
    QRect pageRect= printer.pageRect();
    printSize.scale(printer.pageRect().size(), Qt::KeepAspectRatio);
    QPoint printTopLeft((pageRect.width() - printSize.width()) / 2, (pageRect.height() - printSize.height()) / 2);
    QRect printRect(printTopLeft, printSize);

    painter.drawPixmap(printRect, pix, pix.rect());
    painter.end();
}

void MainWindow::exportToDisk(const QPixmap& pix, const QString& preferredName)
{
    QString saveName(preferredName);

    if(saveName.isEmpty())
    {
        QString selectedExtension= "Images png (*.png)";
        // ask for a filename ...
        saveName
            = QFileDialog::getSaveFileName(this, tr("Save File"), NameManager::getProposedFilename(),
                                           MiscFunctions::getAvailablesImageFormatsForWriting(), &selectedExtension);

        if(!selectedExtension.isEmpty())
        {
            selectedExtension.remove(0, 7);
            selectedExtension= selectedExtension.left(selectedExtension.indexOf('(') - 1);
            selectedExtension.prepend(".");
        }

        if(!saveName.isEmpty() && !saveName.endsWith(selectedExtension))
            saveName.append(selectedExtension);
    }

    if(!saveName.isEmpty() && saveSnapshot(pix, saveName))
    {
        NameManager::lastSnapshotDirectory= QFileInfo(saveName).absolutePath();
        ui->lineEditOutputPath->setText(NameManager::lastSnapshotDirectory);
    }
}

bool MainWindow::saveSnapshot(const QPixmap& pix, const QString& file)
{
    // try to extract base name if changed
    if(!QFileInfo(file).baseName().startsWith(NameManager::lastSnapshotName))
    {
        if(NameManager::addDateTime)
            NameManager::lastSnapshotName
                = QFileInfo(file).baseName().replace(QString("-%1").arg(NameManager::lastDateTime), "");
        else
            NameManager::lastSnapshotName
                = QFileInfo(file).baseName().replace(QString("-%1").arg(NameManager::currentSnapshotNum), "");
    }

    if(!NameManager::addDateTime)
        NameManager::currentSnapshotNum++;

    // extract extension
    QString ext= QFileInfo(file).suffix();

    bool ok= pix.save(file, ext.toUpper().toStdString().c_str());

    if(!ok)
    {
        QMessageBox::warning(this, tr("Save failed"), tr("Error saving file"));
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------
// tray icon functions
//-----------------------------------------------------------------------

void MainWindow::closeEvent(QCloseEvent* event)
{
    if(m_trayIcon && m_trayIcon->isVisible())
    {
        if(!m_firstCloseMessage)
        {
            QMessageBox::information(this, tr("Systray"),
                                     tr("The program will keep running in the "
                                        "system tray. To terminate the program, "
                                        "choose <b>Quit</b> in the context menu "
                                        "of the system tray entry."));
            m_firstCloseMessage= true;
        }

        hide();
        event->ignore();
    }
}

void MainWindow::createActionsSystemTray() {}

void MainWindow::on_actionShowUI_triggered()
{
    show();
    activateWindow();
}

void MainWindow::createTrayIcon()
{
    // tray menu
    if(!m_trayIconMenu)
    {
        m_trayIconMenu= new QMenu(this);
        m_trayIcon= new QSystemTrayIcon(this);
        m_trayIcon->setContextMenu(m_trayIconMenu);

        connect(m_trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this,
                SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));

        m_trayIcon->setIcon(QIcon(":/hotshots/logo_big.png"));
        m_trayIcon->show();
    }

    m_trayIconMenu->clear();

    m_trayIconMenu->addAction(ui->actionShowUI);
    // m_trayIconMenu->addAction(ui->actionAddAnnotations);
    m_trayIconMenu->addSeparator();
    // add user actions
    foreach(const QString& actName, m_systrayUserActionList)
    {
        QAction* act= getActionByName(actName);
        m_trayIconMenu->addAction(act);
    }

    m_trayIconMenu->addSeparator();
    m_trayIconMenu->addAction(ui->actionGrabScreen);
    m_trayIconMenu->addAction(ui->actionGrabAllScreens);
    m_trayIconMenu->addAction(ui->actionGrabWindow);
    m_trayIconMenu->addAction(ui->actionGrabRegion);
    m_trayIconMenu->addAction(ui->actionGrabFreehandRegion);
    m_trayIconMenu->addSeparator();
    m_trayIconMenu->addAction(ui->actionQuit);

    m_trayIconMenu->setTitle(m_appName);
    m_trayIconMenu->setIcon(QIcon(":/hotshots/logo_big.png"));
}

void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch(reason)
    {
    case QSystemTrayIcon::Trigger:
        qDebug() << "QSystemTrayIcon::Trigger";
        break;

    case QSystemTrayIcon::DoubleClick:
    {
        QAction* act= getActionByName(m_defaultActionSystrayName);
        act->trigger();
        // show ();
        // QWidget::setVisible(true);
        break;
    }
    case QSystemTrayIcon::MiddleClick:

        // showMessage();
        break;

    default:;
    }
}

QAction* MainWindow::getActionByName(const QString& actName)
{
    QList<QObject*> childrens= children();
    QListIterator<QObject*> iterator(childrens);
    while(iterator.hasNext())
    {
        QObject* object= iterator.next();
        if(object->objectName() == actName)
            return (QAction*)object;
    }
    return NULL;
}

void MainWindow::on_actionCopyToClipboard_triggered()
{
    copyToClipboard();
}

void MainWindow::copyToClipboard(const QPixmap& pix)
{
    if(m_currPixmap.isNull() && pix.isNull())
        return;

    // something send directly ?
    QPixmap newPix;
    if(pix.isNull())
        newPix= m_currPixmap;
    else
        newPix= pix;

    if(m_forceBgColor)
        QApplication::clipboard()->setPixmap(MiscFunctions::applyAlpha(newPix, m_bgColor), QClipboard::Clipboard);
    else
        QApplication::clipboard()->setPixmap(newPix, QClipboard::Clipboard);
}

void MainWindow::initGlobalKeys()
{
    // global hotkey not available for OS/2
#ifndef Q_OS_PM // OS/2
    // screen
    /*m_globShortCutGrabScreen = new QxtGlobalShortcut(this);
    connect( m_globShortCutGrabScreen, SIGNAL( activated() ), this, SLOT( on_actionGrabScreen_triggered() ) );

    // all screens
    m_globShortCutGrabAllScreens = new QxtGlobalShortcut(this);
    connect( m_globShortCutGrabAllScreens, SIGNAL( activated() ), this, SLOT( on_actionGrabAllScreens_triggered() ) );

    // window
    m_globShortCutGrabWindow = new QxtGlobalShortcut(this);
    connect( m_globShortCutGrabWindow, SIGNAL( activated() ), this, SLOT( on_actionGrabWindow_triggered() ) );

    // region
    m_globShortCutGrabRegion = new QxtGlobalShortcut(this);
    connect( m_globShortCutGrabRegion, SIGNAL( activated() ), this, SLOT( on_actionGrabRegion_triggered() ) );

    // freehand region
    m_globShortCutGrabFreehand = new QxtGlobalShortcut(this);
    connect( m_globShortCutGrabFreehand, SIGNAL( activated() ), this, SLOT( on_actionGrabFreehandRegion_triggered() ) );

    applyGlobalKeys(m_useGlobalHotKey);*/

#endif
}

void MainWindow::applyGlobalKeys(bool val)
{
    m_useGlobalHotKey= val;

    // global hotkeys not available for OS/2
#ifndef Q_OS_PM // OS/2

    // screen
    /*m_globShortCutGrabScreen->setShortcut( ui->actionGrabScreen->shortcut() );
    m_globShortCutGrabScreen->setEnabled(val);

    // all screens
    m_globShortCutGrabAllScreens->setShortcut( ui->actionGrabAllScreens->shortcut() );
    m_globShortCutGrabAllScreens->setEnabled(val);

    // window
    m_globShortCutGrabWindow->setShortcut( ui->actionGrabWindow->shortcut() );
    m_globShortCutGrabWindow->setEnabled(val);

    // region
    m_globShortCutGrabRegion->setShortcut( ui->actionGrabRegion->shortcut() );
    m_globShortCutGrabRegion->setEnabled(val);

    // freehand region
    m_globShortCutGrabFreehand->setShortcut( ui->actionGrabFreehandRegion->shortcut() );
    m_globShortCutGrabFreehand->setEnabled(val);*/

#endif
}

void MainWindow::on_spinBoxDelay_valueChanged(int val)
{
    m_delay= val;
    m_grabber->setDelay(m_delay);
}

void MainWindow::on_comboBoxAfterShot_activated(int val)
{
    m_doAfterShot= val;
}

void MainWindow::on_comboBoxMode_activated(int val)
{
    m_grabMode= val;
    m_grabber->setDefaultMode(m_grabMode);
}

void MainWindow::blink()
{
    static int i= 0;

    if(i++ % 2)
        m_trayIcon->setIcon(QIcon(":/hotshots/logo_idle.png"));
    else
        m_trayIcon->setIcon(QIcon(":/hotshots/hotshots.png"));

    if(i > blinkNumber)
    {
        m_blinkTimer.stop();
        m_trayIcon->setIcon(QIcon(":/hotshots/hotshots.png")); // restore icon just in case ...
        i= 0;
    }
}

void MainWindow::on_actionAddAnnotations_triggered()
{
    openEditor(m_currPixmap);
}

bool MainWindow::openEditor(const QString& file)
{
    initEditor();
    if(QFileInfo(file).suffix().toLower() == "hot")
    {
        m_snapshotEditor->show();
        return m_snapshotEditor->load(file);
    }
    else
        return openEditor(QPixmap(file));
}

bool MainWindow::openEditor(const QPixmap& pixmap)
{
    initEditor();
    m_snapshotEditor->show();
    m_snapshotEditor->load(pixmap);
    return true;
}

void MainWindow::initEditor()
{
    if(!m_snapshotEditor)
    {
        m_snapshotEditor= new EditorWidget();
        connect(m_snapshotEditor, SIGNAL(requestUpload(const QPixmap&, const QString&)), this,
                SLOT(uploadToWeb(const QPixmap&, const QString&)));
        connect(m_snapshotEditor, SIGNAL(requestExport(const QPixmap&, const QString&)), this,
                SLOT(exportToDisk(const QPixmap&, const QString&)));
        connect(m_snapshotEditor, SIGNAL(requestPrint(const QPixmap&)), this, SLOT(exportToPrinter(const QPixmap&)));
        connect(m_snapshotEditor, SIGNAL(requestClipboard(const QPixmap&)), this,
                SLOT(copyToClipboard(const QPixmap&)));
        connect(m_snapshotEditor, SIGNAL(defaultDirectoryChanged(const QString&)), this,
                SLOT(updateDefaultPath(const QString&)));
    }
}

void MainWindow::restoreDisplayState()
{
    if(!m_wasHidden)
        show();
}

void MainWindow::saveDisplayState()
{
    m_wasHidden= isHidden();
    if(m_hideUIForSnapshot)
        hide();
}

void MainWindow::on_actionHelp_triggered()
{
    QDesktopServices::openUrl(QUrl(QString(PACKAGE_ONLINE_HELP_URL)));
}

QString MainWindow::formatUrl(const QString& url, int type)
{
    QString formatted(url);
    switch(type)
    {
    case BaseUploader::FORMAT_IM:
    {
        formatted= QString("%1/%2").arg(QFileInfo(url).filePath()).arg(QFileInfo(url).baseName());
        break;
    }
    case BaseUploader::FORMAT_HTML:
    {
        formatted= QString("<img src=\"%1\" alt=\"\" />").arg(url);
        break;
    }
    case BaseUploader::FORMAT_BBCODE:
    {
        formatted= QString("[img]%1[/img]").arg(url);
        break;
    }
    case BaseUploader::FORMAT_USER:
    {
        formatted= m_userUrlFormat;
        formatted.replace("@url@", url);
        break;
    }
    case BaseUploader::FORMAT_NONE:
    default:
    {
        break;
    }
    }
    return formatted;
}

void MainWindow::readResultUrl(const QString& url)
{
    QString formattedUrl= formatUrl(url, m_clipboardUrlFormat);
    ui->labelOuputUrl->setText(formattedUrl);
    if(m_copyUploadedUrlToClipboard)
        QApplication::clipboard()->setText(formattedUrl);

    if(m_playSound)
        playSound(":/hotshots/beep.wav");

    if(m_launchBrowserWhenDone)
        QDesktopServices::openUrl(QUrl(url));

    if(m_showDialogAfterDownload)
    {
        QString text= tr("Snapshot will successfully uploaded to %1 ").arg(formattedUrl);
        if(m_copyUploadedUrlToClipboard)
            text+= tr("and url copied to clipboard");
        QMessageBox::information(this, m_appName, text);
    }
}

void MainWindow::playSound(const QString& file)
{
    /*if( !QSound::isAvailable() )
        return;*/

    QFile soundFile(file);
    QString tmpFile= MiscFunctions::getValidStorage() + QDir::separator() + QFileInfo(file).fileName();
    soundFile.copy(tmpFile);
    QSound::play(tmpFile);
}

void MainWindow::wakeUp(const QString& mess)
{
    if(!mess.isEmpty())
    {
        // a file ?
        if(QFileInfo(mess).exists())
            openEditor(mess);
    }
    else
    {
        LogHandler::getInstance()->reportWarning(tr("message from other instance."));
    }
    show();
}

void MainWindow::updateDefaultPath(const QString& p)
{
    ui->lineEditOutputPath->setText(p);
}
