#include "kanbancardwidget.h"
#include "ui_kanbancardwidget.h"

KanbanCardWidget::KanbanCardWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::KanbanCardWidget)
{
    ui->setupUi(this);

    colorAction.setText("Card Color...");
    connect(&colorAction, &QAction::triggered,
            this, &KanbanCardWidget::onColorAction);

    connect(&timer, &QTimer::timeout, [this](){
        if (ui->plainTextEdit->isVisible()) {
            resizeToContent();
        }
    });
    timer.start(100);

    ui->plainTextEdit->viewport()->installEventFilter(this);
}

KanbanCardWidget::~KanbanCardWidget()
{
    delete ui;
}

void KanbanCardWidget::setCard(Card *card)
{
    mCard = card;
    connect(mCard, &Card::textChanged,
            this, &KanbanCardWidget::setText);
    connect(mCard, &Card::colorChanged,
            this, &KanbanCardWidget::setColor);

    setText(card->text());
    setColor(card->color());
    resizeToContent();
}

void KanbanCardWidget::setTextEditorFocus()
{
    ui->plainTextEdit->setFocus();
}

void KanbanCardWidget::setText(QString text)
{
    QString currentText = ui->plainTextEdit->toPlainText();
    if (currentText != text) {
        ui->plainTextEdit->setPlainText(text);
    }
}

void KanbanCardWidget::setColor(QColor color)
{
    ui->plainTextEdit->setStyleSheet(
                QString("QPlainTextEdit {background-color: %1;}")
                .arg(color.name()));
}

void KanbanCardWidget::onColorAction()
{
    QColor c = QColorDialog::getColor();
    if (c.isValid()) {
        mCard->setColor(c);
    }
}

void KanbanCardWidget::on_plainTextEdit_textChanged()
{
    QString text = ui->plainTextEdit->toPlainText();
    if (text != mCard->text()) {
        mCard->setText(text);
    }
    resizeToContent();
}

void KanbanCardWidget::resizeToContent()
{
    if (!ui->plainTextEdit->isVisible()) { return; }

    int hbefore = ui->plainTextEdit->height();
    int lineSpacing = ui->plainTextEdit->fontMetrics().lineSpacing();
    int docHeight = ui->plainTextEdit->document()->size().height();
    docHeight = docHeight*lineSpacing;
    int magic = lineSpacing;
    ui->plainTextEdit->setFixedHeight( docHeight + magic );
    int hdelta = ui->plainTextEdit->height() - hbefore;
    int currentHeight = height();
    resize(width(), currentHeight + hdelta);

    emit contentsChanged();
}

bool KanbanCardWidget::eventFilter(QObject* /*watched*/, QEvent *event)
{
    bool ret = false; // False to pass event on, true to consume event
    if (event->type() == QEvent::MouseButtonPress) {
        emit clicked();
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent->button() == Qt::RightButton) {
            ret = true;
            QMenu* menu = ui->plainTextEdit->createStandardContextMenu(
                        mouseEvent->pos());
            QAction* firstAction = menu->actions().at(0);
            menu->insertAction(firstAction, &colorAction);
            menu->insertSeparator(firstAction);
            menu->popup(ui->plainTextEdit->mapToGlobal(mouseEvent->pos()));
        }
    }
    return ret;
}
