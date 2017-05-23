
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

#ifndef _DIALOGABOUT_H_
#define _DIALOGABOUT_H_

#include <QDialog>
#include "ui_AboutDialog.h"

class QListWidgetItem;

using namespace Ui;

class AboutDialog :
    public QDialog, public AboutDialogClass
{
    Q_OBJECT

public:

    enum
    {
        TAB_ABOUT = 0,
        TAB_PREFERENCES,
        TAB_INFOS,
        TAB_LOG
    };

    AboutDialog(QWidget *parent = 0);
    ~AboutDialog();

    void showAbout();
    void showPreferences();

    void setCurrentLanguage(const QString &);

    void addWidgetToShortcutEditor(QWidget *, const QString &desc);
    void clearShortcuts();
    void applyShortcuts();

    void updateDefaultSystrayAction(QWidget *, const QString &defaultAction);
    QString getDefaultSystrayAction() const;

    void updateSystrayUserActionList(QWidget *w,const QStringList &currentList, const QStringList &backList);
    QStringList getSystrayUserActionList() const;

public slots:

    void on_pushButtonClearLog_pressed();
    void on_comboBoxLanguage_activated(int index);
    void on_pushButtonResetPreferences_pressed();
    void on_lineEditDateTimeTemplate_textChanged(const QString &);
    void on_listWidgetTranslations_itemClicked( QListWidgetItem * item);
    void printToLog(const QString & mess);

    void on_checkBoxShowPassword_stateChanged ( int state );
    void on_comboBoxClipboardFormat_currentIndexChanged(int index);
    void on_comboBoxUploaders_currentIndexChanged(int index);
    void on_listWidgetUploaders_currentItemChanged ( QListWidgetItem *, QListWidgetItem *  );
    void on_tabWidget_currentChanged ( int index );

signals:

    void restart();

private:

    enum ColType
    {
        ICON_COLUMN = 0,
        SHORTCUT_COLUMN,
        DESC_COLUMN
    };

    void updateAbout();
    void updateCredits();
    void updateChangelog();
    void fillTextedit(const QString &file, QTextEdit *);
    void updatePrefsLanguages();
    void updateInfosLanguages();
    void updateInfosImageFormats();
    void updateUploaders();
    void updateListShortcuts(QWidget *main,const QString &desc);

    QString m_currentLanguage;
    QMap<int,QString> m_titles;
};

#endif // _DIALOGABOUT_H_
