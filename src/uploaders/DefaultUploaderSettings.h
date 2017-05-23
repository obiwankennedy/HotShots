
#ifndef DEFAULTUPLOADERSETTINGS_H
#define DEFAULTUPLOADERSETTINGS_H

#include <QWidget>
#include "ui_DefaultUploaderSettings.h"

class DefaultUploaderSettings : public QWidget, public Ui::DefaultUploaderSettingsClass
{
    Q_OBJECT

public:

    DefaultUploaderSettings(QWidget *parent, const QPixmap &logo, const QString &url);
    ~DefaultUploaderSettings();
};

#endif // DEFAULTUPLOADERSETTINGS_H
