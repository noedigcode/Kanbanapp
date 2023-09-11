#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDesktopServices>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    settings(QSettings::NativeFormat, QSettings::UserScope,
             "noedig.com", "Kanbanapp")
{
    ui->setupUi(this);
    ui->groupBox_debug->setVisible(false);
    setCurrentFilename("");

    connect(ui->boardWidget, &KanbanBoardWidget::selectedListChanged,
            this, &MainWindow::onSelectedListChanged);

    ui->boardWidget->setBoard(&mKanbanBoard);

    // Set up open button and recents menu
    QToolButton* tb = new QToolButton();
    tb->setDefaultAction(ui->actionOpen);
    recentsMenuFromSettings();
    tb->setMenu(&mRecentsMenu);
    tb->setIcon(ui->actionOpen->icon());
    tb->setPopupMode(QToolButton::MenuButtonPopup);
    ui->toolBar->insertWidget(ui->actionOpen, tb);
    ui->toolBar->removeAction(ui->actionOpen);
    ui->actionNo_recent_files->setEnabled(false);

    QToolButton* tbsave = new QToolButton();
    tbsave->setDefaultAction(ui->actionSave);
    QMenu* mnopen = new QMenu(this);
    mnopen->addAction(ui->actionSave_As);
    tbsave->setMenu(mnopen);
    tbsave->setIcon(ui->actionSave->icon());
    tbsave->setPopupMode(QToolButton::MenuButtonPopup);
    ui->toolBar->insertWidget(ui->actionSave, tbsave);
    ui->toolBar->removeAction(ui->actionSave);

    onSelectedListChanged(nullptr);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::openFile(QString filename)
{
    mKanbanBoard.clear();
    setCurrentFilename(filename);

    QString parseErrorString;
    bool ok = mKanbanBoard.loadFromFile(filename, &parseErrorString);
    if (ok) {
        mOriginalFileContents = mKanbanBoard.toJsonText();
        if (!parseErrorString.isEmpty()) {
            QMessageBox::critical(this, "Open",
                "A parsing error was encountered while opening the file: "
                + parseErrorString);
        }
    } else {
        QMessageBox::critical(this, "Open", "Could not open file.");
    }
}

void MainWindow::onSelectedListChanged(KanbanList *list)
{
    bool enable = (list != nullptr);

    ui->actionMove_List_Left->setEnabled(enable);
    ui->actionMove_List_Right->setEnabled(enable);
    ui->actionDelete_List->setEnabled(enable);
}

void MainWindow::on_actionOpen_triggered()
{
    if (!canBoardBeClosed()) { return; }

    QString filename = QFileDialog::getOpenFileName(this, QString(), QString(),
                                                    mFileFilter);
    if (filename.isEmpty()) { return; }

    openFile(filename);
}

void MainWindow::on_actionSave_triggered()
{
    saveOrSaveAs();
}

void MainWindow::on_pushButton_debug_clicked()
{
    ui->boardWidget_2->setBoard(&mKanbanBoard);
}

void MainWindow::on_actionDebug_triggered()
{
    ui->groupBox_debug->setVisible( !ui->groupBox_debug->isVisible() );
}

bool MainWindow::isFileModified()
{
    bool ret = false;
    if (mCurrentFilename.isEmpty()) {
        // No file was loaded. This is a new file.
        if (mKanbanBoard.lists().count()) {
            ret = true;
        }
    } else {
        // A file was loaded. Compare current and original JSON.
        QByteArray currentContent = mKanbanBoard.toJsonText();
        if (currentContent != mOriginalFileContents) {
            ret = true;
        }
    }
    return ret;
}

bool MainWindow::saveOrSaveAs()
{
    if (mCurrentFilename.isEmpty()) {
        return saveAs();
    } else {
        return writeToFile(mCurrentFilename);
    }
}

bool MainWindow::saveAs()
{
    QString filename = QFileDialog::getSaveFileName(this, QString(), QString(),
                                                    mFileFilter);
    if (filename.isEmpty()) { return false; }
    if (QFileInfo(filename).suffix().isEmpty()) {
        filename += mExtension;
    }

    if (writeToFile(filename)) {
        setCurrentFilename(filename);
        return true;
    } else {
        return false;
    }
}

bool MainWindow::writeToFile(QString filename)
{
    bool ok = mKanbanBoard.saveToFile(filename);
    if (ok) {
        mOriginalFileContents = mKanbanBoard.toJsonText();
    } else {
        QMessageBox::critical(this, "Save", "Could not save file.");
    }
    return ok;
}

void MainWindow::setCurrentFilename(QString filename)
{
    mCurrentFilename = filename;

    // Add to recents
    if (!filename.isEmpty()) {
        addRecentFile(filename);
    }

    // Set window title
    QString basename;
    if (filename.isEmpty()) {
        basename = "New";
    } else {
        basename = QFileInfo(filename).completeBaseName();
    }
    setWindowTitle(QString("%1 - %2").arg(basename, mAppname));
}

bool MainWindow::canBoardBeClosed()
{
    bool close = true;
    if (isFileModified()) {
        QMessageBox mb;
        mb.setWindowTitle(mAppname);
        mb.setIcon(QMessageBox::Question);
        mb.setWindowIcon(this->windowIcon());
        mb.setText("Do you want to save your changes?");
        mb.setStandardButtons(QMessageBox::Save | QMessageBox::Discard |
                              QMessageBox::Cancel);
        mb.setDefaultButton(QMessageBox::Save);
        int ret = mb.exec();
        if (ret == QMessageBox::Cancel) {
            close = false;
        } else if (ret == QMessageBox::Save) {
            close = saveOrSaveAs();
        }
    }
    return close;
}

void MainWindow::recentsMenuFromSettings()
{
    int count = settings.beginReadArray("recents");
    for (int i=0; i < count; i++) {
        settings.setArrayIndex(i);
        QString filename = settings.value("recentFilename").toString();
        mRecentFilenames.append(filename);
        mRecentsMenu.addAction(createRecentsMenuAction(filename));
    }
    settings.endArray();

    if (mRecentsMenu.isEmpty()) {
        mRecentsMenu.addAction(ui->actionNo_recent_files);
    }
}

void MainWindow::addRecentFile(QString filename)
{
    filename = QDir::toNativeSeparators(filename);

    if (mRecentFilenames.contains(filename)) {
        // Already contains file. Move to front.
        int index = mRecentFilenames.indexOf(filename);
        mRecentFilenames.move(index, 0);
        // Move in menu
        QList<QAction*> acts = mRecentsMenu.actions();
        QAction* a = acts[index];
        mRecentsMenu.removeAction(a);
        acts = mRecentsMenu.actions(); // Refresh
        mRecentsMenu.insertAction(acts.first(), a);

    } else {
        // Add new recent file.
        mRecentFilenames.insert(0, filename);
        mRecentsMenu.insertAction(mRecentsMenu.actions().first(),
                                  createRecentsMenuAction(filename));
        // Remove "No recent files" action
        mRecentsMenu.removeAction(ui->actionNo_recent_files);
    }

    saveRecentFileListToSettings();
}

void MainWindow::saveRecentFileListToSettings()
{
    settings.beginWriteArray("recents");
    for (int i=0; i < mRecentFilenames.count(); i++) {
        settings.setArrayIndex(i);
        settings.setValue("recentFilename", mRecentFilenames[i]);
    }
    settings.endArray();
}

QAction *MainWindow::createRecentsMenuAction(QString filename)
{
    QWidgetAction* action = new QWidgetAction(this);
    MenuItem* item = new MenuItem(filename);
    action->setDefaultWidget(item);
    connect(action, &QWidgetAction::triggered, [this, filename](){
        if (canBoardBeClosed()) {
            openFile(filename);
        }
    });
    connect(item, &MenuItem::removeButtonClicked, [this, action, filename](){
        // Remove recent file from list and menu
        mRecentFilenames.removeAll(filename);
        mRecentsMenu.removeAction(action);
        saveRecentFileListToSettings();
    });
    connect(item, &MenuItem::openFolderButtonClicked, [=](){
        // Open folder containing the file
        QUrl url = QUrl::fromLocalFile(QFileInfo(filename).path());
        QDesktopServices::openUrl(url);
    });

    return action;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (canBoardBeClosed()) {
        QMainWindow::closeEvent(event);
    } else {
        event->ignore();
    }
}

void MainWindow::on_actionSave_As_triggered()
{
    saveAs();
}

void MainWindow::on_actionAbout_triggered()
{
    if (!aboutDialog) {
        aboutDialog = new AboutDialog(settings.fileName(), this);
        aboutDialog->setWindowModality(Qt::ApplicationModal);
    }
    aboutDialog->show();
}

void MainWindow::on_actionAdd_List_triggered()
{
    KanbanList* list = new KanbanList();
    list->setTitle(QString("New List %1").arg(mKanbanBoard.lists().count()+1));
    mKanbanBoard.addList(list);

    ui->boardWidget->ensureListVisible(list);
    ui->boardWidget->focusListTitle(list);
}

void MainWindow::on_actionMove_List_Left_triggered()
{
    KanbanList* selectedList = ui->boardWidget->selectedList();
    if (!selectedList) { return; }

    QList<KanbanList*> lists = mKanbanBoard.lists();
    int index = lists.indexOf(selectedList);
    index -= 1;
    if (index < 0) { index = lists.count() - 1; }
    mKanbanBoard.moveList(selectedList, index);

    ui->boardWidget->ensureListVisible(selectedList);
}

void MainWindow::on_actionMove_List_Right_triggered()
{
    KanbanList* selectedList = ui->boardWidget->selectedList();
    if (!selectedList) { return; }

    QList<KanbanList*> lists = mKanbanBoard.lists();
    int index = lists.indexOf(selectedList);
    index += 1;
    if (index >= lists.count()) { index = 0; }
    mKanbanBoard.moveList(selectedList, index);

    ui->boardWidget->ensureListVisible(selectedList);
}

void MainWindow::on_actionDelete_List_triggered()
{
    KanbanList* selectedList = ui->boardWidget->selectedList();
    if (!selectedList) { return; }

    QMessageBox mb;
    mb.setWindowTitle(mAppname);
    mb.setIcon(QMessageBox::Question);
    mb.setWindowIcon(this->windowIcon());
    mb.setText("Are you sure you want to delete list '" +
               selectedList->title() + "'?");
    mb.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    mb.setDefaultButton(QMessageBox::No);
    int ret = mb.exec();
    if (ret == QMessageBox::Yes) {
        mKanbanBoard.removeList(selectedList);
    }
}

void MainWindow::on_actionNew_Board_triggered()
{
    if (canBoardBeClosed()) {
        mKanbanBoard.clear();
        setCurrentFilename("");
        mOriginalFileContents = "";
    }
}
