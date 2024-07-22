#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "aboutdialog.h"
#include "kanban.h"
#include "kanbanboardwidget.h"
#include "menuitem.h"
#include "signalFunction.h"
#include "version.h"

#include <QCloseEvent>
#include <QFileDialog>
#include <QFileInfo>
#include <QMainWindow>
#include <QMessageBox>
#include <QSettings>
#include <QToolButton>
#include <QWidgetAction>

// =============================================================================

class Recents : public QObject
{
    Q_OBJECT
public:
    QStringList items()
    {
        return mItems;
    }

    void addItem(QString filename, int index = -1)
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

    void removeItem(QString filename)
    {
        if (!mItems.contains(filename)) { return; }
        mItems.removeAll(filename);
        emit itemRemoved(filename);
    }

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
    void init(QMenu* menu, Recents* recents)
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

signals:
    void openFileTriggered(QString filename);
    void openFolderButtonClicked(QString filename);

private:
    QMenu* mMenu = nullptr;
    Recents* mRecents = nullptr;
    QAction* menuEmptyAction = nullptr;

    QMap<QString, QAction*> mFilenameActionMap;

    void addOrRemoveMenuEmptyAction()
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

    QAction* createRecentsMenuAction(QString filename)
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

private slots:
    void onItemAdded(QString filename, int index)
    {
        QAction* action = createRecentsMenuAction(filename);
        QAction* before = mMenu->actions().value(index);
        mMenu->insertAction(before, action); // Appends if before == nullptr
        addOrRemoveMenuEmptyAction();

        mFilenameActionMap.insert(filename, action);
    }

    void onItemMoved(QString /*filename*/, int from, int to)
    {
        QAction* action = mMenu->actions().value(from);
        mMenu->removeAction(action);
        QAction* before = mMenu->actions().value(to);
        mMenu->insertAction(before, action);
    }

    void onItemRemoved(QString filename)
    {
        QAction* action = mFilenameActionMap.take(filename);
        if (action) {
            mMenu->removeAction(action);
            addOrRemoveMenuEmptyAction();
        }
    }
};

// =============================================================================

class RecentsListAdapter : public QObject
{
    Q_OBJECT
public:
    void init(QListWidget* listWidget, Recents* recents)
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

signals:
    void openFileTriggered(QString filename);
    void openFolderButtonClicked(QString filename);

private:
    QListWidget* mListWidget = nullptr;
    Recents* mRecents = nullptr;

    QMap<QString, QListWidgetItem*> mFilenameItemMap;

    MenuItem* createItemWidget(QString filename)
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

private slots:
    void onItemAdded(QString filename, int index)
    {
        MenuItem* widget = createItemWidget(filename);

        QListWidgetItem* item = new QListWidgetItem();
        mListWidget->insertItem(index, item);
        mListWidget->setItemWidget(item, widget);

        mFilenameItemMap.insert(filename, item);
    }

    void onItemMoved(QString filename, int /*from*/, int to)
    {
        onItemRemoved(filename);
        onItemAdded(filename, to);
    }

    void onItemRemoved(QString filename)
    {
        QListWidgetItem* item = mFilenameItemMap.value(filename);
        if (!item) { return; }
        mFilenameItemMap.remove(filename);
        delete item;
    }

    void onListWidgetItemActivated(QListWidgetItem* item)
    {
        emit openFileTriggered(mFilenameItemMap.key(item));
    }
};

// =============================================================================

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void openFile(QString filename);

private slots:
    void onSelectedListChanged(KanbanList* list);
    void on_actionOpen_triggered();
    void on_actionSave_triggered();
    void on_pushButton_debug_clicked();
    void on_actionDebug_triggered();
    void on_actionSave_As_triggered();
    void on_actionAbout_triggered();
    void on_actionAdd_List_triggered();
    void on_actionMove_List_Left_triggered();
    void on_actionMove_List_Right_triggered();
    void on_actionDelete_List_triggered();
    void on_actionNew_Board_triggered();

private:
    Ui::MainWindow *ui;
    const QString mExtension = ".kanban";
    const QString mFileFilter = "Kanbanapp (*.kanban);; All files (*)";
    Board mKanbanBoard;
    AboutDialog* aboutDialog = nullptr;
    QString mCurrentFilename;
    QByteArray mOriginalFileContents;
    QSettings settings;

    bool isFileModified();
    bool saveOrSaveAs();
    bool saveAs();
    bool writeToFile(QString filename);
    void setCurrentFilename(QString filename);
    bool canBoardBeClosed();
    void forceNewBoard();

    Recents mRecents;
    void setupRecents();

    QMenu mRecentsMenu;
    RecentsMenuAdapter mRecentsMenuAdapter;

    RecentsListAdapter mRecentsListAdapter;

    void recentsMenuFromSettings();
    void addRecentFile(QString filename);
    void saveRecentFileListToSettings();

    void setupOpenButtonMenu();
    void setupSaveButtonMenu();

    void updateButtonsEnabledStates();

    void showRecentsPage();
    void showMainBoardPage();

protected:
    void closeEvent(QCloseEvent *event) override;
};

#endif // MAINWINDOW_H
