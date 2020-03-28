#ifndef KANBAN_H
#define KANBAN_H

#include <QObject>
#include <QColor>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

class Card : public QObject
{
    Q_OBJECT
public:
    explicit Card(QObject *parent = nullptr);

    void setText(QString text);
    QString text();
    void setColor(QColor color);
    QColor color();

    QJsonObject toJson();
    void fromJson(QJsonObject jsonObject);

signals:
    void textChanged(QString text);
    void colorChanged(QColor color);

private:
    QString mText;
    QColor mColor;
    const QColor mDefaultColor{Qt::white};
};




class KanbanList : public QObject
{
    Q_OBJECT
public:
    explicit KanbanList(QObject *parent = nullptr);

    void setTitle(QString title);
    QString title();

    QList<Card*> cards();
    void addCard(Card* card);
    int cardIndex(Card* card);
    void moveCard(Card* card, int newIndex);
    void takeCard(Card* card);

    QJsonObject toJson();
    void fromJson(QJsonObject jsonObject);

signals:
    void titleChanged(QString title);
    void cardAdded(Card* card);
    void cardMoved(Card* card, int newIndex);
    void cardRemoved(Card* card);

private:
    QString mTitle;
    QList<Card*> mCards;
};




class Board : public QObject
{
    Q_OBJECT
public:
    explicit Board(QObject *parent = nullptr);

    QList<KanbanList*> lists();
    void addList(KanbanList* list);
    void moveList(KanbanList* list, int newIndex);
    void removeList(KanbanList* list);
    void clear();

    QJsonObject toJsonObject();
    void fromJsonObject(QJsonObject jsonObject);
    QByteArray toJsonText();

    bool saveToFile(QString filename);
    bool loadFromFile(QString filename, QString* parseErrorString = nullptr);

signals:
    void listAdded(KanbanList* list);
    void listMoved(KanbanList* list, int newIndex);
    void listRemoved(KanbanList* list);

private:
    QList<KanbanList*> mLists;
};

#endif // KANBAN_H
