#ifndef KANBANBOARDWIDGET_H
#define KANBANBOARDWIDGET_H

#include "kanban.h"
#include "kanbanlistwidget.h"
#include "signalFunction.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QWidget>

namespace Ui {
class KanbanBoardWidget;
}

class KanbanBoardWidget : public QWidget
{
    Q_OBJECT

public:
    explicit KanbanBoardWidget(QWidget *parent = 0);
    ~KanbanBoardWidget();

    void setBoard(Board* board);
    KanbanList* selectedList();
    void ensureListVisible(KanbanList* list);
    void focusListTitle(KanbanList* list);

signals:
    void selectedListChanged(KanbanList* list);

public slots:
    void addList(KanbanList* list);
    void moveList(KanbanList* list, int newIndex);
    void removeList(KanbanList* list);

private slots:
    void onListWidgetCardToClipboard(Card* card);
    void onListWidgetRequestCardPaste(KanbanListWidget* listWidget);
    void onListWidgetFocusReceived(KanbanListWidget* listWidget);

private:
    Ui::KanbanBoardWidget *ui;
    Board* mBoard = nullptr;
    QList<Card*> mCardClipboard;
    QHBoxLayout* layout = nullptr;
    QLabel* mEmptyBoardLabel = nullptr;

    QMap<KanbanList*, KanbanListWidget*> map;
    KanbanListWidget* selectedListWidget = nullptr;
};

#endif // KANBANBOARDWIDGET_H
