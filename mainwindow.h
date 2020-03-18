#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include "aboutdialog.h"
#include "kanbanboardwidget.h"
#include "kanban.h"

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
    Board mKanbanBoard;
    AboutDialog* aboutDialog = nullptr;
    QString mCurrentFilename;
    QByteArray mOriginalFileContents;

    bool isFileModified();
    bool saveOrSaveAs();
    bool saveAs();
    bool writeToFile(QString filename);
    void setCurrentFilename(QString filename);
    bool canBoardBeClosed();

    // QWidget interface
protected:
    void closeEvent(QCloseEvent *event) override;
};

#endif // MAINWINDOW_H
