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
        QString toOpen = filename;
        QString target = QFile::symLinkTarget(filename);
        if (!target.isEmpty()) { toOpen = target; }
        // Execute openFile() on the event loop as a queued connection to ensure
        // it gets called after the mainwindow has been setup and shown.
        // Otherwise, the cards potentially don't resize correctly.
        MainWindow* wptr = &w;
        QMetaObject::invokeMethod(wptr, [=]()
        {
            wptr->openFile(toOpen);
        }, Qt::QueuedConnection);
    }

    return a.exec();
}
