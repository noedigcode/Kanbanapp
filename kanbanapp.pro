
QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = kanbanapp
TEMPLATE = app
CONFIG += C++11

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += src

SOURCES += \
        src/main.cpp \
        src/mainwindow.cpp \
        src/kanbanlistwidget.cpp \
        src/kanban.cpp \
        src/kanbancardwidget.cpp \
        src/kanbanboardwidget.cpp \
        src/aboutdialog.cpp \
        src/menuitem.cpp

HEADERS += \
        src/mainwindow.h \
        src/kanbanlistwidget.h \
        src/kanban.h \
        src/kanbancardwidget.h \
        src/kanbanboardwidget.h \
        src/aboutdialog.h \
        src/menuitem.h

FORMS += \
        src/mainwindow.ui \
        src/kanbanlistwidget.ui \
        src/kanbancardwidget.ui \
        src/kanbanboardwidget.ui \
        src/aboutdialog.ui

RESOURCES += res/icons.qrc

# Windows icon
win32:RC_FILE += res/windowsicon.rc

