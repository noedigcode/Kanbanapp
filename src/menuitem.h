#ifndef MENUITEM_H
#define MENUITEM_H

#include <QWidget>
#include <QEvent>
#include <QLabel>
#include <QHBoxLayout>
#include <QToolButton>

class MenuItem : public QWidget
{
    Q_OBJECT
public:
    explicit MenuItem(QString text = "", QWidget *parent = nullptr);

    void setLayoutSpacing(int spacing);
    void setText(QString text);
    QString text();

signals:
    void buttonClicked();

private:
    QHBoxLayout* layout;
    QLabel* label;

    // QWidget interface
protected:
    void enterEvent(QEvent *event) override;
    void leaveEvent(QEvent *event) override;
};

#endif // MENUITEM_H
