#include "kanban.h"

Card::Card(QObject *parent) : QObject(parent)
{
    mColor = mDefaultColor;
}

void Card::setText(QString text)
{
    if (mText != text) {
        mText = text;
        emit textChanged(text);
    }
}

QString Card::text()
{
    return mText;
}

void Card::setColor(QColor color)
{
    if (!color.isValid()) {
        color = mDefaultColor;
    }
    if (mColor != color) {
        mColor = color;
        emit colorChanged(color);
    }
}

QColor Card::color()
{
    return mColor;
}

KanbanList::KanbanList(QObject *parent) : QObject(parent)
{

}

void KanbanList::setTitle(QString title)
{
    mTitle = title;
    emit titleChanged(title);
}

QString KanbanList::title()
{
    return mTitle;
}

QList<Card *> KanbanList::cards()
{
    return mCards;
}

void KanbanList::addCard(Card *card)
{
    card->setParent(this);
    mCards.append(card);
    emit cardAdded(card);
}

int KanbanList::cardIndex(Card *card)
{
    return mCards.indexOf(card);
}

void KanbanList::moveCard(Card *card, int newIndex)
{
    mCards.move(cardIndex(card), newIndex);
    emit cardMoved(card, newIndex);
}

void KanbanList::takeCard(Card *card)
{
    mCards.removeAll(card);
    emit cardRemoved(card);
}

QJsonObject KanbanList::toJson()
{
    QJsonObject j;
    j["title"] = mTitle;

    QJsonArray cards;
    foreach (Card* card, mCards) {
        QJsonObject obj = card->toJson();
        cards.append(obj);
    }
    j["cards"] = cards;

    return j;
}

void KanbanList::fromJson(QJsonObject jsonObject)
{
    setTitle( jsonObject["title"].toString() );

    QJsonArray cards = jsonObject["cards"].toArray();
    for (int i=0; i < cards.count(); i++) {
        Card* card = new Card();
        card->fromJson(cards.at(i).toObject());
        addCard(card);
    }
}

Board::Board(QObject *parent) : QObject(parent)
{

}

QList<KanbanList *> Board::lists()
{
    return mLists;
}

void Board::addList(KanbanList *list)
{
    list->setParent(this);
    mLists.append(list);
    emit listAdded(list);
}

void Board::moveList(KanbanList *list, int newIndex)
{
    mLists.move( mLists.indexOf(list), newIndex );
    emit listMoved(list, newIndex);
}

void Board::removeList(KanbanList *list)
{
    mLists.removeAll(list);
    emit listRemoved(list);
    list->deleteLater();
}

void Board::clear()
{
    while (mLists.count()) {
        removeList(mLists[0]);
    }
}

QJsonObject Board::toJsonObject()
{
    QJsonObject j;

    QJsonArray lists;
    foreach (KanbanList* list, mLists) {
        QJsonObject obj = list->toJson();
        lists.append(obj);
    }
    j["lists"] = lists;

    return j;
}

void Board::fromJsonObject(QJsonObject jsonObject)
{
    QJsonArray lists = jsonObject["lists"].toArray();
    for (int i=0; i < lists.count(); i++) {
        KanbanList* list = new KanbanList();
        list->fromJson(lists.at(i).toObject());
        addList(list);
    }
}

QByteArray Board::toJsonText()
{
    QJsonDocument doc( this->toJsonObject() );
    return doc.toJson();
}

bool Board::saveToFile(QString filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        // Error opening file
        return false;
    }
    file.write( this->toJsonText() );
    file.close();
    return true;
}

/* Loads board from specified filename and returns true if file could be read,
 * false otherwise. Optional parseErrorString is set to the Json parsing error
 * string, or not assigned if no parsing error occurred. */
bool Board::loadFromFile(QString filename, QString *parseErrorString)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        // Error opening file
        return false;
    }

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &parseError);
    file.close();

    if (parseErrorString) {
        if (parseError.error != QJsonParseError::NoError) {
            *parseErrorString = parseError.errorString();
        }
    }

    QJsonObject obj = doc.object();
    this->fromJsonObject(obj);

    return true;
}

QJsonObject Card::toJson()
{
    QJsonObject j;
    j["text"] = mText;
    j["color"] = mColor.name();
    return j;
}

void Card::fromJson(QJsonObject jsonObject)
{
    setText( jsonObject["text"].toString() );
    setColor(QColor(jsonObject["color"].toString()));
}
