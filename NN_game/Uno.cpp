#include "Uno.h"

Uno::Uno(string fileName)
{
    srand(time(NULL));
    cardDeck.resize(108);
    float widt = 85.4;
    float length = 0;
    cardDeck.resize(108);
    int countCards = 0;
    int posY = 10;
    int count = 0;
    while (count < 8) {
        if (count < 4) {
            for (int i = 0, posX = 10; i < 14; i++, posX += 100, countCards++) {
                cardDeck.at(countCards).sprite.setTexture(cardDeck.at(countCards).texture);
                cardDeck.at(countCards).sprite.setTextureRect(IntRect(widt * i, length, 85.4, 128));
            }
        }
        else if (count >= 4) {
            for (float i = 1, posX = 10; i < 14; i++, posX += 100, countCards++) {
                cardDeck.at(countCards).sprite.setTexture(cardDeck.at(countCards).texture);
                cardDeck.at(countCards).sprite.setTextureRect(IntRect(widt * i, length, 85.4, 128));
            }
        }
        count++;
        posY += 130;
        length += 128;
    }
    countCards = 0;
    file.open(fileName);
    if (file.is_open()) {
        while (countCards < 108) {
            file >> cardDeck.at(countCards).symbol;
            file >> cardDeck.at(countCards).color;
            countCards++;
        }
    }
    /*for (int i = 0; i < 108; i++) {
        cout << cardDeck.at(i).symbol << ":" << cardDeck.at(i).color << endl;
    }*/
    //встановлюємо першу карту, з якої буде починатися гра
    currentColor.setRadius(17);
    currentColor.setOutlineThickness(4);
    currentColor.setPosition(918, 550);
    int randomNum;
    do {
        randomNum = rand() % 108;
        cout << randomNum << endl;
    } while (!(cardDeck.at(randomNum).symbol >= '0' && cardDeck.at(randomNum).symbol <= '9'));

    cout << "size " << cardDeck.size() << endl;
    string color = cardDeck.at(randomNum).color;
    setCurrentColor(color);
    currentCard.sprite.setPosition(958, 475);
    setCurrentCard(cardDeck.at(randomNum));
    currentCard.sprite.setScale(Vector2f(1.6f, 1.6f));
    cardDeck.erase(cardDeck.begin() + randomNum);
    cout << "size " << cardDeck.size() << endl;

    //встановлюємл зображення карти-вигуку "uno"
    textureUnoCard.loadFromFile("Uno/oneCardLeft.png");
    unoCard.setTexture(textureUnoCard);
    unoCard.setScale(0.2f, 0.2f);
    unoCard.setPosition(670, 542);

    //встановлюємо зображення для імітованої колоди карт
    textureForHiddenCard.loadFromFile("Uno/hiddenCard.png");
    hiddenCardDeck.setTexture(textureForHiddenCard);
    hiddenCardDeck.setScale(0.269f, 0.269f);
    hiddenCardDeck.setPosition(780, 475);
}

void Uno::setCurrentColor(string color)
{
    currentColor.setOutlineColor(Color::White);
    if (color == "red") {
        currentColor.setFillColor(Color(255, 85, 85));
    }
    else if (color == "green") {
        currentColor.setFillColor(Color(35, 177, 77));
    }
    else if (color == "yellow") {
        currentColor.setFillColor(Color(255, 201, 13));
    }
    else if (color == "blue") {
        currentColor.setFillColor(Color(85, 85, 255));
    }
}

unoCards Uno::takeCard()
{
    srand(time(NULL));
    unoCards tempCard;
    int num = rand() % cardDeck.size();
    tempCard.color = cardDeck.at(num).color;
    tempCard.sprite = cardDeck.at(num).sprite;
    tempCard.symbol = cardDeck.at(num).symbol;
    cardDeck.erase(cardDeck.begin() + num);
    return tempCard;
}

void Uno::setCurrentCard(unoCards card)
{
    currentCard.color = card.color;
    currentCard.sprite.setTextureRect(card.sprite.getTextureRect());
    //currentCard.sprite = card.sprite;
    currentCard.symbol = card.symbol;
    setCurrentColor(currentCard.color);
}

void Uno::showAllElements(RenderWindow& window)
{
    window.draw(currentCard.sprite);
    window.draw(hiddenCardDeck);
    window.draw(currentColor);
    window.draw(unoCard);
}

Uno::~Uno()
{
    for (int i = 0; i < cardDeck.size(); i++) {
        cardDeck.pop_back();
    }
}