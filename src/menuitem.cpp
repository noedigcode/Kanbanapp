#include "menuitem.h"

MenuItem::MenuItem(QString text, QWidget *parent) : QWidget(parent)
{
    setBackgroundRole(mBackgroundColorRole);
    setAutoFillBackground(true);

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

void MenuItem::setBackgroundToWindow()
{
    mBackgroundColorRole = QPalette::Window;
    setBackgroundRole(mBackgroundColorRole);
}

void MenuItem::setBackgroundToBase()
{
    mBackgroundColorRole = QPalette::Base;
    setBackgroundRole(mBackgroundColorRole);
}

void MenuItem::setBackgroundNotFilled()
{
    setAutoFillBackground(false);
}

void MenuItem::setHighlightEnabled(bool enable)
{
    mHighlightEnabled = enable;
}

void MenuItem::enterEvent(QEvent *event)
{
    if (mHighlightEnabled) {
        setBackgroundRole(QPalette::Highlight);
    }

    event->ignore();
}

void MenuItem::leaveEvent(QEvent *event)
{
    if (mHighlightEnabled) {
        setBackgroundRole(mBackgroundColorRole);
    }

    event->ignore();
}
