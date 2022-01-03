#include "aboutdialog.h"
#include "ui_aboutdialog.h"

AboutDialog::AboutDialog(QString settingsPath, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    QString html = ui->textBrowser->toHtml();
    html.replace("%SETTINGS_PATH%", settingsPath);
    ui->textBrowser->setHtml(html);
}

AboutDialog::~AboutDialog()
{
    delete ui;
}

void AboutDialog::on_pushButton_Ok_clicked()
{
    this->hide();
}
