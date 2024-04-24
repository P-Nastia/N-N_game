#include <iostream>

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Network.hpp>
#include "Board.h"

class Piece {
public:
    float radius = 30;
    int x;
    int y;
    bool isAlive;
    bool isKing = false;
    sf::Color color;

    void Draw(sf::RenderWindow& window) {
        if (isAlive) {
            sf::CircleShape shape(radius);
            shape.setFillColor(color);
            if (isKing) {
                shape.setOutlineThickness(3.f);
                shape.setOutlineColor(sf::Color::Yellow);
            }
            shape.setPosition(sf::Vector2f(x * 75 + (75 - radius * 2) / 2, y * 75 + (75 - 2 * radius) / 2));
            window.draw(shape);
        }
    }
};

inline void Setup(sf::RenderWindow& window, Piece* RedPieces, Piece* WhitePieces) {
    int m = 0;
    for (int i = 0; i < 3; i++) {
        for (int j = (int)!(i % 2); j < 8; j += 2) {
            WhitePieces[m].isAlive = true;
            WhitePieces[m].x = j;
            WhitePieces[m].y = i;
            m++;
        }

    }
    m = 0;
    for (int i = 5; i < 8; i++) {
        for (int j = (int)(i % 2); j < 8; j += 2) {
            RedPieces[m].isAlive = true;
            RedPieces[m].x = j;
            RedPieces[m].y = 7 - i;
            m++;
        }
    }
}


inline Piece* FindPiece(int x, int y, Piece* RedPieces, Piece* WhitePieces) {
    for (int i = 0; i < 12; i++) {
        if (RedPieces[i].x == x && RedPieces[i].y == y) {
            if (RedPieces[i].isAlive) {
                return &RedPieces[i];
            }
        }
        if (WhitePieces[i].x == x && WhitePieces[i].y == y) {
            if (WhitePieces[i].isAlive) {
                return &WhitePieces[i];
            }
        }
    }
    return NULL;
}

inline void AIPlayer(Piece* WhitePieces, Piece* RedPieces, int& turn) {
    // Отримуємо випадкову білу шашку
    int randomIndex = rand() % 12;
    Piece* selectedPiece = &WhitePieces[randomIndex];

    // Отримуємо випадковий напрямок руху
    int direction = rand() % 2 == 0 ? -1 : 1;

    // Перевіряємо чи можливий рух
    if (selectedPiece->isAlive) {
        // Перевіряємо чи можливий рух на діагоналі
        if (selectedPiece->x + direction >= 0 && selectedPiece->x + direction < 8 && selectedPiece->y + 1 >= 0 && selectedPiece->y + 1 < 8) {
            // Перевіряємо чи в цій позиції немає іншої шашки
            if (!FindPiece(selectedPiece->x + direction, selectedPiece->y + 1, RedPieces, WhitePieces)) {
                // Рухаємо шашку
                selectedPiece->x += direction;
                selectedPiece->y += 1;
                // Зміна ходу
                turn = (turn == 1) ? 2 : 1;
                return;
            }
        }
    }

    // Якщо не можливий жоден рух, AI пропускає хід
    turn = (turn == 1) ? 2 : 1;
}

inline void KillPiece(int x, int y, Piece* RedPieces, Piece* WhitePieces, int* turn) {
    FindPiece(x, y, RedPieces, WhitePieces)->isAlive = false;
    *turn = ((*turn == 1) ? 2 : 1);
    return;
}

inline int MovePiece(int x, int y, Piece* s_Piece, Piece* RedPieces, Piece* WhitePieces, int* turn) {
    if (s_Piece->color == sf::Color::Red || s_Piece->color == sf::Color::White && s_Piece->isKing) {
        if (x == s_Piece->x - 1 && y == s_Piece->y - 1) {
            if (!FindPiece(x, y, RedPieces, WhitePieces)) {
                *turn = ((*turn == 1) ? 2 : 1);
                s_Piece->x = x;
                s_Piece->y = y;
                return 1;
            }
        }
        if (x == s_Piece->x + 1 && y == s_Piece->y - 1) {
            if (!FindPiece(x, y, RedPieces, WhitePieces)) {
                *turn = ((*turn == 1) ? 2 : 1);
                s_Piece->x = x;
                s_Piece->y = y;
                return 1;
            }
        }
        if (x == s_Piece->x - 2 && y == s_Piece->y - 2) {
            if (!FindPiece(x, y, RedPieces, WhitePieces) && FindPiece(x + 1, y + 1, RedPieces, WhitePieces) != NULL && FindPiece(x + 1, y + 1, RedPieces, WhitePieces)->color != s_Piece->color) {
                *turn = ((*turn == 1) ? 2 : 1);
                KillPiece(x + 1, y + 1, RedPieces, WhitePieces, turn);
                s_Piece->x = x;
                s_Piece->y = y;
                return 1;
            }
        }
        if (x == s_Piece->x + 2 && y == s_Piece->y - 2) {
            if (!FindPiece(x, y, RedPieces, WhitePieces) && FindPiece(x - 1, y + 1, RedPieces, WhitePieces) != NULL && FindPiece(x - 1, y + 1, RedPieces, WhitePieces)->color != s_Piece->color) {
                *turn = ((*turn == 1) ? 2 : 1);
                KillPiece(x - 1, y + 1, RedPieces, WhitePieces, turn);
                s_Piece->x = x;
                s_Piece->y = y;
                return 1;
            }
        }
    }
    if (s_Piece->color == sf::Color::White || s_Piece->color == sf::Color::Red && s_Piece->isKing) {
        if (x == s_Piece->x - 1 && y == s_Piece->y + 1) {
            if (!FindPiece(x, y, RedPieces, WhitePieces)) {
                *turn = ((*turn == 1) ? 2 : 1);
                s_Piece->x = x;
                s_Piece->y = y;
                return 1;
            }
        }
        if (x == s_Piece->x + 1 && y == s_Piece->y + 1) {
            if (!FindPiece(x, y, RedPieces, WhitePieces)) {
                *turn = ((*turn == 1) ? 2 : 1);
                s_Piece->x = x;
                s_Piece->y = y;
                return 1;
            }
        }
        if (x == s_Piece->x - 2 && y == s_Piece->y + 2) {
            if (!FindPiece(x, y, RedPieces, WhitePieces) && FindPiece(x + 1, y - 1, RedPieces, WhitePieces) != NULL && FindPiece(x + 1, y - 1, RedPieces, WhitePieces)->color != s_Piece->color) {
                *turn = ((*turn == 1) ? 2 : 1);
                s_Piece->x = x;
                s_Piece->y = y;
                KillPiece(x + 1, y - 1, RedPieces, WhitePieces, turn);
                return 1;
            }
        }
        if (x == s_Piece->x + 2 && y == s_Piece->y + 2) {
            if (!FindPiece(x, y, RedPieces, WhitePieces) && FindPiece(x - 1, y - 1, RedPieces, WhitePieces) != NULL && FindPiece(x - 1, y - 1, RedPieces, WhitePieces)->color != s_Piece->color) {
                *turn = ((*turn == 1) ? 2 : 1);
                s_Piece->x = x;
                s_Piece->y = y;
                KillPiece(x - 1, y - 1, RedPieces, WhitePieces, turn);
                return 1;
            }
        }
    }
    return 0;
}