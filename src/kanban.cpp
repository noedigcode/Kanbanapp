#include "kanban.h"

Card::Card(QObject *parent) : QObject(parent)
{

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

/* Add card at specified index. If index -1 or out of range, card is added to
 * the bottom of the list. */
void KanbanList::addCard(Card *card, int index)
{
    card->setParent(this);
    if ((index < 0) || (index >= mCards.count())) {
        index = mCards.count();
    }
    mCards.insert(index, card);
    emit cardAdded(card, index);
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

GidFile::Result Board::saveToFile(QString filename)
{
    return GidFile::write(filename, this->toJsonText());
}

/* Loads board from specified filename and returns the read result.
 * Optional parseErrorString is set to the Json parsing error string, or not
 * assigned if no parsing error occurred. */
GidFile::Result Board::loadFromFile(QString filename, QString *parseErrorString)
{
    GidFile::ReadResult r = GidFile::read(filename);
    if (!r.result.success) {
        return r.result;
    }

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(r.data, &parseError);

    if (parseErrorString) {
        if (parseError.error != QJsonParseError::NoError) {
            *parseErrorString = parseError.errorString();
        }
    }

    QJsonObject obj = doc.object();
    this->fromJsonObject(obj);

    return r.result;
}

QJsonObject Card::toJson()
{
    QJsonObject j;
    j["text"] = mText;

    // Store invalid color as blank string. Display will decide how to handle it,
    // i.e. use system colors.
    QString colorText;
    if (mColor.isValid()) {
        colorText = mColor.name();
    }
    j["color"] = colorText;
    return j;
}

void Card::fromJson(QJsonObject jsonObject)
{
    setText( jsonObject["text"].toString() );
    setColor(QColor(jsonObject["color"].toString()));
}
