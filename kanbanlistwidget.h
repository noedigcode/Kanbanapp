#ifndef KANBANLISTWIDGET_H
#define KANBANLISTWIDGET_H

#include <QWidget>
#include <QListWidgetItem>
#include <QPushButton>
#include "kanban.h"
#include "kanbancardwidget.h"

namespace Ui {
class KanbanListWidget;
}

class KanbanListWidget : public QWidget
{
    Q_OBJECT

public:
    explicit KanbanListWidget(QWidget *parent = 0);
    ~KanbanListWidget();

    void setList(KanbanList* list);
    void setTitleToolbuttonSelected(bool selected);

signals:
    void cardSelected(Card* card); // Selected card or null if no card selected
    void cardToClipboard(Card* card);
    void requestCardPaste(KanbanListWidget* listWidget);
    void titleToolbuttonClicked(KanbanListWidget* listWidget);

public slots:
    void addCard(Card* card);
    void removeCard(Card* card);

private slots:
    void listTitleChanged(QString title);
    void cardMoved(Card* card, int newIndex);
    void on_lineEdit_editingFinished();

    void on_listWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

    void on_toolButton_up_clicked();

    void on_toolButton_down_clicked();

    void on_toolButton_Add_clicked();

    void on_toolButton_cut_clicked();

    void on_toolButton_paste_clicked();

    void on_toolButton_listTitle_clicked();

private:
    Ui::KanbanListWidget *ui;
    KanbanList* mList = nullptr;
    QMap<Card*, KanbanCardWidget*> cardWidgetMap;
    QMap<Card*, QListWidgetItem*> cardListItemMap;

    void addCardAtIndex(Card* card, int index);
};

#endif // KANBANLISTWIDGET_H
