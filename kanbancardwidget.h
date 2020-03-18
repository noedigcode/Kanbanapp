#ifndef KANBANCARD_H
#define KANBANCARD_H

#include <QWidget>
#include <QColorDialog>
#include <QMenu>
#include <QTimer>
#include "kanban.h"

namespace Ui {
class KanbanCardWidget;
}

class KanbanCardWidget : public QWidget
{
    Q_OBJECT

public:
    explicit KanbanCardWidget(QWidget *parent = 0);
    ~KanbanCardWidget();

    void setCard(Card* card);
    void setTextEditorFocus();

signals:
    void contentsChanged(); // Possibly in need of resize
    void clicked();

private slots:
    void setText(QString text);
    void setColor(QColor color);
    void onColorAction();
    void on_plainTextEdit_textChanged();

private:
    Ui::KanbanCardWidget *ui;
    Card* mCard = nullptr;
    void resizeToContent();
    QTimer timer;
    QAction colorAction;

    // QObject interface
public:
    bool eventFilter(QObject *watched, QEvent *event) override;
};

#endif // KANBANCARD_H
