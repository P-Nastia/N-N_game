#pragma once
#include <string>
#include <iomanip>
#include <iostream>
#include <SFML/Graphics.hpp>
#include <fstream>
#include <ctime>
#include <Windows.h>
#include <map>
#include "gameText.h"
using namespace std;
using namespace sf;
struct unoCards {
public:
    Texture texture;
    Sprite sprite;
    string symbol;
    string color;
    unoCards() { texture.loadFromFile("Uno/Cards.png"); sprite.setTexture(texture); }
    unoCards(string s) :symbol{ s } {}
};
class Uno
{
protected:
    vector<unoCards> cardDeck;//колода карт
    CircleShape currentColor;//кол≥р ходу
    unoCards currentCard;
    fstream file;//файл з символами та кольорами карт
    Texture textureUnoCard;
    Sprite unoCard;//нажимаЇтьс€, €кщо залишилас€ 1 карта
    Texture textureForHiddenCard;

public:
    Sprite hiddenCardDeck;//картинка колоди, з €коњ мають вит€гуватис€ карти
    Uno(string fileName);//заповнюЇмо гральну колоду, встановлюЇмо початковий кол≥р та першу карту
    void setCurrentColor(string color);//встановлюЇтьс€ кол≥р дл€ круга, €кий п≥дказуЇ €кий кол≥р використовувати п≥д час гри
    unoCards takeCard();//з колоди видаЇтьс€ карта при натисканн≥ на потр≥бний надпис
    void setCurrentCard(unoCards card);//сетить карту, €кою щойно походили
    unoCards getCurrentCard() {//повертаЇ карту, €ка щойно прийшла на ст≥л
        return currentCard;
    }
    void showAllElements(RenderWindow& window);

    ~Uno();
};

class basePlayer
{
protected:
    vector<unoCards> playerCardDeck;//колода карт гравц€
    bool isPlayerTurn;
    unoCards cardToBeat;//карта, €ку треба побити
    RectangleShape underlineForTurn;//л≥н≥€ дл€ п≥дкресленн€ тексту
    Font font;//шрифт дл€ тексту
    Text textForTurn;//текст
public:
    basePlayer(vector<unoCards>& startDeck) {
        playerCardDeck.resize(7);
        for (int i = 0; i < 7; i++) {
            playerCardDeck.at(i).sprite = startDeck.at(i).sprite;
            playerCardDeck.at(i).sprite.setScale(Vector2f(1.5f, 1.5f));
            playerCardDeck.at(i).texture = startDeck.at(i).texture;
            playerCardDeck.at(i).symbol = startDeck.at(i).symbol;
            playerCardDeck.at(i).color = startDeck.at(i).color;
        }
        underlineForTurn.setSize(Vector2f(600.f, 0.5f));
        underlineForTurn.setOutlineThickness(1.5);
        font.loadFromFile("Fonts/LeagueSpartan-Bold.ttf");
        textForTurn.setFont(font);
        textForTurn.setCharacterSize(30);
        isPlayerTurn = true;
        startDeck.clear();
    }

    void changeColorForTurn() {
        if (isPlayerTurn == false) {
            textForTurn.setFillColor(Color(245, 238, 205));
            underlineForTurn.setOutlineColor(Color(245, 238, 205));
            underlineForTurn.setFillColor(Color(245, 238, 205));
        }
        else {
            textForTurn.setFillColor(Color(251, 251, 0));
            underlineForTurn.setOutlineColor(Color(251, 251, 0));
            underlineForTurn.setFillColor(Color(251, 251, 0));
        }
    }
    virtual void showElements(RenderWindow& window) = 0;//вивести на екран колоду гравц€, л≥н≥ю над/п≥д надписом та надпис, €кий буде задавати х≥д

    bool isEmpty() { //чи колода порожн€
        if (playerCardDeck.empty() == true)
            return true;
        return false;
    }
    virtual void addCard(unoCards& card) {//пот€гнути карту
        playerCardDeck.resize(playerCardDeck.size() + 1);
        playerCardDeck.at(playerCardDeck.size() - 1).color = card.color;
        playerCardDeck.at(playerCardDeck.size() - 1).sprite = card.sprite;
        playerCardDeck.at(playerCardDeck.size() - 1).symbol = card.symbol;
    }
    void setCardToBeat(unoCards& card) {
        cardToBeat.color = card.color;
        cardToBeat.sprite.setTextureRect(card.sprite.getTextureRect());
        cardToBeat.symbol = card.symbol;
    }
    int checkForOtherThanNumberCard() {//перев≥рка на те, чи попалас€ карта з пропуском ходу, реверсом, зм≥ною кольору або т€гнути додатков≥ карти
        if (cardToBeat.symbol >= '0' && cardToBeat.symbol <= '9')
            return 0;//повернути, що це просто карта з цифрою
        else {
            if (cardToBeat.symbol == "skip") {
                return 1;//повернути, що це карта пропуску ходу
            }
            if (cardToBeat.symbol == "reverse") {
                return 2;//повернути, що це карта зм≥ни ходу в ≥ншу сторону
            }
            if (cardToBeat.symbol == "draw2") {
                return 3;//повернути, що це карта "пот€гнути +2 "
            }
            if (cardToBeat.symbol == "wild") {
                return 4;//повернути, що це карта "зм≥нити кол≥р"
            }
            if (cardToBeat.symbol == "draw4") {
                return 5;//повернути, що це карта "пот€гнути +4 ≥ зм≥нити кол≥р"
            }
        }
    }
    void setTurn(bool isTurn) { isPlayerTurn = isTurn; }
    bool getTurn() { return isPlayerTurn; }
    ~basePlayer() {};
};

class Player : public basePlayer {
public:
    Player(vector<unoCards>& startDeck) :basePlayer(startDeck) {
        textForTurn.setString("You");
        textForTurn.setPosition(Vector2f(730.f, 740.f));
        underlineForTurn.setPosition(Vector2f(650, 780));
    }
    void showElements(RenderWindow& window) override {

        int halfSizeOfcardDeck = playerCardDeck.size() / 2;
        int posX = (window.getSize().x / 2) - (halfSizeOfcardDeck * playerCardDeck.at(0).sprite.getScale().x * 85.4);
        if (playerCardDeck.size() % 2 != 0) {
            posX -= 100;
        }
        for (int i = 0; i < playerCardDeck.size(); i++) {
            playerCardDeck.at(i).sprite.setPosition(Vector2f(posX, 800));
            posX += (playerCardDeck.at(0).sprite.getScale().x * 85.4) + 10;
        }
        for (int i = 0; i < playerCardDeck.size(); i++) {
            window.draw(playerCardDeck.at(i).sprite);
        }
        changeColorForTurn();
        window.draw(textForTurn);
        window.draw(underlineForTurn);
    }

    unoCards pushCard(Vector2f mouse) { //зробити х≥д дл€ гравц€ (приход€ть координати мишки ≥ зв≥р€Їтьс€ на €ку карту попало)
        for (int i = 0; i < playerCardDeck.size(); i++) {
            if (playerCardDeck.at(i).sprite.getGlobalBounds().contains(mouse.x, mouse.y) == true) {
                unoCards tempCard;
                tempCard.color = playerCardDeck.at(i).color;
                tempCard.sprite = playerCardDeck.at(i).sprite;
                tempCard.symbol = playerCardDeck.at(i).symbol;
                playerCardDeck.erase(playerCardDeck.begin() + i);
                return tempCard;
                break;
            }
        }
    }

    ~Player() {
        for (int i = 0; i < playerCardDeck.size(); i++) {
            playerCardDeck.pop_back();
        }
    }
};

class Opponent : public basePlayer {
    vector<Sprite> hiddenCards;
    Texture HiddenCardTexture;
public:
    Opponent(vector<unoCards>& startDeck) :basePlayer(startDeck) {
        textForTurn.setString("Opponent");
        textForTurn.setPosition(Vector2f(730.f, 370.f));
        hiddenCards.resize(playerCardDeck.size());

        HiddenCardTexture.loadFromFile("Uno/hiddenCard.png");
        for (int i = 0; i < hiddenCards.size(); i++) {
            hiddenCards.at(i).setTexture(HiddenCardTexture);
            hiddenCards.at(i).setScale(Vector2f(0.269f, 0.269f));
        }
        underlineForTurn.setPosition(Vector2f(650.f, 370.f));
    }
    void showElements(RenderWindow& window) override {

        int halfSizeOfcardDeck = playerCardDeck.size() / 2;
        int posX = (window.getSize().x / 2) - (halfSizeOfcardDeck * hiddenCards.at(0).getScale().x * 85.4);
        if (hiddenCards.size() % 2 != 0) {
            posX -= 100;
        }
        for (int i = 0; i < hiddenCards.size(); i++) {
            hiddenCards.at(i).setPosition(Vector2f(posX, 150));
            posX += (hiddenCards.at(i).getScale().x * 85.4) + 10;
        }
        for (int i = 0; i < hiddenCards.size(); i++) {
            window.draw(hiddenCards.at(i));
        }
        changeColorForTurn();
        window.draw(textForTurn);
        window.draw(underlineForTurn);
    }

    unoCards pushCard() {
        bool cardFound = false;
        unoCards tempCard;
        int item;
        for (int i = 0; i < playerCardDeck.size(); i++) {
            if (cardToBeat.color == playerCardDeck.at(i).color) {
                item = i;
                cardFound = true;
                break;
            }
            else if (cardToBeat.symbol == playerCardDeck.at(i).symbol) {
                item = i;
                cardFound = true;
                break;
            }
        }
        if (cardFound == false) {
            unoCards card;
            card.color = "notFound";
            card.symbol = "notFound";

            return card;
        }
        else {
            tempCard.color = playerCardDeck.at(item).color;
            tempCard.sprite = playerCardDeck.at(item).sprite;
            tempCard.symbol = playerCardDeck.at(item).symbol;
            playerCardDeck.erase(playerCardDeck.begin() + item);
            return tempCard;
        }
    }

    string pickColor() {//обрати кол≥р дл€ зам≥ни
        int counter = 4;
        int red = 0, green = 0, yellow = 0, blue = 0;

        for (int i = 0; i < playerCardDeck.size(); i++) {
            if (playerCardDeck.at(i).color == "red")
                red++;
            else if (playerCardDeck.at(i).color == "green")
                green++;
            else if (playerCardDeck.at(i).color == "yellow")
                yellow++;
            else if (playerCardDeck.at(i).color == "blue")
                blue++;
        }
        if (red >= green && red >= yellow && red >= blue)
            return "red";
        if (green >= red && green >= yellow && green >= blue)
            return "green";
        if (yellow >= red && yellow >= green && yellow >= blue)
            return "yellow";
        if (blue >= red && blue >= yellow && blue >= green)
            return "blue";
    }

    ~Opponent() {
        for (int i = 0; i < playerCardDeck.size(); i++) {
            playerCardDeck.pop_back();
        }
    }
};