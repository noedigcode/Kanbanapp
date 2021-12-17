#include "kanbanboardwidget.h"
#include "ui_kanbanboardwidget.h"

KanbanBoardWidget::KanbanBoardWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::KanbanBoardWidget)
{
    ui->setupUi(this);

    // Prepare the board widget that will contain the lists.
    // A base widget contains a horizontal layout.
    // The layout initially contains an "empty board" label and a stretch spacer.
    // When the label is visible, it overrides the stretch spacer (stretch of 1).
    // When lists are added, the label is hidden (but remains in the layout) and
    // the stretch spacer will keep the lists aligned to the left if the base
    // while it isn't filled.

    layout = new QHBoxLayout();
    layout->setMargin(4);
    layout->setSpacing(4);

    QWidget* widget = new QWidget();
    widget->setLayout(layout);
    ui->scrollArea->setWidget(widget);

    mEmptyBoardLabel = new QLabel("Empty board - add a list to start", this);
    mEmptyBoardLabel->setAlignment(Qt::AlignCenter);
    layout->insertWidget(0, mEmptyBoardLabel, 1);

    layout->addStretch(0);
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

void KanbanBoardWidget::ensureListVisible(KanbanList *list)
{
    // Double-dose to ensure scroll is done after layout and other stuff.
    SignalFunction::call(this, [=](){
        SignalFunction::call(this, [=](){

            KanbanListWidget* w = map.value(list);
            if (!w) { return; }

            ui->scrollArea->ensureWidgetVisible(w);

        });
    });
}

void KanbanBoardWidget::focusListTitle(KanbanList *list)
{
    KanbanListWidget* w = map.value(list);
    if (!w) { return; }
    w->focusTitle();
}

void KanbanBoardWidget::addList(KanbanList *list)
{
    KanbanListWidget* w = new KanbanListWidget();

    connect(w, &KanbanListWidget::cardToClipboard,
            this, &KanbanBoardWidget::onListWidgetCardToClipboard);
    connect(w, &KanbanListWidget::requestCardPaste,
            this, &KanbanBoardWidget::onListWidgetRequestCardPaste);
    connect(w, &KanbanListWidget::focusReceived,
            this, &KanbanBoardWidget::onListWidgetFocusReceived);

    w->setList(list);
    map.insert(list, w);

    mEmptyBoardLabel->hide();

    // The emptyBoardLabel and stretch spacer always remain in the layout, so
    // lists have to be inserted to the left of them, thus count - 2.
    layout->insertWidget(layout->count() - 2, w);
}

void KanbanBoardWidget::moveList(KanbanList *list, int newIndex)
{
    KanbanListWidget* w = map.value(list, nullptr);
    if (!w) { return; }

    layout->removeWidget(w);
    layout->insertWidget(newIndex, w);

    ui->scrollArea->ensureWidgetVisible(w);
}

void KanbanBoardWidget::removeList(KanbanList *list)
{
    KanbanListWidget* w = map.value(list);
    if (selectedListWidget == w) {
        selectedListWidget = nullptr;
        emit selectedListChanged(nullptr);
    }
    map.remove(list);
    delete w;

    if (map.isEmpty()) {
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
        Card* card = mCardClipboard.takeLast();
        list->addCard(card, listWidget->currentRow() + 1);
        listWidget->ensureCardVisible(card);
        listWidget->focusCard(card);
    }
}

void KanbanBoardWidget::onListWidgetFocusReceived(KanbanListWidget *listWidget)
{
    if (selectedListWidget) {
        selectedListWidget->setTitleToolbuttonSelected(false);
    }
    bool changed = (selectedListWidget != listWidget);
    selectedListWidget = listWidget;
    selectedListWidget->setTitleToolbuttonSelected(true);

    if (changed) { emit selectedListChanged(selectedList()); }
}
