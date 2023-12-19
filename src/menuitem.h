#ifndef MENUITEM_H
#define MENUITEM_H

#include <QEvent>
#include <QHBoxLayout>
#include <QLabel>
#include <QToolButton>
#include <QWidget>

class MenuItem : public QWidget
{
    Q_OBJECT
public:
    explicit MenuItem(QString text = "", QWidget *parent = nullptr);

    void setLayoutSpacing(int spacing);
    void setText(QString text);
    QString text();

    void setBackgroundToWindow();
    void setBackgroundToBase();
    void setBackgroundNotFilled();

    void setHighlightEnabled(bool enable);

signals:
    void removeButtonClicked();
    void openFolderButtonClicked();

private:
    QHBoxLayout* layout;
    QLabel* label;
    bool mHighlightEnabled = true;
    QPalette::ColorRole mBackgroundColorRole = QPalette::Window;

    // QWidget interface
protected:
    void enterEvent(QEvent *event) override;
    void leaveEvent(QEvent *event) override;
};

#endif // MENUITEM_H
