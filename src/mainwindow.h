#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "aboutdialog.h"
#include "kanban.h"
#include "kanbanboardwidget.h"
#include "menuitem.h"

#include <QCloseEvent>
#include <QFileDialog>
#include <QFileInfo>
#include <QMainWindow>
#include <QMessageBox>
#include <QSettings>
#include <QToolButton>
#include <QWidgetAction>

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
    const QString mAppname = "Kanbanapp";
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

    QMenu mRecentsMenu;
    QStringList mRecentFilenames;
    void recentsMenuFromSettings();
    void addRecentFile(QString filename);
    void saveRecentFileListToSettings();
    QAction* createRecentsMenuAction(QString filename);

    void updateButtonsEnabledBasedOnListSelection();

    // QWidget interface
protected:
    void closeEvent(QCloseEvent *event) override;
};

#endif // MAINWINDOW_H
