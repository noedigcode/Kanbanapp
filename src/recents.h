#ifndef RECENTS_H
#define RECENTS_H

#include "menuitem.h"

#include <QListWidget>
#include <QMenu>
#include <QObject>

class Recents : public QObject
{
    Q_OBJECT
public:
    QStringList items();
    void addItem(QString filename, int index = -1);
    void removeItem(QString filename);

signals:
    void itemAdded(QString filename, int index);
    void itemMoved(QString filename, int from, int to);
    void itemRemoved(QString filename);

private:
    QStringList mItems;
};

// =============================================================================

class RecentsMenuAdapter : public QObject
{
    Q_OBJECT
public:
    void init(QMenu* menu, Recents* recents);

signals:
    void openFileTriggered(QString filename);
    void openFolderButtonClicked(QString filename);

private:
    QMenu* mMenu = nullptr;
    Recents* mRecents = nullptr;
    QAction* menuEmptyAction = nullptr;
    QMap<QString, QAction*> mFilenameActionMap;
    void addOrRemoveMenuEmptyAction();
    QAction* createRecentsMenuAction(QString filename);

private slots:
    void onItemAdded(QString filename, int index);
    void onItemMoved(QString filename, int from, int to);
    void onItemRemoved(QString filename);
};

// =============================================================================

class RecentsListAdapter : public QObject
{
    Q_OBJECT
public:
    void init(QListWidget* listWidget, Recents* recents);

signals:
    void openFileTriggered(QString filename);
    void openFolderButtonClicked(QString filename);

private:
    QListWidget* mListWidget = nullptr;
    Recents* mRecents = nullptr;
    QMap<QString, QListWidgetItem*> mFilenameItemMap;
    MenuItem* createItemWidget(QString filename);

private slots:
    void onItemAdded(QString filename, int index);
    void onItemMoved(QString filename, int from, int to);
    void onItemRemoved(QString filename);
    void onListWidgetItemActivated(QListWidgetItem* item);
};

#endif // RECENTS_H
