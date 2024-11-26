#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include "version.h"

#include <QDialog>

namespace Ui {
class AboutDialog;
}

class AboutDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AboutDialog(QString settingsPath, QWidget *parent = 0);
    ~AboutDialog();

private slots:
    void on_pushButton_Ok_clicked();

private:
    Ui::AboutDialog *ui;
};

#endif // ABOUTDIALOG_H
