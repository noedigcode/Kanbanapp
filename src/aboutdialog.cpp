#include "aboutdialog.h"
#include "ui_aboutdialog.h"

AboutDialog::AboutDialog(QString settingsPath, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    QString text = ui->label_settingsPath->text();
    text.replace("%SETTINGS_PATH%", settingsPath);
    ui->label_settingsPath->setText(text);

    text = ui->label_appInfo->text();
    text.replace("%APP_VERSION%", APP_VERSION);
    text.replace("%APP_YEAR_FROM%", APP_YEAR_FROM);
    text.replace("%APP_YEAR%", APP_YEAR);
    ui->label_appInfo->setText(text);

    QString changelog = "Could not load changelog";
    QFile f("://changelog");
    if (f.open(QIODevice::ReadOnly)) {
        changelog = f.readAll();
    }
    ui->plainTextEdit->setPlainText(changelog);
}

AboutDialog::~AboutDialog()
{
    delete ui;
}

void AboutDialog::on_pushButton_Ok_clicked()
{
    this->hide();
}
