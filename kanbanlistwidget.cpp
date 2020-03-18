#include "kanbanlistwidget.h"
#include "ui_kanbanlistwidget.h"

KanbanListWidget::KanbanListWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::KanbanListWidget)
{
    ui->setupUi(this);
    setTitleToolbuttonSelected(false);
}

KanbanListWidget::~KanbanListWidget()
{
    delete ui;
}

void KanbanListWidget::setList(KanbanList *list)
{
    mList = list;
    connect(list, &KanbanList::cardAdded,
            this, &KanbanListWidget::addCard);
    connect(list, &KanbanList::titleChanged,
            this, &KanbanListWidget::listTitleChanged);
    connect(list, &KanbanList::cardMoved,
            this, &KanbanListWidget::cardMoved);
    connect(list, &KanbanList::cardRemoved,
            this, &KanbanListWidget::removeCard);

    listTitleChanged(list->title());

    foreach (Card* card, list->cards()) {
        addCard(card);
    }
}

void KanbanListWidget::setTitleToolbuttonSelected(bool selected)
{
    if (selected) {
        ui->toolButton_listTitle->setIcon(QIcon(":/icons/checked_checkbox_48px.png"));
    } else {
        ui->toolButton_listTitle->setIcon(QIcon(":/icons/checked_checkbox_48px empty grey.png"));
    }
}

void KanbanListWidget::addCard(Card *card)
{
    addCardAtIndex(card, cardWidgetMap.count());
}

void KanbanListWidget::removeCard(Card *card)
{
    QListWidgetItem* item = cardListItemMap.value(card);
    KanbanCardWidget* w = cardWidgetMap.value(card);

    cardWidgetMap.remove(card);
    cardListItemMap.remove(card);

    delete w;
    delete item;
}

void KanbanListWidget::listTitleChanged(QString title)
{
    ui->lineEdit->setText(title);
}

void KanbanListWidget::cardMoved(Card *card, int newIndex)
{
    removeCard(card);
    addCardAtIndex(card, newIndex);
}

void KanbanListWidget::on_lineEdit_editingFinished()
{
    mList->setTitle(ui->lineEdit->text());
}

void KanbanListWidget::addCardAtIndex(Card *card, int index)
{
    KanbanCardWidget* widget = new KanbanCardWidget();
    cardWidgetMap.insert(card, widget);

    QListWidgetItem* item = new QListWidgetItem();
    cardListItemMap.insert(card, item);

    ui->listWidget->insertItem(index, item);
    ui->listWidget->setItemWidget(item, widget);

    connect(widget, &KanbanCardWidget::contentsChanged, [this, item, widget](){
        int height = widget->height();
        item->setSizeHint(QSize(50, height));
    });
    connect(widget, &KanbanCardWidget::clicked, [this, item](){
        ui->listWidget->setCurrentItem(item);
    });
    widget->setCard(card);

    ui->listWidget->setCurrentItem(item);
    widget->setTextEditorFocus();
}

void KanbanListWidget::on_listWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem* /*previous*/)
{
    Card* card = cardListItemMap.key(current, nullptr);
    emit cardSelected(card);
}

void KanbanListWidget::on_toolButton_up_clicked()
{
    QListWidgetItem* item = ui->listWidget->currentItem();
    if (!item) { return; }

    int newIndex = ui->listWidget->row(item) - 1;
    if (newIndex < 0) {
        newIndex = cardWidgetMap.count() - 1;
    }
    mList->moveCard(cardListItemMap.key(item), newIndex);
}

void KanbanListWidget::on_toolButton_down_clicked()
{
    QListWidgetItem* item = ui->listWidget->currentItem();
    if (!item) { return; }

    int newIndex = ui->listWidget->row(item) + 1;
    if (newIndex >= cardWidgetMap.count()) {
        newIndex = 0;
    }
    mList->moveCard(cardListItemMap.key(item), newIndex);
}

void KanbanListWidget::on_toolButton_Add_clicked()
{
    mList->addCard(new Card());
}

void KanbanListWidget::on_toolButton_cut_clicked()
{
    Card* card = cardListItemMap.key(ui->listWidget->currentItem(), nullptr);
    if (card) {
        mList->takeCard(card);
        emit cardToClipboard(card);
    }
}

void KanbanListWidget::on_toolButton_paste_clicked()
{
    emit requestCardPaste(this);
}

void KanbanListWidget::on_toolButton_listTitle_clicked()
{
    emit titleToolbuttonClicked(this);
}
