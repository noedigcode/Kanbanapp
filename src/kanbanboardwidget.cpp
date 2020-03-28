#include "kanbanboardwidget.h"
#include "ui_kanbanboardwidget.h"

KanbanBoardWidget::KanbanBoardWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::KanbanBoardWidget)
{
    ui->setupUi(this);

    layout = new QHBoxLayout();
    layout->setMargin(4);
    layout->setSpacing(4);

    QWidget* widget = new QWidget();
    widget->setLayout(layout);
    ui->scrollArea->setWidget(widget);

    mEmptyBoardLabel = new QLabel("Empty board - add a list to start", this);
    mEmptyBoardLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(mEmptyBoardLabel);
}

KanbanBoardWidget::~KanbanBoardWidget()
{
    delete ui;
}

void KanbanBoardWidget::setBoard(Board *board)
{
    mBoard = board;
    connect(board, &Board::listAdded,
            this, &KanbanBoardWidget::addList);
    connect(board, &Board::listMoved,
            this, &KanbanBoardWidget::moveList);
    connect(board, &Board::listRemoved,
            this, &KanbanBoardWidget::removeList);

    foreach (KanbanList* list, board->lists()) {
        addList(list);
    }
}

KanbanList *KanbanBoardWidget::selectedList()
{
    return map.key(selectedListWidget, nullptr);
}

void KanbanBoardWidget::addList(KanbanList *list)
{
    KanbanListWidget* w = new KanbanListWidget();

    connect(w, &KanbanListWidget::cardToClipboard,
            this, &KanbanBoardWidget::onListWidgetCardToClipboard);
    connect(w, &KanbanListWidget::requestCardPaste,
            this, &KanbanBoardWidget::onListWidgetRequestCardPaste);
    connect(w, &KanbanListWidget::titleToolbuttonClicked,
            this, &KanbanBoardWidget::onListWidgetTitleButtonClicked);

    w->setList(list);
    map.insert(list, w);

    layout->addWidget(w);
    layout->removeWidget(mEmptyBoardLabel);
    mEmptyBoardLabel->hide();
}

void KanbanBoardWidget::moveList(KanbanList *list, int newIndex)
{
    KanbanListWidget* w = map.value(list, nullptr);
    if (!w) { return; }

    layout->removeWidget(w);
    layout->insertWidget(newIndex, w);
}

void KanbanBoardWidget::removeList(KanbanList *list)
{
    KanbanListWidget* w = map.value(list);
    if (selectedListWidget == w) {
        selectedListWidget = nullptr;
    }
    map.remove(list);
    delete w;

    if (map.isEmpty()) {
        layout->addWidget(mEmptyBoardLabel);
        mEmptyBoardLabel->show();
    }
}

void KanbanBoardWidget::onListWidgetCardToClipboard(Card *card)
{
    mCardClipboard.append(card);
}

void KanbanBoardWidget::onListWidgetRequestCardPaste(KanbanListWidget *listWidget)
{
    if (mCardClipboard.count()) {
        KanbanList* list = map.key(listWidget);
        list->addCard(mCardClipboard.takeLast());
    }
}

void KanbanBoardWidget::onListWidgetTitleButtonClicked(KanbanListWidget *listWidget)
{
    if (selectedListWidget) {
        selectedListWidget->setTitleToolbuttonSelected(false);
    }
    selectedListWidget = listWidget;
    selectedListWidget->setTitleToolbuttonSelected(true);
}
