#include "recents.h"

#include <QWidgetAction>


QStringList Recents::items()
{
    return mItems;
}

void Recents::addItem(QString filename, int index)
{
    int existingIndex = mItems.indexOf(filename);
    if (existingIndex >= 0) {
        // Already contains filename. Move to specified index.
        mItems.move(existingIndex, index);
        int to = mItems.indexOf(filename);
        emit itemMoved(filename, existingIndex, to);
        return;
    }

    // If we are here, the list didn't already contain the filename
    if (index >= 0) {
        mItems.insert(index, filename);
    } else {
        mItems.append(filename);
    }
    int actualIndex = mItems.indexOf(filename);
    emit itemAdded(filename, actualIndex);
}

void Recents::removeItem(QString filename)
{
    if (!mItems.contains(filename)) { return; }
    mItems.removeAll(filename);
    emit itemRemoved(filename);
}

void RecentsMenuAdapter::init(QMenu* menu, Recents* recents)
{
    mMenu = menu;
    mRecents = recents;

    connect(mRecents, &Recents::itemAdded,
            this, &RecentsMenuAdapter::onItemAdded);
    connect(mRecents, &Recents::itemMoved,
            this, &RecentsMenuAdapter::onItemMoved);
    connect(mRecents, &Recents::itemRemoved,
            this, &RecentsMenuAdapter::onItemRemoved);

    // Fill menu with existing items
    foreach (QString filename, mRecents->items()) {
        onItemAdded(filename, -1);
    }
    addOrRemoveMenuEmptyAction();
}

void RecentsMenuAdapter::addOrRemoveMenuEmptyAction()
{
    if (mMenu->isEmpty()) {
        if (!menuEmptyAction) {
            menuEmptyAction = new QAction("No recent files");
            menuEmptyAction->setEnabled(false);
        }
        mMenu->insertAction(nullptr, menuEmptyAction);
    } else {
        mMenu->removeAction(menuEmptyAction);
    }
}

QAction* RecentsMenuAdapter::createRecentsMenuAction(QString filename)
{
    QWidgetAction* action = new QWidgetAction(this);
    MenuItem* item = new MenuItem(filename);
    action->setDefaultWidget(item);
    connect(action, &QWidgetAction::triggered, [=]()
            {
                emit openFileTriggered(filename);
            });
    connect(item, &MenuItem::removeButtonClicked, [=]()
            {
                mRecents->removeItem(filename);
            });
    connect(item, &MenuItem::openFolderButtonClicked, [=]()
            {
                emit openFolderButtonClicked(filename);
            });

    return action;
}

void RecentsMenuAdapter::onItemAdded(QString filename, int index)
{
    QAction* action = createRecentsMenuAction(filename);
    QAction* before = mMenu->actions().value(index);
    mMenu->insertAction(before, action); // Appends if before == nullptr
    addOrRemoveMenuEmptyAction();

    mFilenameActionMap.insert(filename, action);
}

void RecentsMenuAdapter::onItemMoved(QString /*filename*/, int from, int to)
{
    QAction* action = mMenu->actions().value(from);
    mMenu->removeAction(action);
    QAction* before = mMenu->actions().value(to);
    mMenu->insertAction(before, action);
}

void RecentsMenuAdapter::onItemRemoved(QString filename)
{
    QAction* action = mFilenameActionMap.take(filename);
    if (action) {
        mMenu->removeAction(action);
        addOrRemoveMenuEmptyAction();
    }
}

void RecentsListAdapter::init(QListWidget* listWidget, Recents* recents)
{
    mListWidget = listWidget;
    mRecents = recents;

    connect(mRecents, &Recents::itemAdded,
            this, &RecentsListAdapter::onItemAdded);
    connect(mRecents, &Recents::itemMoved,
            this, &RecentsListAdapter::onItemMoved);
    connect(mRecents, &Recents::itemRemoved,
            this, &RecentsListAdapter::onItemRemoved);

    connect(mListWidget, &QListWidget::itemActivated,
            this, &RecentsListAdapter::onListWidgetItemActivated);

    // Fill list with existing items
    foreach (QString filename, mRecents->items()) {
        onItemAdded(filename, -1);
    }
}

MenuItem* RecentsListAdapter::createItemWidget(QString filename)
{
    MenuItem* item = new MenuItem(filename);
    item->setBackgroundNotFilled();
    item->setHighlightEnabled(false);
    connect(item, &MenuItem::removeButtonClicked, [=]()
            {
                mRecents->removeItem(filename);
            });
    connect(item, &MenuItem::openFolderButtonClicked, [=]()
            {
                emit openFolderButtonClicked(filename);
            });

    return item;
}

void RecentsListAdapter::onItemAdded(QString filename, int index)
{
    MenuItem* widget = createItemWidget(filename);

    QListWidgetItem* item = new QListWidgetItem();
    mListWidget->insertItem(index, item);
    mListWidget->setItemWidget(item, widget);

    mFilenameItemMap.insert(filename, item);
}

void RecentsListAdapter::onItemMoved(QString filename, int /*from*/, int to)
{
    onItemRemoved(filename);
    onItemAdded(filename, to);
}

void RecentsListAdapter::onItemRemoved(QString filename)
{
    QListWidgetItem* item = mFilenameItemMap.value(filename);
    if (!item) { return; }
    mFilenameItemMap.remove(filename);
    delete item;
}

void RecentsListAdapter::onListWidgetItemActivated(QListWidgetItem* item)
{
    emit openFileTriggered(mFilenameItemMap.key(item));
}
