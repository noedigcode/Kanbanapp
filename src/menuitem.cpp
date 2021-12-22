#include "menuitem.h"

MenuItem::MenuItem(QString text, QWidget *parent) : QWidget(parent)
{
    setContentsMargins(5,0,5,0);

    layout = new QHBoxLayout(this);
    layout->setMargin(0);
    setLayoutSpacing(2);

    label = new QLabel(text, this);
    label->setMinimumWidth(100);

    QToolButton* delbtn = new QToolButton(this);
    delbtn->setIcon(QIcon(":/res/icons/icons8-cancel-48.png"));
    delbtn->setAutoRaise(true); // Hide border
    delbtn->setToolTip("Remove");
    connect(delbtn, &QToolButton::clicked, this, &MenuItem::removeButtonClicked);

    QToolButton* folderbtn = new QToolButton(this);
    folderbtn->setIcon(QIcon(":/res/icons/icons8-browse-folder-48.png"));
    folderbtn->setAutoRaise(true); // Hide border
    folderbtn->setToolTip("Open containing folder");
    connect(folderbtn, &QToolButton::clicked, this, &MenuItem::openFolderButtonClicked);

    layout->addWidget(label);
    layout->addWidget(folderbtn);
    layout->addWidget(delbtn);
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
