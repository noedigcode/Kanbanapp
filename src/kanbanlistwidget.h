#ifndef KANBANLISTWIDGET_H
#define KANBANLISTWIDGET_H

#include "kanban.h"
#include "kanbancardwidget.h"
#include "signalFunction.h"

#include <QListWidgetItem>
#include <QPushButton>
#include <QWidget>

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

    int currentRow();
    void ensureCardVisible(Card* card);
    void focusTitle();
    void focusCard(Card* card);

signals:
    void cardSelected(Card* card); // Selected card or null if no card selected
    void cardToClipboard(Card* card);
    void requestCardPaste(KanbanListWidget* listWidget);
    void focusReceived(KanbanListWidget* listWidget);

public slots:
    void addCard(Card* card, int index = -1);
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

    // QObject interface
public:
    bool eventFilter(QObject *watched, QEvent *event) override;
};

#endif // KANBANLISTWIDGET_H
