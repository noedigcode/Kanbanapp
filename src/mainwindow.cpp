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

    if (settings.value("maximized", false).toBool()) {
        this->setWindowState(Qt::WindowMaximized);
    }

    setWindowTitle(QString("%1 %2").arg(APP_NAME).arg(APP_VERSION));

    ui->groupBox_debug->setVisible(false);
    setCurrentFilename("");

    connect(ui->boardWidget, &KanbanBoardWidget::selectedListChanged,
            this, &MainWindow::onSelectedListChanged);

    ui->boardWidget->setBoard(&mKanbanBoard);

    setupRecents();
    recentsMenuFromSettings();
    setupOpenButtonMenu();
    setupSaveButtonMenu();

    updateButtonsEnabledStates();

    // Display board or recents page
    if (mCurrentFilename.isEmpty() && !mRecentsMenu.isEmpty()) {
        showRecentsPage();
    } else {
        showMainBoardPage();
    }
}

MainWindow::~MainWindow()
{
    bool fullscreen = this->windowState() & Qt::WindowMaximized;
    settings.setValue("maximized", fullscreen);

    delete ui;
}

void MainWindow::openFile(QString filename)
{
    mKanbanBoard.clear();
    setCurrentFilename(filename);

    QString parseErrorString;
    GidFile::Result r = mKanbanBoard.loadFromFile(filename, &parseErrorString);
    if (r.success) {
        mOriginalFileContents = mKanbanBoard.toJsonText();
        if (!parseErrorString.isEmpty()) {
            QMessageBox::critical(this, "Open",
                "A parsing error was encountered while opening the file: "
                + parseErrorString);
            // Create new file so the incorrectly loaded file doesn't get
            // overwritten accidentally
            forceNewBoard();
        }
    } else {
        QMessageBox::critical(this, "Open", "Could not open file: " + r.errorString);
        // Create new file so the incorrectly loaded file doesn't get
        // overwritten accidentally
        forceNewBoard();
    }

    showMainBoardPage();
}

void MainWindow::onSelectedListChanged(KanbanList* /*list*/)
{
    updateButtonsEnabledStates();
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
    GidFile::Result r = mKanbanBoard.saveToFile(filename);
    if (r.success) {
        mOriginalFileContents = mKanbanBoard.toJsonText();
    } else {
        QMessageBox::critical(this, "Save", "Could not save file: " + r.errorString);
    }
    return r.success;
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
    setWindowTitle(QString("%1 - %2 %3").arg(basename).arg(APP_NAME).arg(APP_VERSION));
}

bool MainWindow::canBoardBeClosed()
{
    bool close = true;
    if (isFileModified()) {
        QMessageBox mb;
        mb.setWindowTitle(APP_NAME);
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

void MainWindow::forceNewBoard()
{
    mKanbanBoard.clear();
    setCurrentFilename("");
    mOriginalFileContents = "";
}

void MainWindow::setupRecents()
{
    // Recents menu
    mRecentsMenuAdapter.init(&mRecentsMenu, &mRecents);
    connect(&mRecentsMenuAdapter, &RecentsMenuAdapter::openFileTriggered,
            this, [=](QString filename)
    {
        if (canBoardBeClosed()) {
            openFile(filename);
        }
    });
    connect(&mRecentsMenuAdapter, &RecentsMenuAdapter::openFolderButtonClicked,
            this, [=](QString filename)
    {
        QUrl url = QUrl::fromLocalFile(QFileInfo(filename).path());
        QDesktopServices::openUrl(url);
    });

    // Recents list
    mRecentsListAdapter.init(ui->listWidget_recents, &mRecents);
    connect(&mRecentsListAdapter, &RecentsListAdapter::openFileTriggered,
            this, [=](QString filename)
    {
        if (canBoardBeClosed()) {
            openFile(filename);
        }
    });
    connect(&mRecentsListAdapter, &RecentsListAdapter::openFolderButtonClicked,
            this, [=](QString filename)
    {
        QUrl url = QUrl::fromLocalFile(QFileInfo(filename).path());
        QDesktopServices::openUrl(url);
    });

    // Other connections
    connect(&mRecents, &Recents::itemAdded,
            this, [=](QString /*filename*/, int /*index*/)
    {
        saveRecentFileListToSettings();
    });

    connect(&mRecents, &Recents::itemMoved,
            this, [=](QString /*filename*/, int /*from*/, int /*to*/)
    {
        saveRecentFileListToSettings();
    });

    connect(&mRecents, &Recents::itemRemoved,
           this, [=](QString /*filename*/)
    {
        saveRecentFileListToSettings();
    });
}

void MainWindow::recentsMenuFromSettings()
{
    int count = settings.beginReadArray("recents");
    for (int i=0; i < count; i++) {
        settings.setArrayIndex(i);
        QString filename = settings.value("recentFilename").toString();
        mRecents.addItem(filename);
    }
    settings.endArray();
}

void MainWindow::addRecentFile(QString filename)
{
    filename = QDir::toNativeSeparators(filename);
    mRecents.addItem(filename, 0);
}

void MainWindow::saveRecentFileListToSettings()
{
    QStringList filenames = mRecents.items();

    settings.beginWriteArray("recents");
    for (int i=0; i < filenames.count(); i++) {
        settings.setArrayIndex(i);
        settings.setValue("recentFilename", filenames[i]);
    }
    settings.endArray();
}

void MainWindow::setupOpenButtonMenu()
{
    // Set up open button and recents menu

    QToolButton* tb = new QToolButton();
    tb->setDefaultAction(ui->actionOpen);
    tb->setMenu(&mRecentsMenu);
    tb->setIcon(ui->actionOpen->icon());
    tb->setPopupMode(QToolButton::MenuButtonPopup);
    ui->toolBar->insertWidget(ui->actionOpen, tb);
    ui->toolBar->removeAction(ui->actionOpen);
    ui->actionNo_recent_files->setEnabled(false);
}

void MainWindow::setupSaveButtonMenu()
{
    // Set up save button and save-as in submenu

    QToolButton* tbsave = new QToolButton();
    tbsave->setDefaultAction(ui->actionSave);
    QMenu* mnopen = new QMenu(this);
    mnopen->addAction(ui->actionSave_As);
    tbsave->setMenu(mnopen);
    tbsave->setIcon(ui->actionSave->icon());
    tbsave->setPopupMode(QToolButton::MenuButtonPopup);
    ui->toolBar->insertWidget(ui->actionSave, tbsave);
    ui->toolBar->removeAction(ui->actionSave);
}

void MainWindow::updateButtonsEnabledStates()
{
    bool disableAll = ui->stackedWidget->currentWidget() != ui->page_board;

    // Buttons depending on list selection
    bool enableListButtons = (ui->boardWidget->selectedList() != nullptr);
    enableListButtons &= !disableAll;

    ui->actionMove_List_Left->setEnabled(enableListButtons);
    ui->actionMove_List_Right->setEnabled(enableListButtons);
    ui->actionDelete_List->setEnabled(enableListButtons);

    // Other buttons
    ui->actionAdd_List->setEnabled(!disableAll);
}

void MainWindow::showRecentsPage()
{
    ui->stackedWidget->setCurrentWidget(ui->page_recents);
    updateButtonsEnabledStates();
}

void MainWindow::showMainBoardPage()
{
    ui->stackedWidget->setCurrentWidget(ui->page_board);
    updateButtonsEnabledStates();
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
    mb.setWindowTitle(APP_NAME);
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
        forceNewBoard();
        showMainBoardPage();
    }
}
