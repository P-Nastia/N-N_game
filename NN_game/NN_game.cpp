#include <string>
#include <iomanip>
#include <iostream>
#include <SFML/Graphics.hpp>
#include <fstream>
#include <ctime>
#include <Windows.h>
#include <map>
using namespace std;
using namespace sf;
#include "Hangman.h"
#include "GameMenu.h"
#include "BattleShip.h"
#include "gameText.h"
#include "Board.h"
#include "Piece.h"

void setText(Text& menuText, float xPos, float yPos, String str, int fontSize = 60,
    Color menuTextColor = Color::White, int bord = 0, Color borderColor = Color::Black)//bord = обрамлення тексту
{
    menuText.setCharacterSize(fontSize);
    menuText.setPosition(xPos, yPos);
    menuText.setString(str);
    menuText.setFillColor(menuTextColor);
    menuText.setOutlineThickness(bord);
    menuText.setOutlineColor(borderColor);
}

void showHangManRules(RenderWindow& rules, int width, int height);
void mainMenuStart(RenderWindow& mainWindow, int width, int height);
void hangmanMenu(RenderWindow& hangmanMenu, int width, int height);
void battleShipMenu(RenderWindow& bsWindow, int width, int height);
void startBattleship(RenderWindow& window, int width, int height);
int checkersMenu(RenderWindow& window, int width, int height);
int startCheckers(RenderWindow& window, int width, int height);

int main()
{
    RenderWindow mainWindow;
    mainWindow.create(VideoMode::getDesktopMode(), "Menu", Style::Fullscreen);
    float width = VideoMode::getDesktopMode().width;//ширина екрана
    float height = VideoMode::getDesktopMode().height;//висота екрана
    mainMenuStart(mainWindow, width, height);
}

int pickCoord() {
    srand(time(NULL));
    int num = rand() % 10;
    return num;
}
void enemiesTurn(gameText& text, BattleShip& enemy, BattleShip& player) {
    player.setTurn(false);
    enemy.setTurn(true);
    text.setString("Wait until opponent takes turn");
}
void playersTurn(gameText& text, BattleShip& enemy, BattleShip& player) {
    player.setTurn(true);
    enemy.setTurn(false);
    text.setString("Your turn");
}
//винесла за функцію startBattleship(), щоби не було помилки ... consider moving some data to heap
BattleShip player("player");
BattleShip enemy("enemy");

void startBattleship(RenderWindow& window, int width, int height) {
    srand(time(NULL));
    RectangleShape bg(Vector2f(VideoMode::getDesktopMode().width, VideoMode::getDesktopMode().height));
    Texture bgTexture;
    bgTexture.loadFromFile("battleship/paper.jpg");
    bg.setTexture(&bgTexture);

    gameText textForShots(" ", "Fonts/LeagueSpartan-Bold.ttf");
    gameText textForTurn(" ", "Fonts/LeagueSpartan-Bold.ttf");
    gameText grid("Your grid                                                                                                                                            opponent`s grid", "Fonts/LeagueSpartan-Bold.ttf");
    grid.setFontsize(15);
    grid.setColor(Color::Black);
    int num = rand() % 2;
    if (num == 0) {
        playersTurn(textForTurn, enemy, player);
    }
    else {
        enemiesTurn(textForTurn, enemy, player);
    }
    int x, y;
    int prevX = -1, prevY = -1;
    bool opponentHit = false;
    int countOfDestroyed = 0;
    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) window.close();
            if (event.type == Event::KeyReleased) {

            }
        }
        if (enemy.isMyTurn() == true) {
            //Sleep(1000);
            x = pickCoord();
            //Sleep(1000);
            y = pickCoord();
            if (player.checkForRepiteness(x, y) == true) {
                while (player.checkForRepiteness(x, y) != false) {
                    x = pickCoord();
                    y = pickCoord();
                }
            }
            if (player.checkForCorrectness(x, y, window) == true) {
                enemiesTurn(textForTurn, enemy, player);
                if (player.checkForCompleteness(x, y) == true) {
                    textForShots.setString(" ");
                }
                else {
                    textForShots.setString(" ");
                }
            }
            else {
                playersTurn(textForTurn, enemy, player);
                textForShots.setString(" ");
            }
        }
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            sf::Vector2i playPos = { (int)((Mouse::getPosition().x / 58) - 18),(int)((Mouse::getPosition().y / 58) - 4) };
            if (enemy.checkForRepiteness(playPos.x, playPos.y) == true) {
                textForTurn.setString("Pick another coordinates");
            }
            else {
                if (enemy.checkForCorrectness(playPos.x, playPos.y, window) == true) {
                    playersTurn(textForTurn, enemy, player);
                    if (enemy.checkForCompleteness(playPos.x, playPos.y) == true) {
                        textForShots.setString("Destroyed");
                    }
                    else {
                        textForShots.setString("Hit");
                    }
                }
                else {
                    enemiesTurn(textForTurn, enemy, player);
                    textForShots.setString("Missed");
                }
            }
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
            battleShipMenu(window, width, height);
        }

        window.clear();
        window.draw(bg);
        grid.showString(window, 470, 840);
        if (textForTurn.str.length() < 10) {
            textForTurn.showString(window, 770, 15);
        }
        else {
            textForTurn.showString(window, 530, 15);
        }
        if (textForShots.str.length() < 10) {
            textForShots.showString(window, 810, 90);
        }
        else {
            textForShots.showString(window, 570, 90);
        }
        player.showCoords(window);
        enemy.showCoords(window);
        player.showField(window);
        enemy.showField(window);
        window.display();
    }
}

void battleShipMenu(RenderWindow& bsWindow, int width, int height) {
    //візуалізація меню
    RectangleShape mainBackground(Vector2f(width, height));//фон меню
    Texture windowTexture;
    windowTexture.loadFromFile("battleship/BSMenuBG.jpeg");
    mainBackground.setTexture(&windowTexture);
    //встановлюємо шрифт для навз категорій
    Font font;
    font.loadFromFile("Fonts/LeagueSpartan-Bold.ttf");
    Text titulText;
    titulText.setFont(font);
    setText(titulText, 340, 90, "Battleship menu", 150, Color(60, 97, 223), 10);
    //використовуємо клас MainGameMenu
    String menuNames[] = { "Rules","Play", "Exit" };
    GameMenu menu(bsWindow, 950, 550, 3, menuNames, 100, 160);
    menu.setColorsForItems(Color(1, 13, 65), Color(217, 0, 5), Color(56, 108, 160));
    //початок гри
    while (bsWindow.isOpen()) {
        Event event;
        while (bsWindow.pollEvent(event)) {
            if (event.type == Event::KeyReleased) {
                if (event.key.code == Keyboard::Up) {
                    menu.moveUp();
                }
                if (event.key.code == Keyboard::Down) {
                    menu.moveDown();
                }
                if (event.key.code == Keyboard::Enter) {
                    switch (menu.getMenuSelected()) {
                    case 0: { }; break;
                    case 1: { startBattleship(bsWindow, width, height); } break;
                    case 2: { mainMenuStart(bsWindow, width, height); }; break;
                    }
                }
            }
        }
        bsWindow.clear();
        bsWindow.draw(mainBackground);
        bsWindow.draw(titulText);
        menu.drawItems();
        bsWindow.display();
    }
}

void mainMenuStart(RenderWindow& mainWindow, int width, int height) {
    //візуалізація меню
    RectangleShape mainBackground(Vector2f(width, height));//фон меню
    Texture windowTexture;
    windowTexture.loadFromFile("Images/mainMenuBg.jpg");
    mainBackground.setTexture(&windowTexture);
    //встановлюємо шрифт для навз категорій
    Font font;
    font.loadFromFile("Fonts/LeagueSpartan-Bold.ttf");
    Text titulText;
    titulText.setFont(font);
    setText(titulText, 480, 50, "Game menu", 150, Color(1, 13, 65), 10);
    //використовуємо клас MainGameMenu
    String menuNames[] = { "Tic tac toe", "Hangman","Snake", "Battleship","Checkers","Exit game" };//потім додамо ще назви
    GameMenu menu(mainWindow, 950, 450, 6, menuNames, 80, 100);
    menu.setColorsForItems(Color(1, 13, 65), Color(217, 0, 5), Color(56, 108, 160));
    //початок гри
    while (mainWindow.isOpen()) {
        Event event;
        while (mainWindow.pollEvent(event)) {
            if (event.type == Event::KeyReleased) {
                if (event.key.code == Keyboard::Up) {
                    menu.moveUp();
                }
                if (event.key.code == Keyboard::Down) {
                    menu.moveDown();
                }
                if (event.key.code == Keyboard::Enter) {
                    switch (menu.getMenuSelected()) {
                    case 0://tic tac toe
                    case 1:hangmanMenu(mainWindow, width, height); break;
                    case 2:
                    case 3:battleShipMenu(mainWindow, width, height); break;
                    case 4:checkersMenu(mainWindow, width, height); break;
                    case 5: mainWindow.close(); break;
                    }
                }
            }
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
            mainWindow.close();
        }
        mainWindow.clear();
        mainWindow.draw(mainBackground);
        mainWindow.draw(titulText);
        menu.drawItems();
        mainWindow.display();
    }
}
void showHangManRules(RenderWindow& rules, int width, int height) {
    RectangleShape rulesBg(Vector2f(1920, 1080));//фон меню
    Texture windowTexture;
    windowTexture.loadFromFile("HangMan/hangmanRules.png");
    rulesBg.setTexture(&windowTexture);
    while (rules.isOpen())
    {
        Event event_opt;
        while (rules.pollEvent(event_opt))
        {
            if (event_opt.type == Event::Closed) rules.close();
            if (event_opt.type == Event::KeyPressed)
            {
                if (event_opt.key.code == Keyboard::Escape) {
                    hangmanMenu(rules, width, height);
                }
            }
        }
        rules.clear();
        rules.draw(rulesBg);
        rules.display();
    }
}

void hangmanMenu(RenderWindow& hangmanMenu, int width, int height) {
    //візуалізація меню
    RectangleShape mainBackground(Vector2f(width, height));//фон меню
    Texture windowTexture;
    windowTexture.loadFromFile("Images/hangmanMenu.png");
    mainBackground.setTexture(&windowTexture);
    //встановлюємо шрифт для навз категорій
    Font font;
    font.loadFromFile("Fonts/LeagueSpartan-Bold.ttf");
    Text titulText;
    titulText.setFont(font);
    setText(titulText, 400, 80, "Hangman menu", 150, Color(1, 13, 65), 10);
    //використовуємо клас MainGameMenu
    String menuNames[] = { "Body parts","Sports","Proffesions", "Rules", "Exit" };
    GameMenu menu(hangmanMenu, 950, 550, 5, menuNames, 100, 100);
    menu.setColorsForItems(Color(1, 13, 65), Color(217, 0, 5), Color(56, 108, 160));
    //початок гри

    while (hangmanMenu.isOpen()) {
        Event event;
        while (hangmanMenu.pollEvent(event)) {
            if (event.type == Event::KeyReleased) {
                if (event.key.code == Keyboard::Up) {
                    menu.moveUp();
                }
                if (event.key.code == Keyboard::Down) {
                    menu.moveDown();
                }
                if (event.key.code == Keyboard::Enter) {
                    switch (menu.getMenuSelected()) {
                    case 0: {Hangman hm("bodyParts.txt");
                        hm.startGame(); }break;
                    case 1: {Hangman hm("sports.txt");
                        hm.startGame(); }; break;
                    case 2: {Hangman hm("proffesions.txt");
                        hm.startGame(); }; break;
                    case 3:showHangManRules(hangmanMenu, width, height); break;
                    case 4: {
                        mainMenuStart(hangmanMenu, width, height);
                    } break;
                    }
                }
            }
        }
        hangmanMenu.clear();
        hangmanMenu.draw(mainBackground);
        hangmanMenu.draw(titulText);
        menu.drawItems();
        hangmanMenu.display();
    }
}

int checkersMenu(RenderWindow& window, int width, int height) {

    RectangleShape mainBackground(Vector2f(width, height));//фон меню
    Texture windowTexture;
    windowTexture.loadFromFile("Texture/fonMenu.jpg");
    mainBackground.setTexture(&windowTexture);
    //встановлюємо шрифт для навз категорій
    Font font;
    font.loadFromFile("Fonts/LeagueSpartan-Bold.ttf");
    Text titulText;
    titulText.setFont(font);
    setText(titulText, 480, 50, "Game menu", 150, Color(1, 13, 65), 10);
    //використовуємо клас MainGameMenu
    String menuNames[] = { "Play", "Exit" };//потім додамо ще назви
    GameMenu menu(window, 950, 550, 2, menuNames, 100, 200);
    menu.setColorsForItems(Color(1, 13, 65), Color(217, 0, 5), Color(56, 108, 160));
    //початок гри
    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::KeyReleased) {
                if (event.key.code == Keyboard::Up) {
                    menu.moveUp();
                }
                if (event.key.code == Keyboard::Down) {
                    menu.moveDown();
                }
                if (event.key.code == Keyboard::Enter) {
                    switch (menu.getMenuSelected()) {
                    case 0:startCheckers(window, width, height);
                    case 1:mainMenuStart(window, width, height);
                    }
                }
            }
        }
        window.clear();
        window.draw(mainBackground);
        window.draw(titulText);
        menu.drawItems();
        window.display();
    }
    return 0;
}

int startCheckers(RenderWindow& window, int width, int height) {
    ContextSettings settings;
    settings.antialiasingLevel = 16.0;
    Event event;
    Board board;
    int grid[8][8];
    Piece RedPieces[12];
    Piece WhitePieces[12];
    bool selected = false;
    Piece* SelectedPiece = NULL;
    int turn = 1;

    for (int i = 0; i < 12; i++) {
        WhitePieces[i].color = Color::White;
        RedPieces[i].color = Color::Red;
    }

    Setup(window, RedPieces, WhitePieces);

    while (window.isOpen()) {
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) {
                checkersMenu(window, width, height);
            }
            if (event.type == Event::MouseButtonReleased) {
                if (event.mouseButton.button == Mouse::Left) {
                    selected = !selected;
                }
            }
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
            checkersMenu(window, width, height);
        }
        window.clear();

        board.Draw(window);

        if (SelectedPiece != NULL) {
            board.Highlight(window, SelectedPiece->x, SelectedPiece->y);
        }

        for (int i = 0; i < 12; i++) {
            WhitePieces[i].Draw(window);
            RedPieces[i].Draw(window);
        }

        if (selected) {
            int x = Mouse::getPosition(window).x / 75;
            int y = Mouse::getPosition(window).y / 75;
            if (FindPiece(x, y, RedPieces, WhitePieces) && (FindPiece(x, y, RedPieces, WhitePieces)->color == Color::Red && turn == 1 || FindPiece(x, y, RedPieces, WhitePieces)->color == Color::White && turn == 2)) {
                if (FindPiece(x, y, RedPieces, WhitePieces) == SelectedPiece) {
                    SelectedPiece = NULL;
                }
                else {
                    SelectedPiece = FindPiece(x, y, RedPieces, WhitePieces);
                }

                selected = false;
            }
            else if (SelectedPiece != NULL && MovePiece(x, y, SelectedPiece, RedPieces, WhitePieces, &turn)) {
                selected = false;
                SelectedPiece = NULL;
            }
            selected = false;
        }
        for (int i = 0; i < 12; i++) {
            if (RedPieces[i].y == 0) {
                RedPieces[i].isKing = true;
            }
            if (WhitePieces[i].y == 7) {
                WhitePieces[i].isKing = true;
            }
        }


        window.display();
    }
    return 0;
}