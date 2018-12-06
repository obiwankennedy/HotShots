
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

#include <QtCore/QFileInfo>
#include <QtCore/QDateTime>
#include <QtCore/QTranslator>

#include <QMessageBox>
#include <QSound>
#include <QStyledItemDelegate>
#include <QHeaderView>
#include <QStandardItemModel>
#include <QAction>
//#include <QPlastiqueStyle>


#include "AboutDialog.h"
#include "MiscFunctions.h"
#include "AppSettings.h"
#include "LogHandler.h"
#include "UploaderManager.h"
#include "BaseUploader.h"

#include "qkeysequencewidget.h"

// Utilities
// -----------------------------------------------------------------------------

QAction* variantToAction( QVariant variant )
{
#if QT_POINTER_SIZE == 4
    return (QAction*)variant.toUInt();

#else
    return (QAction*)variant.toULongLong();

#endif
}

QVariant addressToVariant(void *it )
{
#if QT_POINTER_SIZE == 4
    return QVariant( reinterpret_cast<uint>(it) );

#else
    return QVariant( reinterpret_cast<qulonglong>(it) );

#endif
}

// Smart shortcut editor
class SmartShortcutEditDelegate : public QStyledItemDelegate
{
public:

    SmartShortcutEditDelegate(QObject* parent = 0) : QStyledItemDelegate(parent) {}

    virtual QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
    {
        Q_UNUSED(option);
        Q_UNUSED(index);
        QKeySequenceWidget *w = new QKeySequenceWidget(QKeySequence(index.data().toString()),parent);
        w->setClearButtonIcon(QIcon(":/hotshots/delete.png"));
        return w;
    }

    void setModelData ( QWidget * editor, QAbstractItemModel * model, const QModelIndex & index ) const
    {
        QKeySequenceWidget *w = qobject_cast<QKeySequenceWidget*>(editor);
        if (w)
            model->setData(index, w->keySequence().toString(), Qt::DisplayRole);
    }
};

// Dymmy editor to "block" the edition if needed
class NoEditDelegate : public QStyledItemDelegate
{
public:

    NoEditDelegate(QObject* parent = 0) : QStyledItemDelegate(parent) {}
    virtual QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
    {
        Q_UNUSED(parent);
        Q_UNUSED(option);
        Q_UNUSED(index);
        return NULL;
    }
};

// -----------------------------------------------------------------------------

AboutDialog::AboutDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUi(this);
    updateAbout();
}

AboutDialog::~AboutDialog()
{}

void AboutDialog::updateAbout()
{
    // setup credits ...
    updateCredits();

    // setup changelog ...
    updateChangelog();

    // setup image format list
    updateInfosImageFormats();

    // setup languages list
    updateInfosLanguages();

    // setup pref language
    updatePrefsLanguages();

    // setup uploaders
    updateUploaders();

    // create and connect the log handler
    connect( LogHandler::getInstance(),SIGNAL( newMessage(const QString &) ),this,SLOT( printToLog(const QString &) ) );
    LogHandler::getInstance()->setBufferization(false); // get all stored messages if exists

    labelRelease->setText( QString(PACKAGE_NAME) + " " + QString(PACKAGE_VERSION) );
    labelCheckRelease->setText( tr("<a href=\"%1\">Check for new release ...</a>").arg(PACKAGE_CHECK_RELEASE_URL) );

    // paypal
    pushButtonPaypal->setBusinessId(PAYPAL_BUSINESS_ID);
    pushButtonPaypal->setItemName( "Support TheHive" );
    pushButtonPaypal->setItemId( "TheHive-DONATION" );
    pushButtonPaypal->setCurrencyCode( "EUR" );

    // add validators
    QValidator *validator = new QIntValidator(1, 9999, this);
    lineEditScaleWidth->setValidator(validator);
    lineEditScaleHeight->setValidator(validator);

    // QSound ?
    /*if( !QSound::isAvailable() )
        checkBoxPlaySound->setEnabled(false);*/

    m_titles.insert( TAB_ABOUT,tr("About") );
    m_titles.insert( TAB_PREFERENCES,tr("Preferences") );
    m_titles.insert( TAB_INFOS,tr("Info") );
    m_titles.insert( TAB_LOG,tr("Log") );
}

void AboutDialog::updateUploaders()
{
    // update comboBoxUploaders
    const QList<BaseUploader *> &list = UploaderManager::getUploaders();
    foreach (BaseUploader * up, list)
    {
        comboBoxUploaders->addItem( up->getName(),qVariantFromValue( (void *) up ) ); // store directly pointer
    }

    // update listWidgetUploaders
    foreach (BaseUploader * up, list)
    {
        QListWidgetItem *item = new QListWidgetItem(up->getName(), listWidgetUploaders);
        item->setData( Qt::UserRole,qVariantFromValue( (void *) up ) );  // store directly pointer
    }
}

void AboutDialog::updateInfosImageFormats()
{
    // update format list

    // clear the current list
    listWidgetFormats->clear();

    QStringList formats = MiscFunctions::getAvailablesImageFormatsList();
    QMap<QString, QString> longFormats = MiscFunctions::getLongImageFormats();

    foreach (const QString &format, formats)
    {
        if ( longFormats.contains(format) )
            new QListWidgetItem(longFormats[format] + " (*." + format + ")", listWidgetFormats);
        else
            new QListWidgetItem(format, listWidgetFormats);
    }

    // Qt version
    labelQtVersion->setText( tr("Qt version %1").arg(QT_VERSION_STR) );
}

void AboutDialog::updateInfosLanguages()
{
    // get the current Language from settings
    AppSettings settings;
    settings.beginGroup("Application");
    m_currentLanguage = settings.value("currentLanguage","auto").toString();

    QString currentLang = m_currentLanguage;

    if ( currentLang.isEmpty()  || currentLang == "auto" )
        currentLang = QLocale::system().name().left(2);

    // update translation list
    const QMap<QString, QString> &mapLang = MiscFunctions::getAvailableLanguages();
    QStringList names = mapLang.keys();
    listWidgetTranslations->clear();
    foreach (const QString &lang, names)
    {
        QString text;

        if (mapLang[lang] == currentLang)
            text = lang + " *";
        else
            text = lang;

        QListWidgetItem *item = new QListWidgetItem(text, listWidgetTranslations);
        item->setData(Qt::UserRole,mapLang[lang]); // save the lang acronym
    }
}

void AboutDialog::on_listWidgetTranslations_itemClicked( QListWidgetItem * item)
{
    QString lang = item->data(Qt::UserRole).toString();
    QString file( MiscFunctions::getTranslationsPath("fr") );
    file += QString("/%1_%2.qm").arg( QString(PACKAGE_NAME).toLower() ).arg(lang);

    QTranslator translator;
    translator.load(file);
    labelTranslation->setText( translator.translate("","release of translation and translator name please","put your name here dear translator and the release of the translation file!!") );
}

void AboutDialog::updateCredits()
{
    fillTextedit("CREDITS.txt",textEditCredits);
}

void AboutDialog::updateChangelog()
{
    fillTextedit("Changelog.txt",textEditChangelog);
}

void AboutDialog::fillTextedit(const QString &file, QTextEdit *text)
{
    text->viewport ()->setAutoFillBackground(false);
    QStringList paths;
    QString filePath;
    paths << QApplication::applicationDirPath () + "/" + file;
    paths << QApplication::applicationDirPath () + "/../" + file;
    paths << QApplication::applicationDirPath () + "/../../" + file;
    paths << QApplication::applicationDirPath () + "/../../../" + file;
    paths << QApplication::applicationDirPath () + "/../Resources/" + file; // MaxOSX
    paths << "/usr/share/" + QString("%1").arg(PACKAGE_NAME).toLower() + "/" + file;
    paths << "/usr/local/share/" + QString("%1").arg(PACKAGE_NAME).toLower() + "/" + file;

    foreach (const QString &path, paths)
    {
        if ( QFileInfo(path).exists() )
        {
            filePath = path;
            break;
        }
    }

    // perhaps, not found !!
    if ( QFileInfo(filePath).exists() )
    {
        QFile f( filePath );
        LogHandler::getInstance()->reportDebug( QObject::tr("%1 found").arg( filePath ) );
        if( f.open( QIODevice::ReadOnly | QIODevice::Text ) )
        {
            text->setText( f.readAll() );
            f.close();
            return;
        }
    }
    text->setText( tr("The %1 file can't be found, sorry ...").arg(file) );
}

void AboutDialog::on_pushButtonClearLog_pressed()
{
    textBrowserLog->clear();
}

void AboutDialog::showAbout()
{
    tabWidget->setCurrentIndex (0);
}

void AboutDialog::showPreferences()
{
    tabWidget->setCurrentIndex (1);
}

void AboutDialog::updatePrefsLanguages()
{
    comboBoxLanguage->clear();

    // set detected language
    QString detectedLanguage = QLocale::system().name().left(2);
    labelDetectedLanguage->setText( tr("detected language: %1").arg(detectedLanguage) );

    // fill comboBox
    const QMap<QString, QString> &mapLang = MiscFunctions::getAvailableLanguages();
    QStringList names = mapLang.keys();
    comboBoxLanguage->addItem(tr("Automatic detection"), "auto");
    if (m_currentLanguage == "auto")
        comboBoxLanguage->setCurrentIndex(comboBoxLanguage->count() - 1);
    comboBoxLanguage->addItem(tr("Default (no use of translation files)"),"default");
    if (m_currentLanguage == "default")
        comboBoxLanguage->setCurrentIndex(comboBoxLanguage->count() - 1);
    foreach (const QString &lang, names)
    {
        comboBoxLanguage->addItem(lang,mapLang[lang]);
        if (mapLang[lang] == m_currentLanguage)
            comboBoxLanguage->setCurrentIndex(comboBoxLanguage->count() - 1);
    }
}

void AboutDialog::setCurrentLanguage(const QString &lang)
{
    comboBoxLanguage->setCurrentIndex( comboBoxLanguage->findData(lang) );
}

void AboutDialog::on_comboBoxLanguage_activated(int index)
{
    QString lang = comboBoxLanguage->itemData(index).toString();
    if (lang == m_currentLanguage)
        return;

    m_currentLanguage = lang;

    // check if recursive scan
    int ret = QMessageBox::question(this, PACKAGE_NAME,
                                    tr("The application need to restart in order to take into account new translation\n, Do you want to restart application now?"),
                                    QMessageBox::Yes
                                    | QMessageBox::No);

    if (ret == QMessageBox::Yes)
    {
        emit accepted ();
        emit restart();
    }
}

void AboutDialog::on_pushButtonResetPreferences_pressed()
{
    // check if recursive scan
    int ret = QMessageBox::question(this, PACKAGE_NAME,
                                    tr("Do you want to reset all the preferences to default value?\n If yes, the application will be relaunch"),
                                    QMessageBox::Yes
                                    | QMessageBox::No);

    if (ret == QMessageBox::Yes)
    {
        AppSettings settings;

        settings.beginGroup("Application");

        settings.setValue("resetConfig", true);
        settings.endGroup();

        emit accepted ();
        emit restart();
    }
}

void AboutDialog::on_lineEditDateTimeTemplate_textChanged(const QString &format)
{
    QString currentDateTime = QDateTime::currentDateTime().toString(format);
    labelExampleDateTime->setText(tr("Example: ") + currentDateTime);
}

//--------------------------------------------------------------------------------------
// LOG functions
//--------------------------------------------------------------------------------------

void AboutDialog::printToLog(const QString & mess)
{
    textBrowserLog->append(mess);
}

//--------------------------------------------------------------------------------------
// Uploaders functions
//--------------------------------------------------------------------------------------

void AboutDialog::on_checkBoxShowPassword_stateChanged ( int state )
{
    if (state == Qt::Checked)
        lineEditProxyPassword->setEchoMode (QLineEdit::Normal);
    else
        lineEditProxyPassword->setEchoMode (QLineEdit::Password);
}

void AboutDialog::on_comboBoxClipboardFormat_currentIndexChanged(int index)
{
    lineEditUserFormat->setEnabled(index == BaseUploader::FORMAT_USER);
}

void AboutDialog::on_comboBoxUploaders_currentIndexChanged(int index)
{
    if (index < 0)
        return;

    BaseUploader *up = static_cast<BaseUploader *>( comboBoxUploaders->itemData(index,Qt::UserRole).value<void *>() );
    labelUploader->setPixmap( up->getLogo().scaled(QSize(22,22),Qt::KeepAspectRatio,Qt::SmoothTransformation) );
}

void AboutDialog::on_listWidgetUploaders_currentItemChanged ( QListWidgetItem * current,  QListWidgetItem *prev)
{
    Q_UNUSED(prev);
    QLayout *layout = widgetParameters->layout();
    layout->itemAt(0)->widget()->hide();
    layout->removeItem( layout->itemAt(0) );

    BaseUploader *up = static_cast<BaseUploader *>( current->data(Qt::UserRole).value<void *>() );
    layout->addWidget( up->getSettingsInterface() );
    layout->itemAt(0)->widget()->show();
    layout->invalidate();
}

void AboutDialog::on_tabWidget_currentChanged ( int index )
{
    setWindowTitle( tabWidget->tabText(index) );
}

void AboutDialog::updateSystrayUserActionList(QWidget *w, const QStringList &currentList,const QStringList &backList)
{
    // update default systray action
    comboBoxSystrayUserActionList->clear();

    if (!w)
        return;

    QStandardItemModel *model = new QStandardItemModel(this);

    QList<QObject*> childrens = w->children();
    QListIterator<QObject*> iterator(childrens);
    int index = 0;
    while( iterator.hasNext() )
    {
        QObject *object = iterator.next();
        QString classe = object->metaObject()->className();
        if( classe == "QAction" )
        {
            QString text = ( (QAction *)object )->text().remove("&");
            if( !text.isEmpty() && !backList.contains(object->objectName()) )
            {
                QStandardItem* item = new QStandardItem(text);
                item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
                if (currentList.contains(object->objectName()))
                    item->setData(Qt::Checked, Qt::CheckStateRole);
                else
                    item->setData(Qt::Unchecked, Qt::CheckStateRole);

                item->setData(((QAction *)object)->icon(), Qt::DecorationRole);
                item->setData(addressToVariant(object),Qt::UserRole);

                model->setItem(index, 0, item);

                //comboBoxSystrayActionList->addItem(((QAction *)object)->icon(),text,addressToVariant(object));
                //comboBoxSystrayActionList->setItemData(index,true,Qt::ItemIsUserCheckable);
                //comboBoxSystrayActionList->setItemData(index,true,Qt::CheckStateRole);
                //if (defaultAction == object->objectName())
                //    comboBoxSystrayActionList->setCurrentIndex(comboBoxSystrayAction->count()-1);
                index++;
            }
        }
    }
    comboBoxSystrayUserActionList->setModel(model);
#ifdef Q_OS_UNIX
    //comboBoxSystrayUserActionList->setStyle(new QPlastiqueStyle); // for a problem of checkbox on GNOME !!!!
#endif
}

QStringList AboutDialog::getSystrayUserActionList() const
{
    QStringList res;
    QStandardItemModel * model = qobject_cast<QStandardItemModel*>(comboBoxSystrayUserActionList->model());
    if (!model)
        return res;

    // search for checked
    for (int i = 0; i < model->rowCount(); i++)
    {
        QStandardItem *	item = model->item(i);
        bool checked = item->data(Qt::CheckStateRole).toBool();
        if (checked)
        {
            QAction *act = variantToAction(item->data(Qt::UserRole));
            res << act->objectName();
        }
    }

    return res;
}

void AboutDialog::updateDefaultSystrayAction(QWidget *w,const QString &defaultAction)
{
    // update default systray action
    comboBoxSystrayAction->clear();

    if (!w)
        return;

    QList<QObject*> childrens = w->children();
    QListIterator<QObject*> iterator(childrens);
    while( iterator.hasNext() )
    {
        QObject *object = iterator.next();
        QString classe = object->metaObject()->className();
        if( classe == "QAction" )
        {
            QString text = ( (QAction *)object )->text().remove("&");
            if( !text.isEmpty() )
            {
                comboBoxSystrayAction->addItem(((QAction *)object)->icon(),text,addressToVariant(object));
                if (defaultAction == object->objectName())
                    comboBoxSystrayAction->setCurrentIndex(comboBoxSystrayAction->count()-1);
            }
        }
    }
}


QString AboutDialog::getDefaultSystrayAction() const
{
    int currentIndex = comboBoxSystrayAction->currentIndex();
    QAction *action = (QAction *)variantToAction( comboBoxSystrayAction->itemData(currentIndex, Qt::UserRole) );
    return action->objectName();
}

void AboutDialog::addWidgetToShortcutEditor(QWidget *w, const QString &desc)
{
    updateListShortcuts(w,desc);
}

void AboutDialog::updateListShortcuts(QWidget *main,const QString &desc)
{
    if (!main)
        return;

    QList<QObject*> childrens = main->children();

    //
    QTreeWidgetItem *rootItem = new QTreeWidgetItem(treeWidgetShortCut);
    rootItem->setData(0,Qt::DisplayRole,desc);
    treeWidgetShortCut->addTopLevelItem(rootItem);
    QListIterator<QObject*> iterator(childrens);
    while( iterator.hasNext() )
    {
        QObject *object = iterator.next();
        QString classe = object->metaObject()->className();
        if( classe == "QAction" )
        {
            QString text = ( (QAction *)object )->text().remove("&");
            if( !text.isEmpty() )
            {
                QString shortcut = ( (QAction *)object )->shortcut().toString();
                QTreeWidgetItem *newItem = new QTreeWidgetItem(rootItem);
                newItem->setData( ICON_COLUMN,Qt::DisplayRole, text );
                newItem->setData( ICON_COLUMN,Qt::UserRole, addressToVariant(object) );
                newItem->setFlags( Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable );
                newItem->setIcon( ICON_COLUMN, ( (QAction *)object )->icon() );
                newItem->setData( SHORTCUT_COLUMN, Qt::DisplayRole, shortcut );
                QString tooltip = ( (QAction *)object )->toolTip().replace(QRegExp(" \\(.*\\)$"),"");
                newItem->setData( DESC_COLUMN, Qt::DisplayRole, tooltip);
                rootItem->addChild(newItem);
            }
        }
    }
    treeWidgetShortCut->expandItem(rootItem); // expand
    treeWidgetShortCut->resizeColumnToContents(0); // resize to content
    treeWidgetShortCut->setItemDelegateForColumn( ICON_COLUMN, new NoEditDelegate(this) ); // block edition
    treeWidgetShortCut->setItemDelegateForColumn( DESC_COLUMN, new NoEditDelegate(this) );
    treeWidgetShortCut->setItemDelegateForColumn( SHORTCUT_COLUMN, new SmartShortcutEditDelegate(this) );
    
}

void AboutDialog::applyShortcuts()
{
    for (int i = 0; i < treeWidgetShortCut->topLevelItemCount(); i++)
    {
        QTreeWidgetItem *rootItem = treeWidgetShortCut->topLevelItem (i);
        for (int j = 0; j < rootItem->childCount (); j++)
        {
            QTreeWidgetItem *item = rootItem->child(j);
            QAction *action = (QAction *)variantToAction( item->data(ICON_COLUMN, Qt::UserRole) );
            QString shortcut = item->data(SHORTCUT_COLUMN, Qt::DisplayRole).toString();
            qDebug() << action->text() << shortcut;
            action->setShortcut( shortcut );

            // update tooltip if needed
            QString tooltip = item->data(DESC_COLUMN, Qt::DisplayRole).toString();
            if ( !shortcut.isEmpty() )
                tooltip += " (" + shortcut + ")";
            action->setToolTip(tooltip);
        }
    }
}

void AboutDialog::clearShortcuts()
{
    treeWidgetShortCut->clear();
}
