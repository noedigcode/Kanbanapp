#include "mainwindow.h"
#include <QApplication>
#include <QFile>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    w.show();

    if (a.arguments().count() > 1) {
        QString filename = a.arguments().at(1);
        // Check if filename is a link/shortcut
        QString target = QFile::symLinkTarget(filename);
        if (target.isEmpty()) {
            w.openFile(filename);
        } else {
            w.openFile(target);
        }
    }

    return a.exec();
}
