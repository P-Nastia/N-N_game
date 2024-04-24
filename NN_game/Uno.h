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
    vector<unoCards> cardDeck;//������ ����
    CircleShape currentColor;//���� ����
    unoCards currentCard;
    fstream file;//���� � ��������� �� ��������� ����
    Texture textureUnoCard;
    Sprite unoCard;//����������, ���� ���������� 1 �����
    Texture textureForHiddenCard;
    vector<int> availableCards;//������ � ����� ����, �� �� �� �����������������
public:
    Sprite hiddenCardDeck;//�������� ������, � ��� ����� ������������ �����
    Uno(string fileName);//���������� ������� ������, ������������ ���������� ���� �� ����� �����
    void setCurrentColor(string color);//�������������� ���� ��� �����, ���� ������ ���� ���� ��������������� �� ��� ���
    unoCards takeCard();//� ������ �������� ����� ��� ��������� �� �������� ������
    void setCurrentCard(unoCards card);//������ �����, ���� ����� ��������
    unoCards getCurrentCard() {//������� �����, ��� ����� ������� �� ���
        return currentCard;
    }
    void showAllElements(RenderWindow& window);


    ~Uno();
};

class basePlayer
{
protected:
    vector<unoCards> playerCardDeck;//������ ���� ������
    bool isPlayerTurn;
    unoCards cardToBeat;//�����, ��� ����� ������
    RectangleShape underlineForTurn;//��� ��� ����������� ������
    Font font;//����� ��� ������
    Text textForTurn;//�����
public:
    basePlayer(vector<unoCards>& startDeck) {
        playerCardDeck.resize(7);
        for (int i = 0; i < 7; i++) {
            playerCardDeck.at(i).sprite.setTexture(startDeck.at(i).texture);
            playerCardDeck.at(i).sprite = startDeck.at(i).sprite;
            playerCardDeck.at(i).sprite.setScale(Vector2f(1.5f, 1.5f));
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
    virtual void showElements(RenderWindow& window) = 0;//������� �� ����� ������ ������, ��� ���/�� �������� �� ������, ���� ���� �������� ���

    bool isEmpty() { //�� ������ �������
        if (playerCardDeck.empty() == true)
            return true;
        else
            return false;
    }
    void addCard(unoCards& card) {//��������� �����
        playerCardDeck.resize(playerCardDeck.size() + 1);
        playerCardDeck.at(playerCardDeck.size() - 1).color = card.color;
        playerCardDeck.at(playerCardDeck.size() - 1).sprite.setTextureRect(card.sprite.getTextureRect());
        playerCardDeck.at(playerCardDeck.size() - 1).sprite = card.sprite;
        playerCardDeck.at(playerCardDeck.size() - 1).sprite.setScale(Vector2f(1.5f, 1.5f));
        playerCardDeck.at(playerCardDeck.size() - 1).symbol = card.symbol;

    }
    void setCardToBeat(unoCards& card) {
        cardToBeat.color = card.color;
        cardToBeat.sprite.setTextureRect(card.sprite.getTextureRect());
        cardToBeat.symbol = card.symbol;
    }
    int checkForOtherThanNumberCard(unoCards& card) {//�������� �� ��, �� �������� ������ � ��������� ����, ��������, ����� ������� ��� ������� �������� �����
        if (card.symbol >= '0' && card.symbol <= '9')
            return 0;//���������, �� �� ������ ����� � ������
        else {
            if (card.symbol == "skip") {
                return 1;//���������, �� �� ����� �������� ����
            }
            if (card.symbol == "reverse") {
                return 2;//���������, �� �� ����� ���� ���� � ���� �������
            }
            if (card.symbol == "draw2") {
                return 3;//���������, �� �� ����� "��������� +2 "
            }
            if (card.symbol == "wild") {
                return 4;//���������, �� �� ����� "������ ����"
            }
            if (card.symbol == "draw4") {
                return 5;//���������, �� �� ����� "��������� +4 � ������ ����"
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
        if (!playerCardDeck.empty()) {
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
        }
        changeColorForTurn();
        window.draw(textForTurn);
        window.draw(underlineForTurn);
    }

    unoCards pushCard(Vector2f mouse) { //������� ��� ��� ������ (��������� ���������� ����� � ��������� �� ��� ����� ������)
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
        if (!playerCardDeck.empty())
            playerCardDeck.clear();
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
    void resizeHiddenCards() {
        hiddenCards.resize(playerCardDeck.size());
    }
    void showElements(RenderWindow& window) override {
        if (!playerCardDeck.empty()) {
            int halfSizeOfcardDeck = playerCardDeck.size() / 2;
            if (halfSizeOfcardDeck != 1) {
                int posX = (window.getSize().x / 2) - (halfSizeOfcardDeck * 90);
                for (int i = 0; i < hiddenCards.size(); i++) {
                    hiddenCards.at(i).setTexture(HiddenCardTexture);
                    hiddenCards.at(i).setScale(Vector2f(0.269f, 0.269f));
                    hiddenCards.at(i).setPosition(Vector2f(posX, 150));
                    posX += 85.4;
                }
                for (int i = 0; i < hiddenCards.size(); i++) {
                    window.draw(hiddenCards.at(i));
                }
            }
            else if (hiddenCards.size() == 1) {
                hiddenCards.at(0).setTexture(HiddenCardTexture);
                hiddenCards.at(0).setScale(Vector2f(0.269f, 0.269f));
                hiddenCards.at(0).setPosition(900, 150);
                window.draw(hiddenCards.at(0));
            }
            changeColorForTurn();
            window.draw(textForTurn);
            window.draw(underlineForTurn);
        }
        /*changeColorForTurn();
        window.draw(textForTurn);
        window.draw(underlineForTurn);*/
    }

    unoCards pushCard() {
        unoCards tempCard;
        tempCard.color = "notFound";
        tempCard.symbol = "notFound";
        tempCard.sprite = playerCardDeck.at(0).sprite;
        if (playerCardDeck.empty() == false) {

            bool cardFound = false;

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
                for (int i = 0; i < playerCardDeck.size(); i++) {
                    if (playerCardDeck.at(i).symbol == "draw4" || playerCardDeck.at(i).symbol == "draw2" || playerCardDeck.at(i).symbol == "wild") {
                        item = i;
                        cardFound = true;
                        break;
                    }
                }
            }
            if (cardFound == false) {
                return tempCard;
            }
            else {
                tempCard.color = playerCardDeck.at(item).color;
                tempCard.sprite = playerCardDeck.at(item).sprite;
                tempCard.symbol = playerCardDeck.at(item).symbol;
                playerCardDeck.erase(playerCardDeck.begin() + item);
                hiddenCards.pop_back();
                return tempCard;
            }
        }
        else
            return tempCard;
    }

    string pickColor() {//������ ���� ��� �����

        for (int i = 0; i < playerCardDeck.size(); i++) {
            if (playerCardDeck.at(i).color == "red")
                return "red";
            else if (playerCardDeck.at(i).color == "green")
                return "green";
            else if (playerCardDeck.at(i).color == "yellow")
                return "yellow";
            else if (playerCardDeck.at(i).color == "blue")
                return "blue";
        }
    }

    ~Opponent() {
        if (!playerCardDeck.empty())
            playerCardDeck.clear();
        if (!hiddenCards.empty())
            hiddenCards.clear();
    }
};