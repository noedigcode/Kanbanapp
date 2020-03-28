#include "menuitem.h"

MenuItem::MenuItem(QString text, QWidget *parent) : QWidget(parent)
{
    setContentsMargins(5,0,5,0);

    layout = new QHBoxLayout(this);
    layout->setMargin(0);
    setLayoutSpacing(2);

    label = new QLabel(text, this);
    label->setMinimumWidth(100);

    QToolButton* tb = new QToolButton(this);
    tb->setIcon(QIcon("://icons/delete_48px.png"));
    tb->setAutoRaise(true); // Hide border
    tb->setToolTip("Remove");
    connect(tb, &QToolButton::clicked, this, &MenuItem::buttonClicked);

    layout->addWidget(label);
    layout->addWidget(tb);
    setLayout(layout);
}

void MenuItem::setLayoutSpacing(int spacing)
{
    layout->setSpacing(spacing);
}

void MenuItem::setText(QString text)
{
    label->setText(text);
}

QString MenuItem::text()
{
    return label->text();
}

void MenuItem::enterEvent(QEvent *event)
{
    setBackgroundRole(QPalette::Highlight);
    setAutoFillBackground(true);

    event->ignore();
}

void MenuItem::leaveEvent(QEvent *event)
{
    setBackgroundRole(QPalette::Window);
    setAutoFillBackground(true);

    event->ignore();
}
