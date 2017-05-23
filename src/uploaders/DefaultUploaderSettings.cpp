
#include "DefaultUploaderSettings.h"

DefaultUploaderSettings::DefaultUploaderSettings(QWidget *parent,const QPixmap &logo, const QString &url)
    : QWidget(parent)
{
    setupUi(this);
    labelPixmap->setPixmap(logo);
    labelBaseURL->setText( QString("<a href=\"%1\">%1</a>").arg(url) );
}

DefaultUploaderSettings::~DefaultUploaderSettings()
{
}
