#include "kanbanlistwidget.h"
#include "ui_kanbanlistwidget.h"

#include "Utilities.h"

#include <QKeyEvent>

KanbanListWidget::KanbanListWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::KanbanListWidget)
{
    ui->setupUi(this);

    this->setMinimumWidth(
                Utilities::scaleWithPrimaryScreenScalingFactor(
                    this->minimumSize()).width());
    this->setMaximumWidth(
                Utilities::scaleWithPrimaryScreenScalingFactor(
                    this->maximumSize()).width());

    setTitleToolbuttonSelected(false);

    ui->lineEdit->installEventFilter(this);
    ui->listWidget->installEventFilter(this);
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
        ui->toolButton_listTitle->setIcon(QIcon(":/res/icons/checked_checkbox_48px.png"));
    } else {
        ui->toolButton_listTitle->setIcon(QIcon(":/res/icons/checked_checkbox_48px empty grey.png"));
    }
}

int KanbanListWidget::currentRow()
{
    return ui->listWidget->currentRow();
}

void KanbanListWidget::ensureCardVisible(Card *card)
{
    SignalFunction::call(this, [=](){

        QListWidgetItem* item = cardListItemMap.value(card);
        if (!item) { return; }

        ui->listWidget->scrollToItem(item);

    });
}

void KanbanListWidget::focusTitle()
{
    ui->lineEdit->setFocus();
    ui->lineEdit->selectAll();
}

void KanbanListWidget::focusCard(Card *card)
{
    SignalFunction::call(this, [=](){
        QListWidgetItem* item = cardListItemMap.value(card);
        if (!item) { return; }
        KanbanCardWidget* w = cardWidgetMap.value(card);
        if (!w) { return; }

        ui->listWidget->setCurrentItem(item);
        w->setFocus();
        w->setTextEditorFocus();
    });
}

/* Add card at specified index. If index is out of bounds, the card is added
 * at the bottom of the list. */
void KanbanListWidget::addCard(Card *card, int index)
{
    if ((index < 0) || (index >= ui->listWidget->count())) {
        index = ui->listWidget->count();
    }
    addCardAtIndex(card, index);
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
        emit focusReceived(this);
    });
    connect(widget, &KanbanCardWidget::splitLinesToCards, [=](QStringList lines){
        int index = mList->cardIndex(card);
        for (int i=0; i < lines.count(); i++) {
            Card* c = new Card();
            c->setText(lines.value(i));
            mList->addCard(c, index + i + 1);
        }
    });
    widget->setCard(card);

    widget->installEventFilter(this);
}

bool KanbanListWidget::eventFilter(QObject* watched, QEvent *event)
{
    if (event->type() == QEvent::FocusIn) {
        emit focusReceived(this);
    } else if (event->type() == QEvent::KeyPress) {

        if (cardWidgetMap.values().contains((KanbanCardWidget*)watched)) {
            QKeyEvent* k =  static_cast<QKeyEvent*>(event);
            if ( (k->key() == Qt::Key_Return) || (k->key() == Qt::Key_Enter)) {
                // Main Enter (Return) or Numlock Enter
                if (k->modifiers() & Qt::ControlModifier) {
                    // Ctrl+Enter was pressed. Create new card.
                    Card* card = new Card();
                    mList->addCard(card, ui->listWidget->currentRow() + 1);
                    ensureCardVisible(card);
                    focusCard(card);
                }
            }
        }
    }
    return false;
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
    Card* card = cardListItemMap.key(item);
    mList->moveCard(card, newIndex);
    ensureCardVisible(card);
    focusCard(card);
}

void KanbanListWidget::on_toolButton_down_clicked()
{
    QListWidgetItem* item = ui->listWidget->currentItem();
    if (!item) { return; }

    int newIndex = ui->listWidget->row(item) + 1;
    if (newIndex >= cardWidgetMap.count()) {
        newIndex = 0;
    }
    Card* card = cardListItemMap.key(item);
    mList->moveCard(card, newIndex);
    ensureCardVisible(card);
    focusCard(card);
}

void KanbanListWidget::on_toolButton_Add_clicked()
{
    Card* card = new Card();
    mList->addCard(card, ui->listWidget->currentRow()+1);
    ensureCardVisible(card);
    focusCard(card);
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
    emit focusReceived(this);
}
