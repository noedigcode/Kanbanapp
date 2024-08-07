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

    resetColorAction.setText("Reset Card Color");
    connect(&resetColorAction, &QAction::triggered,
            this, &KanbanCardWidget::onResetColorAction);

    splitLinesAction.setText("Split Lines to Cards");
    connect(&splitLinesAction, &QAction::triggered,
            this, &KanbanCardWidget::onSplitLinesAction);

    ui->plainTextEdit->viewport()->installEventFilter(this);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
    ui->plainTextEdit->setTabStopDistance(40);
#else
    ui->plainTextEdit->setTabStopWidth(40);
#endif
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
    if (!color.isValid()) {
        color = QGuiApplication::palette().color(QPalette::Base);
    }

    QColor textColor;

    // From https://alienryderflex.com/hsp.html,
    // https://forum.qt.io/topic/106362/best-way-to-set-text-color-for-maximum-contrast-on-background-color/3
    int brightness = 0.299*color.red() + 0.587*color.green() + 0.114*color.blue();
    if (brightness > 127) {
        textColor = Qt::black;
    } else {
        textColor = Qt::white;
    }

    ui->plainTextEdit->setStyleSheet(
                QString("QPlainTextEdit {color: %1; background-color: %2;}")
                .arg(textColor.name())
                .arg(color.name()));
}

void KanbanCardWidget::onColorAction()
{
    QColor c = QColorDialog::getColor(mCard->color());
    if (c.isValid()) {
        mCard->setColor(c);
    }
}

void KanbanCardWidget::onResetColorAction()
{
    // Set invalid color
    mCard->setColor(QColor());
}

void KanbanCardWidget::onSplitLinesAction()
{
    if (!mCard) { return; }

    QStringList lines = mCard->text().split("\n");

    if (!lines.isEmpty()) {
        mCard->setText(lines.takeFirst());
    }

    if (!lines.isEmpty()) {
        emit splitLinesToCards(lines);
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

    int lineHeight = ui->plainTextEdit->fontMetrics().boundingRect("M").height();
    int lines = ui->plainTextEdit->document()->size().height();
    int magic = lineHeight;
    int docHeight = lineHeight * lines + magic;

    ui->plainTextEdit->setFixedHeight( docHeight );

    int hdelta = ui->plainTextEdit->height() - hbefore;
    int currentHeight = height();

    if (hdelta != 0) {
        resize(width(), currentHeight + hdelta);
        emit contentsChanged();
    }
}

bool KanbanCardWidget::eventFilter(QObject* /*watched*/, QEvent *event)
{
    bool ret = false; // False to pass event on, true to consume event

    if (event->type() == QEvent::ContextMenu) {

        ret = true; // Do not pass event on to plainTextEdit

        // Modify and show the standard context menu
        QContextMenuEvent* menuEvent = static_cast<QContextMenuEvent*>(event);
        QMenu* menu = ui->plainTextEdit->createStandardContextMenu(
                    menuEvent->pos());
        QAction* firstAction = menu->actions().at(0);
        menu->insertAction(firstAction, &colorAction);
        menu->insertAction(firstAction, &resetColorAction);
        menu->insertAction(firstAction, &splitLinesAction);
        menu->insertSeparator(firstAction);
        menu->popup(ui->plainTextEdit->mapToGlobal(menuEvent->pos()));

    } else if (event->type() == QEvent::MouseButtonPress) {

        emit clicked();

    }
    return ret;
}

void KanbanCardWidget::resizeEvent(QResizeEvent* /*event*/)
{
    resizeToContent();
}
