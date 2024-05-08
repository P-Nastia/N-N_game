#include <string>
#include <iomanip>
#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <fstream>
#include <ctime>
#include <Windows.h>
#include <map>
#include <io.h>
#include <cstdlib>
#include <set>
using namespace std;
using namespace sf;
#include "Hangman.h"
#include "GameMenu.h"
#include "BattleShip.h"
#include "gameText.h"
#include "Music.h"
#include "Board.h"
#include "Piece.h"
#include "Uno.h"

void enemiesTurn(gameText& text, BattleShip& enemy, BattleShip& player);
void playersTurn(gameText& text, BattleShip& enemy, BattleShip& player);
void setCardParameters(unoCards from, unoCards& to);
void playersTurn(Player& player, Opponent& opponent);
void opponentsTurn(Player& player, Opponent& opponent);
void ClientCode(RenderWindow& mainWindow, int width, int height);

/////////////
//патерн Singleton (одинак)

class SettingsManager {
private:
    static SettingsManager* instance;
    // Параметри налаштувань:
    int antialiasingLevel;

    // Закритий конструктор для обмеження створення екземплярів
    SettingsManager() : antialiasingLevel(16) {}

public:
    // Метод для отримання екземпляра класу
    static SettingsManager* getInstance() {
        if (instance == nullptr) {
            instance = new SettingsManager();
        }
        return instance;
    }

    // Метод для отримання значення налаштування antialiasingLevel
    int getAntialiasingLevel() {
        return antialiasingLevel;
    }

    // Метод для встановлення значення antialiasingLevel
    void setAntialiasingLevel(int level) {
        antialiasingLevel = level;
    }
};

// Ініціалізація статичного члена класу
SettingsManager* SettingsManager::instance = nullptr;

bool checkWin(Piece* RedPieces, Piece* WhitePieces) {
    // Перевірте, чи всі фішки одного кольору видалені з дошки
    int redCount = 0;
    int whiteCount = 0;

    for (int i = 0; i < 12; i++) {
        if (RedPieces[i].isAlive) {
            redCount++;
        }
        if (WhitePieces[i].isAlive) {
            whiteCount++;
        }
    }

    // Якщо одна з команд не має жодної фішки, поверніть true
    if (redCount == 0 || whiteCount == 0) {
        return true;
    }

    return false;
}
//////////////

namespace snake {
   
    // Типи клітинок на ігровому полі
#define FIELD_CELL_TYPE_NONE 0   // Пуста клітинка
#define FIELD_CELL_TYPE_APPLE -1 // Клітинка з яблуком
#define FIELD_CELL_TYPE_WALL -2  // Клітинка зі стіною
// Напрямки руху змійки
#define SNAKE_DIRECTION_UP 0     // Вгору
#define SNAKE_DIRECTION_RIGHT 1  // Вправо
#define SNAKE_DIRECTION_DOWN 2   // Вниз
#define SNAKE_DIRECTION_LEFT 3   // Вліво
// Пункти меню гри
#define MENU_ITEM_START "Start new game"
#define MENU_ITEM_RESUME "Resume game"
#define MENU_ITEM_SETTINGS "Settings"
#define MENU_ITEM_QUIT "Quit"
#define MENU_ITEM_BACK "Back to main menu"
#define MENU_ITEM_VOLUME "Volume"
// Коди для визначення поточного меню
#define MENU_MAIN 0              // Головне меню
#define MENU_SETTINGS 1          // Меню налаштувань
// Кількість життів гравця
#define LIVES 3
    const int field_size_x = 35;     //Ширина поля у клітинках
    const int field_size_y = 25;     //Висота поля у клітинках
    const int cell_size = 32;        //Розмір клітинки на полі(ширина та висота)
    const int score_bar_height = 60; //Висота панелі з рахунком та іншими інформ. елементами
    const int window_width = field_size_x * cell_size;   //Ширина вікна гри, залежна від розмірів поля та клітинок
    const int window_height = field_size_y * cell_size + score_bar_height;   //Висота вікна гри, залежна від розмірів поля, клітинок та висоти панелі рахунку

    // Структура для зберігання стану гри
    struct GameState {
        int field[field_size_y][field_size_x];   // Поле гри
        int snake_position_x;                    // Позиція змійки по горизонталі 
        int snake_position_y;                    // Позиція змійки по вертикалі 
        int snake_length = 0;                    // Довжина змійки
        int snake_direction = SNAKE_DIRECTION_RIGHT;// Напрямок руху змійки
        int score = 0;                           // Рахунок гравця
    };
    // Глобальні змінні гри
    GameState game_state;                       // Поточний стан гри
    std::vector<GameState> game_last_states;
    bool rollback = false;
    int lives = 0;
    bool game_started = false;
    bool game_over = false;
    int game_over_timeout = 0;                  // Таймаут після завершення гри
    bool game_paused = true;
    int current_menu = MENU_MAIN;               // Поточне меню (головне або налаштування)
    int current_main_menu_item_index = 0;       // Індекс поточного пункту меню (головного меню)
    int current_settings_menu_item_index = 0;   // Індекс поточного пункту меню (налаштування)
    int settings_volume = 50;                    // Гучність звуку

    // Графічні об'єкти та звукові ефекти
    // (текстури, спрайти, звукові буфери, звукові ефекти, шрифти та текстові об'єкти)
    sf::Texture snake_texture;
    sf::Sprite snake;
    sf::Texture head_texture;
    sf::Sprite head;
    sf::Texture none_texture;
    sf::Sprite none;
    sf::Texture apple_texture;
    sf::Sprite apple;
    sf::Texture wall_texture;
    sf::Sprite wall;
    sf::Texture life_texture;
    sf::Sprite life;
    sf::SoundBuffer sb_menu_navigate;
    sf::Sound sound_menu_navigate;
    sf::SoundBuffer sb_game_start;
    sf::Sound sound_game_start;
    sf::SoundBuffer sb_ate_apple;
    sf::Sound sound_ate_apple;
    sf::SoundBuffer sb_died_against_the_wall;
    sf::Sound sound_died_against_the_wall;
    sf::SoundBuffer sb_ate_himself;
    sf::Sound sound_ate_himself;
    sf::Font font_score;
    sf::Text text_score;
    sf::Font font_title;
    sf::Text text_title;
    sf::Font font_game_over;
    sf::Text text_game_over;

    // Меню гри
    sf::Font font_menu;    // Шрифт для тексту меню
    std::vector<sf::Text> text_main_menu_items;
    std::vector<std::string> main_menu_items = { MENU_ITEM_START, MENU_ITEM_SETTINGS, MENU_ITEM_QUIT };   // Пункти головного меню
    std::vector<sf::Text> text_settings_menu_items;
    std::vector<std::string> settings_menu_items = { MENU_ITEM_BACK, MENU_ITEM_VOLUME };   // Пункти меню налаштувань

    // Генерує випадкове пусте місце на полі.
    int get_random_empty_cell()
    {
        int empty_cell_count = 0;
        for (int j = 0; j < field_size_y; j++) {
            for (int i = 0; i < field_size_x; i++) {
                if (game_state.field[j][i] == FIELD_CELL_TYPE_NONE) {
                    empty_cell_count++;
                }
            }
        }
        int target_empty_cell_index = std::rand() % empty_cell_count;
        int empty_cell_index = 0;
        for (int j = 0; j < field_size_y; j++) {
            for (int i = 0; i < field_size_x; i++) {
                if (game_state.field[j][i] == FIELD_CELL_TYPE_NONE) {
                    if (empty_cell_index == target_empty_cell_index) {
                        return j * field_size_x + i;
                    }
                    empty_cell_index++;
                }
            }
        }
        return -1;
    }
    // Додає яблуко на поле випадковим чином.
    void add_apple()
    {
        int apple_pos = get_random_empty_cell();
        if (apple_pos != -1) {
            game_state.field[apple_pos / field_size_x][apple_pos % field_size_x] = FIELD_CELL_TYPE_APPLE;
        }
    }
    // Очищує поле та розміщує стіни та змійку.
    void clear_field()
    {
        for (int j = 0; j < field_size_y; j++) {
            for (int i = 0; i < field_size_x; i++) {
                game_state.field[j][i] = FIELD_CELL_TYPE_NONE;
            }
        }
        for (int i = 0; i < game_state.snake_length; i++) {
            game_state.field[game_state.snake_position_y][game_state.snake_position_x - i] = game_state.snake_length - i;
        }
        for (int i = 0; i < field_size_x; i++) {
            if (i < 5 || field_size_x - i - 1 < 5) {
                game_state.field[0][i] = FIELD_CELL_TYPE_WALL;
                game_state.field[field_size_y - 1][i] = FIELD_CELL_TYPE_WALL;
            }
        }
        for (int i = 1; i < field_size_y - 1; i++) {
            if (i < 5 || field_size_y - i - 1 < 5) {
                game_state.field[i][0] = FIELD_CELL_TYPE_WALL;
                game_state.field[i][field_size_x - 1] = FIELD_CELL_TYPE_WALL;
            }
        }
        add_apple();
    }
    //Встановлює гучність звукових ефектів відповідно до поточного рівня гучності.
    void set_volume()
    {
        sound_menu_navigate.setVolume(settings_volume);
        sound_game_start.setVolume(settings_volume);
        sound_ate_apple.setVolume(settings_volume);
        sound_died_against_the_wall.setVolume(settings_volume);
        sound_ate_himself.setVolume(settings_volume);
    }
    //Ініціалізує гру, завантажує текстури, налаштування та інші параметри.
    void init_game()
    {
        std::srand(time(NULL));
        clear_field();

        snake_texture.loadFromFile("images/snake.png");
        snake.setTexture(snake_texture);

        head_texture.loadFromFile("images/head.png");
        head.setTexture(head_texture);

        none_texture.loadFromFile("images/none.png");
        none.setTexture(none_texture);

        apple_texture.loadFromFile("images/apple.png");
        apple.setTexture(apple_texture);

        wall_texture.loadFromFile("images/wall.png");
        wall.setTexture(wall_texture);

        life_texture.loadFromFile("images/life.png");
        life.setTexture(life_texture);

        sb_menu_navigate.loadFromFile("sounds/menu-navigate-02.wav");
        sound_menu_navigate.setBuffer(sb_menu_navigate);

        sb_game_start.loadFromFile("sounds/jingle-achievement-01.wav");
        sound_game_start.setBuffer(sb_game_start);

        sb_ate_apple.loadFromFile("sounds/collect-point-01.wav");
        sound_ate_apple.setBuffer(sb_ate_apple);

        sb_died_against_the_wall.loadFromFile("sounds/explosion-02.wav");
        sound_died_against_the_wall.setBuffer(sb_died_against_the_wall);

        sb_ate_himself.loadFromFile("sounds/explosion-00.wav");
        sound_ate_himself.setBuffer(sb_ate_himself);

        set_volume();

        font_score.loadFromFile("fonts/ShockMintFund-YzA8v.ttf");
        text_score.setFont(font_score);
        text_score.setCharacterSize(36);
        text_score.setFillColor(sf::Color::Black);

        font_title.loadFromFile("fonts/BigfatScript-2OvA8.otf");
        text_title.setFont(font_title);
        text_title.setString("Snake");
        text_title.setCharacterSize(40);
        text_title.setFillColor(sf::Color::Black);
        text_title.setPosition(20, 0);

        font_game_over.loadFromFile("fonts/BigOldBoldy-dEjR.ttf");
        text_game_over.setFont(font_game_over);
        text_game_over.setString("GAME OVER");
        text_game_over.setCharacterSize(120);
        text_game_over.setFillColor(sf::Color::Red);
        text_game_over.setPosition((window_width - text_game_over.getLocalBounds().width) / 2, (window_height - text_game_over.getLocalBounds().height + score_bar_height) / 2);

        font_menu.loadFromFile("fonts/BigOldBoldy-dEjR.ttf");
        for (int i = 0; i < main_menu_items.size(); i++) {
            text_main_menu_items.emplace_back(sf::Text());
            text_main_menu_items.back().setString(main_menu_items.at(i));
            text_main_menu_items.back().setFont(font_menu);
            text_main_menu_items.back().setCharacterSize(40);
        }
        for (int i = 0; i < settings_menu_items.size(); i++) {
            text_settings_menu_items.emplace_back(sf::Text());
            text_settings_menu_items.back().setString(settings_menu_items.at(i));
            text_settings_menu_items.back().setFont(font_menu);
            text_settings_menu_items.back().setCharacterSize(40);
        }
    }
    //Починає нову гру, встановлює початкові значення та очищає поле.
    void start_game()
    {
        game_state.snake_position_x = field_size_x / 2;
        game_state.snake_position_y = field_size_y / 2;
        game_state.snake_length = 4;
        game_state.snake_direction = SNAKE_DIRECTION_RIGHT;
        game_state.score = 0;
        game_started = true;
        game_over = false;
        game_paused = false;
        lives = LIVES;
        clear_field();
        sound_game_start.play();
    }
    //Завершує гру, встановлює прапорець game_over та game_paused.
    void finish_game()
    {
        game_over = true;
        game_paused = true;
        game_over_timeout = 20;
        current_main_menu_item_index = 0;
    }
    //Обробляє смерть змійки.
    void snake_died()
    {
        lives--;
        if (lives > 0) {
            rollback = true;
        }
        else {
            finish_game();
        }
    }
    //Малює графічне представлення поля.
    void draw_field(sf::RenderWindow& window)
    {
        for (int j = 0; j < field_size_y; j++) {
            for (int i = 0; i < field_size_x; i++) {
                switch (game_state.field[j][i]) {
                case FIELD_CELL_TYPE_NONE:
                    none.setPosition(float(i * cell_size), float(j * cell_size + score_bar_height));
                    window.draw(none);
                    break;
                case FIELD_CELL_TYPE_APPLE:
                    apple.setPosition(float(i * cell_size), float(j * cell_size + score_bar_height));
                    window.draw(apple);
                    break;
                case FIELD_CELL_TYPE_WALL:
                    wall.setPosition(float(i * cell_size), float(j * cell_size + score_bar_height));
                    window.draw(wall);
                    break;
                default:
                    if (game_state.field[j][i] == game_state.snake_length) {
                        float offset_x = head.getLocalBounds().width / 2;
                        float offset_y = head.getLocalBounds().height / 2;
                        head.setPosition(float(i * cell_size + offset_x), float(j * cell_size + score_bar_height + offset_y));
                        head.setOrigin(offset_x, offset_y);
                        switch (game_state.snake_direction) {
                        case SNAKE_DIRECTION_UP:
                            head.setRotation(0);
                            break;
                        case SNAKE_DIRECTION_RIGHT:
                            head.setRotation(90);
                            break;
                        case SNAKE_DIRECTION_DOWN:
                            head.setRotation(180);
                            break;
                        case SNAKE_DIRECTION_LEFT:
                            head.setRotation(-90);
                            break;
                        }
                        window.draw(head);
                    }
                    else {
                        snake.setPosition(float(i * cell_size), float(j * cell_size + score_bar_height));
                        window.draw(snake);
                    }
                }
            }
        }
    }
    //Малює рахункову панель.
    void draw_score_bar(sf::RenderWindow& window)
    {
        window.draw(text_title);

        text_score.setString("Score: " + std::to_string(game_state.score));
        text_score.setPosition(window_width - text_score.getLocalBounds().width - 20, 10);
        window.draw(text_score);

        for (int i = 0; i < lives; i++) {
            life.setPosition((window_width - LIVES * 48) / 2 + i * 48, (score_bar_height - 48) / 2);
            window.draw(life);
        }
    }
    // Малює головне меню.
    void draw_main_menu(sf::RenderWindow& window)
    {
        float const menu_padding_horizontal = 40;
        float const menu_padding_vertical = 30;
        float const menu_item_interval = 20;

        float menu_item_max_width = 0;
        float current_menu_item_offset_y = 0;
        for (int i = 0; i < text_main_menu_items.size(); i++) {
            if (main_menu_items.at(i) == MENU_ITEM_START) {
                if (!game_over && game_started) {
                    text_main_menu_items.at(i).setString(MENU_ITEM_RESUME);
                }
                else {
                    text_main_menu_items.at(i).setString(MENU_ITEM_START);
                }
            }
            text_main_menu_items.at(i).setPosition(0, current_menu_item_offset_y);
            text_main_menu_items.at(i).setFillColor(current_main_menu_item_index == i ? sf::Color(224, 224, 224) : sf::Color(128, 128, 128));
            current_menu_item_offset_y += text_main_menu_items.at(i).getLocalBounds().height + menu_item_interval;
            menu_item_max_width = std::max(menu_item_max_width, text_main_menu_items.at(i).getLocalBounds().width);
        }

        float const menu_width = menu_item_max_width + menu_padding_horizontal * 2;
        float const menu_height = current_menu_item_offset_y - menu_item_interval + menu_padding_vertical * 2;

        float const menu_position_x = (window_width - menu_width) / 2;
        float const menu_position_y = (window_height - menu_height) / 2;

        sf::RectangleShape menu_rect(sf::Vector2f(menu_width, menu_height));
        menu_rect.setPosition(menu_position_x, menu_position_y);
        menu_rect.setFillColor(sf::Color(0, 0, 0, 224));
        window.draw(menu_rect);

        for (int i = 0; i < text_main_menu_items.size(); i++) {
            text_main_menu_items.at(i).move(menu_position_x + menu_padding_horizontal, menu_position_y + menu_padding_vertical);
            window.draw(text_main_menu_items.at(i));
        }
    }
    // Малює меню налаштувань.
    void draw_settings_menu(sf::RenderWindow& window)
    {
        float const menu_padding_horizontal = 40;
        float const menu_padding_vertical = 30;
        float const menu_item_interval = 20;

        float menu_item_max_width = 0;
        float current_menu_item_offset_y = 0;
        for (int i = 0; i < text_settings_menu_items.size(); i++) {
            if (settings_menu_items.at(i) == MENU_ITEM_VOLUME) {
                text_settings_menu_items.at(i).setString(settings_menu_items.at(i) + ": " + std::to_string(settings_volume));
            }
            text_settings_menu_items.at(i).setPosition(0, current_menu_item_offset_y);
            text_settings_menu_items.at(i).setFillColor(current_settings_menu_item_index == i ? sf::Color(224, 224, 224) : sf::Color(128, 128, 128));
            current_menu_item_offset_y += text_settings_menu_items.at(i).getLocalBounds().height + menu_item_interval;
            menu_item_max_width = std::max(menu_item_max_width, text_settings_menu_items.at(i).getLocalBounds().width);
        }

        float const menu_width = menu_item_max_width + menu_padding_horizontal * 2;
        float const menu_height = current_menu_item_offset_y - menu_item_interval + menu_padding_vertical * 2;

        float const menu_position_x = (window_width - menu_width) / 2;
        float const menu_position_y = (window_height - menu_height) / 2;

        sf::RectangleShape menu_rect(sf::Vector2f(menu_width, menu_height));
        menu_rect.setPosition(menu_position_x, menu_position_y);
        menu_rect.setFillColor(sf::Color(0, 0, 0, 224));
        window.draw(menu_rect);

        for (int i = 0; i < text_settings_menu_items.size(); i++) {
            text_settings_menu_items.at(i).move(menu_position_x + menu_padding_horizontal, menu_position_y + menu_padding_vertical);
            window.draw(text_settings_menu_items.at(i));
        }
    }
    //Збільшує довжину змійки.
    void grow_snake()
    {
        for (int j = 0; j < field_size_y; j++) {
            for (int i = 0; i < field_size_x; i++) {
                if (game_state.field[j][i] > FIELD_CELL_TYPE_NONE) {
                    game_state.field[j][i]++;
                }
            }
        }
    }
    //Виконує крок гри, переміщує змійку та перевіряє умови перемоги або поразки.
    void make_move()
    {
        game_last_states.push_back(game_state);
        if (game_last_states.size() > 10) {
            game_last_states.erase(game_last_states.begin());
        }
        switch (game_state.snake_direction) {
        case SNAKE_DIRECTION_UP:
            game_state.snake_position_y--;
            if (game_state.snake_position_y < 0) {
                game_state.snake_position_y = field_size_y - 1;
            }
            break;
        case SNAKE_DIRECTION_RIGHT:
            game_state.snake_position_x++;
            if (game_state.snake_position_x > field_size_x - 1) {
                game_state.snake_position_x = 0;
            }
            break;
        case SNAKE_DIRECTION_DOWN:
            game_state.snake_position_y++;
            if (game_state.snake_position_y > field_size_y - 1) {
                game_state.snake_position_y = 0;
            }
            break;
        case SNAKE_DIRECTION_LEFT:
            game_state.snake_position_x--;
            if (game_state.snake_position_x < 0) {
                game_state.snake_position_x = field_size_x - 1;
            }
            break;
        }

        if (game_state.field[game_state.snake_position_y][game_state.snake_position_x] != FIELD_CELL_TYPE_NONE) {
            switch (game_state.field[game_state.snake_position_y][game_state.snake_position_x]) {
            case FIELD_CELL_TYPE_APPLE:
                sound_ate_apple.play();
                game_state.snake_length++;
                game_state.score++;
                grow_snake();
                add_apple();
                break;
            case FIELD_CELL_TYPE_WALL:
                sound_died_against_the_wall.play();
                snake_died();
                break;
            default:
                if (game_state.field[game_state.snake_position_y][game_state.snake_position_x] > 1) {
                    sound_ate_himself.play();
                    snake_died();
                }
            }
        }

        if (!game_over && !rollback) {
            for (int j = 0; j < field_size_y; j++) {
                for (int i = 0; i < field_size_x; i++) {
                    if (game_state.field[j][i] > FIELD_CELL_TYPE_NONE) {
                        game_state.field[j][i]--;
                    }
                }
            }
            game_state.field[game_state.snake_position_y][game_state.snake_position_x] = game_state.snake_length;
        }
    }
}

namespace ticTacToe {
    const int FIELD_SIZE = 9; /*розмір ігрового поля*/
    const int INIT_WEIGHT = 100; /*Початкова вага матриці ваг*/
    const int PRECISION_COEF = 75; /*Точність генератора ходу для Smart гравця*/
    const double STEP_COEF = 0.75; /*Коефіцієнт навчання*/
    const int STEP_LEARN = 20; /*Крок навчання*/
    const int NUMBER_OF_GAMES = 10000; /*Кількість ігор які має зіграти Smart гравець для навчання*/
    const bool stat_smart = true; /*Відображати статистику після навчання Smart*/

    char Field[FIELD_SIZE]; /*масив під ігрове поле*/
    int size_database_X = 0;/*Розмір бази даних X*/
    int size_database_O = 0;/*Розмір бази даних O*/

    struct DataBase /*База знань smart гравця*/
    {
        char MyField[9]; //Ситуація на полі
        int MyWeight[9]; //Матриця ваг
        DataBase() {
            std::fill(MyWeight, MyWeight + FIELD_SIZE, INIT_WEIGHT);
        }
    };

    DataBase* Collections_X = new DataBase[size_database_X];/*База для X комбінацій*/
    DataBase* Collections_O = new DataBase[size_database_O];/*База для О комбінацій*/

    struct Stack /*Історія виконаних ходів Smart гравця в поточній партії для функції Learn*/
    {
        int current_move;
        int index_weight;
    };

    struct Wins /*Містить переможну комбінацію та хто виграв*/
    {
        int mas[3] = { 0 };
        char xod;
    };

    void setup(int*, bool*); /*Функція ініціалізації прапорних змінних*/
    void type_symbol(bool*, char*, char*, int); /*Функція рандомно визначає хто гратиме за X а хтось за O*/
    void clear_field(); /*Функція очищує ігрові поля*/
    int random_player(); /*Функція повертає можливий хід випадково у стратегії Random*/
    int input_events(bool*, int, Stack**, int*, char, char); /*Функція повертає хід зроблений користувачем з клавіатури, і виводить варіанти ходу, що залишилися.*/
    void check_wins(int*, int, int*, int*, int*, bool*, Wins*); /*Функція перевіряє на перемогу після кожного ходу*/
    void game_logic(int, int*, bool*, int, char, char, int*, int*, int*, bool*, Stack**, int*, Wins*); /*Функція логіки гри*/
    DataBase* push_database(DataBase*, int*); /*Додає до бази даних невідому ситуацію на полі*/
    Stack* push_stack(int, int, Stack*, int*); /*Додає до Stack поточний хід гравця Smart*/
    int get_situation(DataBase*, int); /*Шукає в базі ситуації, що склалася на полі*/
    int get_smart_random(int, DataBase*); /*Генерує хід Smart гравця на підставі матриці ваг*/
    void smart_learn(Stack*, int, int, DataBase*); /*Рекурсивна функція навчання, зменшує вагу ходу у разі програшу, і збільшує навпаки, нічия нейтрально*/
    void display_statistic(RenderWindow&, int, int, int); /*Виводить статистику*/
    void menu_graph(RenderWindow&, int*, Event, bool game_over = false); /*Виводить меню гри*/
    void display_field(RenderWindow&, char, char, bool game_over = false); /*Виводить ігрову підлогу*/
    int move_human(RenderWindow&); /*Повертає перебіг людини*/
    void wins_victory(RenderWindow&, Wins*); /*Виводить смугу при перемозі*/
    void progress_bar(RenderWindow&, int); /*Виводить ProgressBar під час навчання Smart*/

    int input_events(bool* turn, int type_game, Stack** Hystory, int* stack_size, char player_1, char player_2)
    {
        int move = 0, index;

        if ((*turn && type_game == 3) || type_game == 2)
        {
            if (player_1 == 'X' && player_2 == 'O' && type_game == 2)
            {
                if (get_situation(Collections_O, size_database_O) == -1)
                    Collections_O = push_database(Collections_O, &size_database_O);

                index = get_situation(Collections_O, size_database_O);

                move = get_smart_random(index, Collections_O);
            }
            else if (player_1 == 'O' && player_2 == 'X' && type_game == 2)
            {
                if (get_situation(Collections_X, size_database_X) == -1)
                    Collections_X = push_database(Collections_X, &size_database_X);

                index = get_situation(Collections_X, size_database_X);

                move = get_smart_random(index, Collections_X);
            }
            else if (player_1 == 'O' && player_2 == 'X' && type_game == 3)
            {
                if (get_situation(Collections_O, size_database_O) == -1)
                    Collections_O = push_database(Collections_O, &size_database_O);

                index = get_situation(Collections_O, size_database_O);

                move = get_smart_random(index, Collections_O);
            }
            else if (player_1 == 'X' && player_2 == 'O' && type_game == 3)
            {
                if (get_situation(Collections_X, size_database_X) == -1)
                    Collections_X = push_database(Collections_X, &size_database_X);

                index = get_situation(Collections_X, size_database_X);

                move = get_smart_random(index, Collections_X);
            }

            *Hystory = push_stack(move, index, *Hystory, stack_size);

            return move;
        }
        else
        {
            return random_player();
        }
        return move;
    }

    void game_logic(int move, int* draw, bool* turn, int type_game, char player_1, char player_2,
        int* x_wins, int* o_wins, int* d_wins, bool* game_over, Stack** Hystory, int* stack_size, Wins* Win)
    {
        bool wins = false;
        if ((*turn && type_game == 1) || (*turn && type_game == 2))/*ó��� ������*/
        {
            Field[move] = player_1;
            check_wins(draw, type_game, x_wins, o_wins, d_wins, &wins, Win);
            if (wins)
            {
                if (Win->xod != 'D' && type_game == 2 && player_1 == 'X')
                {
                    smart_learn(*Hystory, *stack_size - 1, -STEP_LEARN, Collections_O);
                }
                else if (Win->xod != 'D' && type_game == 2 && player_1 == 'O')
                {
                    smart_learn(*Hystory, *stack_size - 1, -STEP_LEARN, Collections_X);
                }
                *game_over = true;
                return;
            }
            *turn = false;
        }
        else /*ó��� Random ������*/
        {
            Field[move] = player_2;
            check_wins(draw, type_game, x_wins, o_wins, d_wins, &wins, Win);
            if (wins)
            {
                if (Win->xod != 'D' && type_game != 1 && player_2 == 'X')
                {
                    smart_learn(*Hystory, *stack_size - 1, -STEP_LEARN, Collections_O);
                }
                else if (Win->xod != 'D' && type_game != 1 && player_2 == 'O')
                {
                    smart_learn(*Hystory, *stack_size - 1, -STEP_LEARN, Collections_X);
                }
                *game_over = true;
                return;
            }
            *turn = true;
        }
    }

    void check_wins(int* draw, int type_game, int* x_wins, int* o_wins, int* d_wins, bool* wins, Wins* Win)
    {
        int victory[8][3] = { {0, 1 , 2}, {3, 4, 5}, {6, 7, 8},
        {0, 3, 6}, {1, 4, 7}, {2, 5, 8}, {0, 4, 8}, {2, 4, 6} };

        for (int i = 0; i < FIELD_SIZE - 1; i++)
        {
            if (Field[victory[i][0]] == Field[victory[i][1]] &&
                Field[victory[i][0]] == Field[victory[i][2]] &&
                Field[victory[i][0]] != ' ')
            {
                *wins = true;
                if (stat_smart == true && type_game == 3)
                    Field[victory[i][0]] == 'X' ? (*x_wins)++ : (*o_wins)++;
                else if (type_game != 3)
                    Field[victory[i][0]] == 'X' ? (*x_wins)++ : (*o_wins)++;
                for (int j = 0, k = 0; j < 3; j++, k++)
                {
                    Win->mas[j] = victory[i][k];
                }
                Field[victory[i][0]] == 'X' ? Win->xod = 'X' : Win->xod = 'O';
                return;
            }
        }
        if (*draw == 9)
        {
            *wins = true;
            if (stat_smart == true && type_game == 3)
                (*d_wins)++;
            else if (type_game != 3)
                (*d_wins)++;
            for (int i = 0; i < 3; i++) { Win->mas[i] = 0; }
            Win->xod = 'D';
            return;
        }
        Win->xod = 'U';
    }

    void smart_learn(Stack* Hystory, int iterator, int STEP_LEARN, DataBase* Collections)
    {
        if (iterator < 0)
            return;

        Collections[Hystory[iterator].index_weight].MyWeight[Hystory[iterator].current_move] += STEP_LEARN;

        if ((Collections[Hystory[iterator].index_weight].MyWeight[Hystory[iterator].current_move]) < 0)
        {
            Collections[Hystory[iterator].index_weight].MyWeight[Hystory[iterator].current_move] = 0;
        }

        smart_learn(Hystory, --iterator, STEP_LEARN * STEP_COEF, Collections);
    }

    Stack* push_stack(int move, int index, Stack* Hystory, int* stack_size)
    {
        Stack* Temp = new Stack[*stack_size + 1];

        for (int i = 0; i < *stack_size; i++)
        {
            Temp[i] = Hystory[i];
        }

        Temp[*stack_size].current_move = move;
        Temp[*stack_size].index_weight = index;

        delete[] Hystory;

        (*stack_size)++;

        return Temp;
    }

    DataBase* push_database(DataBase* Collections, int* size_database)
    {
        DataBase* Temp = new DataBase[*size_database + 1];

        for (int i = 0; i < *size_database; i++)
        {
            Temp[i] = Collections[i];
        }

        strcpy_s(Temp[*size_database].MyField, Field);

        for (int i = 0; i < FIELD_SIZE; i++)
        {
            if (strncmp(&Temp[*size_database].MyField[i], " ", 1) != 0)
            {
                Temp[*size_database].MyWeight[i] = 0;
            }
        }

        delete[] Collections;

        (*size_database)++;

        return Temp;
    }

    int get_situation(DataBase* Collections, int size_database)
    {

        for (int i = 0; i < size_database; i++)
        {
            if (strcmp(Collections[i].MyField, Field) == 0)
            {
                return i;
            }
        }
        return -1;
    }

    int get_smart_random(int index, DataBase* Collections)
    {
        int summ = 0, count = 0, move = 0;
        int mas_temp[9];

        for (int i = 0; i < FIELD_SIZE; i++)
        {
            summ += Collections[index].MyWeight[i];
            mas_temp[i] = Collections[index].MyWeight[i];
        }

        if (summ == 0) return random_player();

        for (int i = 0; i < FIELD_SIZE; i++)
        {
            mas_temp[i] = (mas_temp[i] / (double)summ) * PRECISION_COEF;
        }
        for (int i = 0; i < FIELD_SIZE; i++)
        {
            count += mas_temp[i];
        }

        int* new_mas = new int[count];

        for (int i = 0, j = 0; i < FIELD_SIZE; i++)
        {
            int tmp = mas_temp[i];
            while (tmp != 0)
            {
                new_mas[j] = i;
                j++;
                tmp--;
            }
        }

        std::random_shuffle(new_mas, new_mas + count);

        move = new_mas[rand() % count];

        delete[] new_mas;

        return move;
    }

    int random_player()
    {
        int move = 0, count = 0;

        for (int i = 0; i < FIELD_SIZE; i++)
        {
            if (Field[i] == ' ') count++;
        }

        int* pTmp = new int[count];

        for (int i = 0, j = 0; i < FIELD_SIZE; i++)
        {
            if (Field[i] == ' ')
            {
                pTmp[j] = i;
                j++;
            }
        }

        move = pTmp[rand() % count];

        delete[] pTmp;

        return move;
    }

    void setup(int* draw, bool* game_over)
    {
        *game_over = false;
        *draw = 0;
        clear_field();
    }

    void type_symbol(bool* turn, char* player_1, char* player_2, int type_game)
    {
        int type_symbol = rand() % 2 + 1;

        if (type_symbol == 2)
        {
            *player_1 = 'O'; *player_2 = 'X';
            *turn = false;
        }
        else
        {
            *player_1 = 'X'; *player_2 = 'O';
            *turn = true;
        }
    }

    void clear_field()
    {
        for (int i = 0; i < FIELD_SIZE; i++)
        {
            Field[i] = ' ';
        }
    }

    void display_statistic(RenderWindow& window, int x_wins, int o_wins, int d_wins)
    {
        Text text_2, text_3, text_4, text_5, text_6, text_7, text_8, text_9;
        Font font;
        font.loadFromFile("comic.ttf");

        char x[7]; char o[7]; char d[7]; char s[7];

        int summ = x_wins + o_wins + d_wins;

        for (auto it : { &text_2, &text_3, &text_4, &text_8, &text_9 })
        {
            it->setFont(font);
            it->setCharacterSize(25);
        }

        for (auto it : { &text_5, &text_6, &text_7 })
        {
            it->setFont(font);
            it->setCharacterSize(35);
            it->setFillColor(Color(65, 105, 255));
        }

        for (auto it : { &text_2, &text_3, &text_4 })
        {
            it->setFillColor(Color(65, 105, 255));
            it->setStyle(Text::Underlined);
        }

        text_3.setFillColor(Color(220, 20, 60));
        text_6.setFillColor(Color(220, 20, 60));
        text_8.setFillColor(Color(34, 139, 34));
        text_9.setFillColor(Color(34, 139, 34));

        text_2.setString("Wins X");
        text_3.setString("Wins O");
        text_4.setString("Draws");
        text_9.setString("Games in total: ");


        _itoa_s(x_wins, x, 10);
        _itoa_s(o_wins, o, 10);
        _itoa_s(d_wins, d, 10);
        _itoa_s(summ, s, 10);

        text_5.setString(x);
        text_6.setString(o);
        text_7.setString(d);
        text_8.setString(s);

        //+20\+50
        text_2.setPosition(95, 450);//
        text_3.setPosition(375, 450);//
        text_4.setPosition(670, 450);//
        text_5.setPosition(95, 500);//
        text_6.setPosition(375, 500);//
        text_7.setPosition(670, 500);//
        text_8.setPosition(520, 570);//
        text_9.setPosition(270, 570);//

        window.draw(text_9); window.draw(text_2);
        window.draw(text_3); window.draw(text_4);
        window.draw(text_5); window.draw(text_6);
        window.draw(text_7); window.draw(text_8);
    }

    void menu_graph(RenderWindow& window, int* type_game, Event event, bool game_over)
    {
        Texture logo;
        Sprite spritelogo;
        logo.loadFromFile("logo.png");
        spritelogo.setTexture(logo);
        spritelogo.setPosition(10, 60);

        Text menu_0, menu_1, menu_2, menu_3, menu_4;
        Font font;
        font.loadFromFile("comic.ttf");

        for (auto it : { &menu_0, &menu_1, &menu_2, &menu_3, &menu_4 })
        {
            it->setFont(font);
            it->setCharacterSize(30);
            it->setFillColor(Color(255, 140, 0));
        }

        menu_0.setFillColor(Color::White);
        menu_4.setFillColor(Color(255, 20, 147));

        menu_0.setStyle(Text::Underlined);

        menu_0.setString(" Menu ");
        menu_1.setString("Play");
        menu_4.setString("Exit");

        menu_0.setPosition(600, 50);//
        menu_1.setPosition(600, 130);//
        menu_4.setPosition(600, 190);//

        if (IntRect(600, 130, 320, 30).contains(Mouse::getPosition(window)))//
        {
            menu_1.setFillColor(Color::Blue);
            if (event.type == Event::MouseButtonReleased)
                if (event.mouseButton.button == Mouse::Left) *type_game = 1;
        }

        if (IntRect(600, 190, 320, 30).contains(Mouse::getPosition(window)))//
        {
            menu_4.setFillColor(Color::Black);
            if (event.type == Event::MouseButtonReleased)
                if (event.mouseButton.button == Mouse::Left) {
                    ClientCode(window, 1920, 1080);
                }
        }

        window.draw(menu_0); window.draw(menu_1);
        window.draw(menu_2); window.draw(menu_3);
        window.draw(menu_4);
        if (!game_over)
        {
            window.draw(spritelogo);
        }
    }

    void display_field(RenderWindow& window, char player_1, char player_2, bool game_over)
    {
        RectangleShape line_1(Vector2f(300.f, 3.f)), line_2(Vector2f(300.f, 3.f));
        RectangleShape line_3(Vector2f(300.f, 3.f)), line_4(Vector2f(300.f, 3.f));

        for (auto it : { &line_1, &line_2, &line_3, &line_4 })
        {
            it->setFillColor(Color::Black);
        }

        line_1.rotate(90); line_2.rotate(90);

        line_1.move(150, 50); line_2.move(250, 50);
        line_3.move(50, 150); line_4.move(50, 250);

        window.draw(line_1); window.draw(line_2);
        window.draw(line_3); window.draw(line_4);

        Text X, O, type_char_1, type_char_2, text_1, text_2, text_3;
        Font font;
        font.loadFromFile("comic.ttf");

        text_1.setFont(font);
        text_1.setCharacterSize(30);
        text_1.setString(" You`re playing for ");
        text_2.setFont(font);
        text_2.setCharacterSize(30);
        text_2.setString("Opponent is playing for ");
        text_3.setFont(font);
        text_3.setCharacterSize(30);
        text_3.setString(" Xs take first turn!");
        text_3.setFillColor(Color(65, 105, 255));

        type_char_1.setFont(font);
        type_char_1.setCharacterSize(75);
        type_char_1.setString(player_1);
        type_char_2.setFont(font);
        type_char_2.setCharacterSize(75);
        type_char_2.setString(player_2);

        if (player_1 == 'X') {
            type_char_1.setFillColor(Color(65, 105, 255));
            type_char_2.setFillColor(Color(220, 20, 60));
            text_1.setFillColor(Color(65, 105, 255));
            text_2.setFillColor(Color(220, 20, 60));
        }
        else {
            type_char_1.setFillColor(Color(220, 20, 60));
            type_char_2.setFillColor(Color(65, 105, 255));
            text_1.setFillColor(Color(220, 20, 60));
            text_2.setFillColor(Color(65, 105, 255));
        }
        text_1.setPosition(400, 75);
        type_char_1.setPosition(550, 105);
        text_2.setPosition(450, 190);
        type_char_2.setPosition(550, 220);
        text_3.setPosition(425, 310);

        if (!game_over)
        {
            window.draw(type_char_1);
            window.draw(type_char_2);
            window.draw(text_1);
            window.draw(text_2);
            window.draw(text_3);
        }

        for (auto it : { &X, &O })
        {
            it->setFont(font);
            it->setCharacterSize(100);
        }

        X.setFillColor(Color(65, 105, 255));
        O.setFillColor(Color(220, 20, 60));

        X.setString("X"); O.setString("O");

        for (int i = 0; i < FIELD_SIZE; i++)
        {
            if (Field[i] == 'X')
            {
                if (i == 0) { X.setPosition(60, 35); window.draw(X); }
                if (i == 1) { X.setPosition(160, 35); window.draw(X); }
                if (i == 2) { X.setPosition(260, 35); window.draw(X); }

                if (i == 3) { X.setPosition(60, 135); window.draw(X); }
                if (i == 4) { X.setPosition(160, 135); window.draw(X); }
                if (i == 5) { X.setPosition(260, 135); window.draw(X); }

                if (i == 6) { X.setPosition(60, 235); window.draw(X); }
                if (i == 7) { X.setPosition(160, 235); window.draw(X); }
                if (i == 8) { X.setPosition(260, 235); window.draw(X); }
            }
            else if (Field[i] == 'O')
            {
                if (i == 0) { O.setPosition(60, 35); window.draw(O); }
                if (i == 1) { O.setPosition(160, 35); window.draw(O); }
                if (i == 2) { O.setPosition(260, 35); window.draw(O); }

                if (i == 3) { O.setPosition(60, 135); window.draw(O); }
                if (i == 4) { O.setPosition(160, 135); window.draw(O); }
                if (i == 5) { O.setPosition(260, 135); window.draw(O); }

                if (i == 6) { O.setPosition(60, 235); window.draw(O); }
                if (i == 7) { O.setPosition(160, 235); window.draw(O); }
                if (i == 8) { O.setPosition(260, 235); window.draw(O); }
            }
        }
    }

    int move_human(RenderWindow& window)
    {
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
                window.close();
        }
        if (IntRect(60, 60, 80, 80).contains(Mouse::getPosition(window)))
            if (Mouse::isButtonPressed(Mouse::Left)) return 0;
        if (IntRect(160, 60, 80, 80).contains(Mouse::getPosition(window)))
            if (Mouse::isButtonPressed(Mouse::Left)) return 1;
        if (IntRect(260, 60, 80, 80).contains(Mouse::getPosition(window)))
            if (Mouse::isButtonPressed(Mouse::Left)) return 2;
        if (IntRect(60, 160, 80, 80).contains(Mouse::getPosition(window)))
            if (Mouse::isButtonPressed(Mouse::Left)) return 3;
        if (IntRect(160, 160, 80, 80).contains(Mouse::getPosition(window)))
            if (Mouse::isButtonPressed(Mouse::Left)) return 4;
        if (IntRect(260, 160, 80, 80).contains(Mouse::getPosition(window)))
            if (Mouse::isButtonPressed(Mouse::Left)) return 5;
        if (IntRect(60, 260, 80, 80).contains(Mouse::getPosition(window)))
            if (Mouse::isButtonPressed(Mouse::Left)) return 6;
        if (IntRect(160, 260, 80, 80).contains(Mouse::getPosition(window)))
            if (Mouse::isButtonPressed(Mouse::Left)) return 7;
        if (IntRect(260, 260, 80, 80).contains(Mouse::getPosition(window)))
            if (Mouse::isButtonPressed(Mouse::Left)) return 8;

        return -1;
    }

    void wins_victory(RenderWindow& window, Wins* Win)
    {
        RectangleShape line(Vector2f(300.f, 5.f));
        RectangleShape line_2(Vector2f(425.f, 5.f));

        line.setFillColor(Color(0, 153, 102));
        line_2.setFillColor(Color(0, 153, 102));

        if (std::set<int>(Win->mas, Win->mas + 3) == std::set<int>{0, 1, 2})
        {
            line.setPosition(50, 100);
            window.draw(line);
        }
        if (std::set<int>(Win->mas, Win->mas + 3) == std::set<int>{3, 4, 5})
        {
            line.setPosition(50, 200);
            window.draw(line);
        }
        if (std::set<int>(Win->mas, Win->mas + 3) == std::set<int>{6, 7, 8})
        {
            line.setPosition(50, 300);
            window.draw(line);
        }
        if (std::set<int>(Win->mas, Win->mas + 3) == std::set<int>{0, 3, 6})
        {
            line.rotate(90); line.setPosition(100, 50);
            window.draw(line);
        }
        if (std::set<int>(Win->mas, Win->mas + 3) == std::set<int>{1, 4, 7})
        {
            line.rotate(90); line.setPosition(200, 50);
            window.draw(line);
        }
        if (std::set<int>(Win->mas, Win->mas + 3) == std::set<int>{2, 5, 8})
        {
            line.rotate(90); line.setPosition(300, 50);
            window.draw(line);
        }
        if (std::set<int>(Win->mas, Win->mas + 3) == std::set<int>{0, 4, 8})
        {
            line_2.rotate(45); line_2.setPosition(50, 50);
            window.draw(line_2);
        }
        if (std::set<int>(Win->mas, Win->mas + 3) == std::set<int>{2, 4, 6})
        {
            line_2.rotate(135); line_2.setPosition(350, 50);
            window.draw(line_2);
        }
    }

    void progress_bar(RenderWindow& window, int line_bar)
    {
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
                window.close();
        }

        RectangleShape progressBar;
        progressBar.setFillColor(Color(127, 255, 212));
        progressBar.setOutlineThickness(2);
        progressBar.setOutlineColor(Color::Black);
        progressBar.setPosition(50, 20);
        progressBar.setSize(Vector2f(line_bar, 20));

        window.draw(progressBar);
        window.display();
    }
}

auto coordsForOpponent(multimap<int, int>& map, int pos) {
    int count = 0;
    int x, y;
    for (auto m : map) {
        count++;
        if (pos == count) {
            return m;
        }
    }
}
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
void startHangman(string hmType, RenderWindow& window, int width, int height);

class Strategy
{
public:
    virtual ~Strategy() {}
    virtual void DoAlgorithm(RenderWindow& window, int width, int height) const = 0;
};

class Context
{
private:
    Strategy* strategy_;
public:
    Context(Strategy* strategy = nullptr) : strategy_(strategy)
    {
    }
    ~Context()
    {
        delete this->strategy_;
    }
    void set_strategy(Strategy* strategy)
    {
        delete this->strategy_;
        this->strategy_ = strategy;
    }
    void DoSomeBusinessLogic(RenderWindow& mainWindow, int width, int height) const
    {
        this->strategy_->DoAlgorithm(mainWindow, width, height);
    }
};

class BattleShipGame : public Strategy
{
public:

    void DoAlgorithm(RenderWindow& window, int width, int height) const override
    {
        SetConsoleCP(1251);
        SetConsoleOutputCP(1251);
        srand(time(NULL));
        BattleShip player("player");
        BattleShip enemy("enemy");
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
        multimap<int, int> shotsForBot;//всі можливі ходи для бота
        for (int x = 0; x < 10; x++) {
            for (int y = 0; y < 10; y++) {
                shotsForBot.emplace(x, y);
            }
        }
        int x, y;
        int playerX, playerY;
        int countOfDestroyed_by_player = 0, countOfDestroyed_by_opponent = 0;
        pair<int, int> p;//пара, куди будуть вертатися координати з coordsForOpponent
        int shotsCount = 100;//кількфсть всіх ходів
        gameText playerShoot(to_string(countOfDestroyed_by_player) + "/10", "Fonts/LeagueSpartan-Bold.ttf");
        playerShoot.setFontsize(40);
        playerShoot.setColor(Color::Black);
        gameText opponentShoot(to_string(countOfDestroyed_by_opponent) + "/10", "Fonts/LeagueSpartan-Bold.ttf");
        opponentShoot.setFontsize(40);
        opponentShoot.setColor(Color::Black);
        Texture gameOverTexture;//картинка для закінчення гри
        Sprite gameOver;
        gameOverTexture.loadFromFile("battleShip/gameOver.png");
        gameOver.setTexture(gameOverTexture);
        gameOver.setScale(Vector2f(0.8f, 0.8f));
        gameOver.setPosition(Vector2f(450.f, 250.f));
        while (window.isOpen()) {
            Event event;
            while (window.pollEvent(event)) {
                if (event.type == Event::Closed) window.close();
                if (event.type == Event::KeyReleased) {

                }
            }
            if (enemy.isMyTurn() == true) {
                num = rand() % shotsCount;
                p = coordsForOpponent(shotsForBot, num);
                x = p.first;
                y = p.second;
                if ((x >= 0 && x <= 9) && (y >= 0 && y <= 9)) {
                    shotsCount--;
                    auto it = find_if(//лямбда, щоб видалити пару з multimap
                        shotsForBot.begin(), shotsForBot.end(), [&](const auto& pair) {
                            return pair.first == x
                            && pair.second == y;
                        });

                    if (it != shotsForBot.end()) {
                        shotsForBot.erase(it);
                    }
                    Sleep(1000);
                    if (player.checkForCorrectness(x, y, window) == true) {
                        enemiesTurn(textForTurn, enemy, player);
                        if (player.checkForCompleteness(x, y) == true) {
                            countOfDestroyed_by_opponent++;
                            opponentShoot.setString(to_string(countOfDestroyed_by_opponent) + "/10");
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
                else {
                    playersTurn(textForTurn, enemy, player);
                    textForShots.setString(" ");
                }
            }
            if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                sf::Vector2i playPos = { (int)((Mouse::getPosition().x / 58) - 18),(int)((Mouse::getPosition().y / 58) - 4) };
                playerX = playPos.x;
                playerY = playPos.y;
                if (enemy.checkForRepiteness(playerX, playerY) == true) {
                    textForTurn.setString("Pick other coordinates");
                }
                else {
                    if (enemy.checkForCorrectness(playerX, playerY, window) == true) {
                        playersTurn(textForTurn, enemy, player);
                        if (enemy.checkForCompleteness(playerX, playerY) == true) {
                            countOfDestroyed_by_player++;
                            playerShoot.setString(to_string(countOfDestroyed_by_player) + "/10");
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
                ClientCode(window, width, height);
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
            playerShoot.showString(window, 1800, 20);
            opponentShoot.showString(window, 30, 20);
            player.showCoords(window);
            enemy.showCoords(window);
            player.showField(window);
            enemy.showField(window);
            if (countOfDestroyed_by_opponent == 10 && countOfDestroyed_by_player < 10) {
                window.draw(gameOver);
                gameText opponentWin("You lost. Press 'Escape'", "Fonts/LeagueSpartan-Bold.ttf");
                opponentWin.setColor(Color::White);
                opponentWin.setFontsize(70);
                opponentWin.showString(window, 460, 590);
            }
            else if (countOfDestroyed_by_player == 10 && countOfDestroyed_by_opponent < 10) {
                window.draw(gameOver);
                gameText playerWin("You won ! Press 'Escape'", "Fonts/LeagueSpartan-Bold.ttf");
                playerWin.setColor(Color::White);
                playerWin.setFontsize(70);
                playerWin.showString(window, 460, 590);
            }
            window.display();
        }
        shotsForBot.clear();
    }

};

class UnoGame : public Strategy
{
    void DoAlgorithm(RenderWindow& window, int width, int height) const override
    {

        RectangleShape bg(Vector2f(width, height));
        Texture bgTexture;
        bgTexture.loadFromFile("Uno/unoBG.jpg");
        bg.setTexture(&bgTexture);
        Uno unoTable("Uno/SymbolAndColor.txt");
        Texture t;
        t.loadFromFile("Uno/hiddenCard.png");
        Sprite s;
        s.setTexture(t);
        vector<unoCards> startCardsDeck;
        startCardsDeck.resize(7);
        unoCards temp;

        for (int i = 0; i < 7; i++) {
            temp = unoTable.takeCard();
            setCardParameters(temp, startCardsDeck.at(i));
        }

        vector<unoCards> startCardsDeck2;
        startCardsDeck2.resize(7);


        for (int i = 0; i < 7; i++) {
            temp = unoTable.takeCard();
            setCardParameters(temp, startCardsDeck2.at(i));
        }

        Opponent opponent(startCardsDeck2);
        Player player(startCardsDeck);
        vector<CircleShape> colorsToPick;
        colorsToPick.resize(4);
        for (int i = 0, yPos = 450; i < colorsToPick.size(); i++, yPos += 70) {
            colorsToPick.at(i).setOutlineColor(Color::White);
            colorsToPick.at(i).setOutlineThickness(4);
            colorsToPick.at(i).setRadius(17);
            colorsToPick.at(i).setPosition(Vector2f(1250, yPos));
        }

        colorsToPick.at(0).setFillColor(Color(255, 85, 85));
        colorsToPick.at(1).setFillColor(Color(35, 177, 77));
        colorsToPick.at(2).setFillColor(Color(255, 201, 13));
        colorsToPick.at(3).setFillColor(Color(85, 85, 255));

        bool opponentPicksColor = false;//чи потрібно тягнути карту супернику

        bool playerPicksColor = false;// чи потрібно тягнути карту гравцю

        int whoStarts = rand() % 2;
        if (whoStarts == 0) {
            playersTurn(player, opponent);
        }
        else {
            cout << "opponent" << endl;
            opponentsTurn(player, opponent);
            setCardParameters(unoTable.getCurrentCard(), temp);
            opponent.setCardToBeat(temp);
        }
        bool endOfGame = false;
        int itemOfcard;//буде приходити цифра 0 -- якщо просто цифра на карті, 1 -- пропустити хід, 2 -- зміна напрямку ходу, 3 -- +2, 4 -- змінити колір, 5 -- +4 і змінити колір
        Font font;
        font.loadFromFile("Fonts/LeagueSpartan-Bold.ttf");
        Text text;
        text.setFont(font);
        text.setCharacterSize(30);
        text.setFillColor(Color(251, 251, 0));
        text.setPosition(Vector2f(700.f, 690.f));
        class::Music sound;

        Texture txt;
        txt.loadFromFile("Uno/endGameBG.png");
        Sprite sp;
        sp.setTexture(txt);
        sp.setPosition(Vector2f(780.f, 380.f));

        while (window.isOpen()) {
            Event event;
            while (window.pollEvent(event)) {
                if (event.type == Event::Closed) window.close();
                if (event.key.code == Keyboard::Escape) {
                    ClientCode(window, width, height);
                }
                if (event.type == Event::MouseButtonPressed && endOfGame == false) {
                    if (event.mouseButton.button == Mouse::Left && player.getTurn() == true) {
                        sf::Vector2f playPos = { (float)Mouse::getPosition().x,(float)Mouse::getPosition().y };
                        if (playerPicksColor == true) {
                            if (colorsToPick.at(0).getGlobalBounds().contains(playPos.x, playPos.y)) {
                                unoTable.setCurrentColor("red");
                            }
                            else if (colorsToPick.at(1).getGlobalBounds().contains(playPos.x, playPos.y)) {
                                unoTable.setCurrentColor("green");
                            }
                            else if (colorsToPick.at(2).getGlobalBounds().contains(playPos.x, playPos.y)) {
                                unoTable.setCurrentColor("yellow");
                            }
                            else if (colorsToPick.at(3).getGlobalBounds().contains(playPos.x, playPos.y)) {
                                unoTable.setCurrentColor("blue");
                            }
                            playerPicksColor = false;
                            playersTurn(player, opponent);
                        }
                        else if (unoTable.hiddenCardDeck.getGlobalBounds().contains(playPos.x, playPos.y)) {
                            sound.setMusic("flipcard.wav");
                            setCardParameters(unoTable.takeCard(), temp);
                            player.addCard(temp);
                            opponentsTurn(player, opponent);
                        }
                        else {
                            sound.setMusic("flipcard.wav");
                            setCardParameters(player.pushCard(playPos), temp);
                            unoTable.setCurrentCard(temp);
                            opponent.setCardToBeat(temp);
                            itemOfcard = player.checkForOtherThanNumberCard(temp);
                            switch (itemOfcard) {
                            case 0:opponentsTurn(player, opponent); break;
                            case 1:playersTurn(player, opponent); break;
                            case 2:playersTurn(player, opponent); break;
                            case 3: {playersTurn(player, opponent); for (int i = 0; i < 2; i++) { setCardParameters(unoTable.takeCard(), temp); opponent.addCard(temp); } }; break;
                            case 4: {playersTurn(player, opponent); playerPicksColor = true; }; break;
                            case 5: {playerPicksColor = true; playersTurn(player, opponent); for (int i = 0; i < 4; i++) {
                                setCardParameters(unoTable.takeCard(), temp); opponent.addCard(temp);
                            } }; break;
                            }
                        }
                        if (player.isEmpty() == true) {
                            text.setString("You won!!! Press Escape to exit");
                            endOfGame = true;
                        }
                    }
                }
            }
            if (unoTable.isEmpty() == true) {
                text.setString("Draw. Press Escape to exit");
                endOfGame = true;
            }
            window.clear();
            window.draw(bg);

            player.showElements(window);
            opponent.showElements(window);
            for (int i = 0; i < 4; i++) {
                window.draw(colorsToPick.at(i));
            }
            if (endOfGame == true) {
                window.draw(sp);
                window.draw(text);
            }
            else {
                unoTable.showAllElements(window);
            }
            window.display();
            if (opponent.getTurn() == true && endOfGame == false) {
                Sleep(1000);
                if (opponentPicksColor == true) {
                    unoTable.setCurrentColor(opponent.pickColor());
                    opponentPicksColor = false;
                    opponentsTurn(player, opponent);
                }
                sound.setMusic("flipcard.wav");
                setCardParameters(unoTable.getCurrentCard(), temp);
                opponent.setCardToBeat(temp);
                setCardParameters(opponent.pushCard(), temp);
                if (temp.color != "notFound") {
                    unoTable.setCurrentCard(temp);
                    player.setCardToBeat(temp);
                    itemOfcard = opponent.checkForOtherThanNumberCard(temp);
                    switch (itemOfcard) {
                    case 0:playersTurn(player, opponent); break;
                    case 1:opponentsTurn(player, opponent); break;
                    case 2:opponentsTurn(player, opponent); break;
                    case 3: {opponentsTurn(player, opponent); for (int i = 0; i < 2; i++) { setCardParameters(unoTable.takeCard(), temp); player.addCard(temp); } }; break;
                    case 4: {opponentsTurn(player, opponent); opponentPicksColor = true; }; break;
                    case 5: {opponentPicksColor = true; opponentsTurn(player, opponent); for (int i = 0; i < 4; i++) { setCardParameters(unoTable.takeCard(), temp); player.addCard(temp); } }; break;
                    }
                }
                else {
                    sound.setMusic("flipcard.wav");
                    setCardParameters(unoTable.takeCard(), temp);
                    opponent.addCard(temp);
                    playersTurn(player, opponent);
                }
                if (opponent.isEmpty() == true) {
                    text.setString("You lost!!! Press Escape to exit");
                    endOfGame = true;
                }
            }
            if (unoTable.isEmpty() == true) {
                text.setString("Draw. Press Escape to exit");
                endOfGame = true;
            }
            window.clear();
            window.draw(bg);
            player.showElements(window);
            opponent.showElements(window);
            for (int i = 0; i < 4; i++) {
                window.draw(colorsToPick.at(i));
            }
            if (endOfGame == true) {
                window.draw(sp);
                window.draw(text);
            }
            else {
                unoTable.showAllElements(window);
            }
            window.display();
        }
        colorsToPick.clear();
    }
};

class HangmanMenu :public Strategy {
public:
    void DoAlgorithm(RenderWindow& window, int width, int height) const override {
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
        String menuNames[] = { "Body parts","Sports","Proffesions", "Exit" };
        GameMenu menu(window, 950, 550, 4, menuNames, 100, 100);
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
                        case 0: {startHangman("bodyParts.txt", window, width, height); }break;
                        case 1: {startHangman("sports.txt", window, width, height); }break;
                        case 2: {startHangman("proffesions.txt", window, width, height); }break;
                        case 3: {ClientCode(window, width, height); } break;
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
    }
};

class Snake :public Strategy {
public:
    void DoAlgorithm(RenderWindow& Window, int width, int height) const override
    {
        Window.close();
        snake::init_game();

        sf::RenderWindow window(sf::VideoMode(snake::window_width, snake::window_height), "Snake", sf::Style::Close);

        std::vector<int> snake_direction_queue;

        while (window.isOpen())
        {
            sf::Event event;
            while (window.pollEvent(event))
            {
                if (event.type == sf::Event::Closed) {
                    snake_direction_queue.clear();
                    window.close();
                    Window.create(VideoMode::getDesktopMode(), "Menu", Style::Fullscreen);
                    float width = VideoMode::getDesktopMode().width;
                    float height = VideoMode::getDesktopMode().height;
                    ClientCode(Window, width, height);
                }
                if (event.type == sf::Event::KeyPressed) {
                    if (snake::game_paused) {
                        if (snake::game_over_timeout == 0) {
                            if (snake::current_menu == MENU_MAIN) {
                                switch (event.key.code) {
                                case sf::Keyboard::Up:
                                    snake::sound_menu_navigate.play();
                                    snake::current_main_menu_item_index--;
                                    if (snake::current_main_menu_item_index < 0) {
                                        snake::current_main_menu_item_index = snake::text_main_menu_items.size() - 1;
                                    }
                                    break;
                                case sf::Keyboard::Down:
                                    snake::sound_menu_navigate.play();
                                    snake::current_main_menu_item_index++;
                                    if (snake::current_main_menu_item_index > snake::text_main_menu_items.size() - 1) {
                                        snake::current_main_menu_item_index = 0;
                                    }
                                    break;
                                case sf::Keyboard::Enter:
                                    snake::sound_menu_navigate.play();
                                    if (snake::main_menu_items.at(snake::current_main_menu_item_index) == MENU_ITEM_START) {
                                        if (!snake::game_over && snake::game_started) {
                                            snake::game_paused = false;
                                        }
                                        else {
                                            snake::start_game();
                                        }
                                    }
                                    if (snake::main_menu_items.at(snake::current_main_menu_item_index) == MENU_ITEM_SETTINGS) {
                                        snake::current_menu = MENU_SETTINGS;
                                        snake::current_settings_menu_item_index = 0;
                                    }
                                    if (snake::main_menu_items.at(snake::current_main_menu_item_index) == MENU_ITEM_QUIT) {
                                        window.close();
                                        Window.create(VideoMode::getDesktopMode(), "Menu", Style::Fullscreen);
                                        float width = VideoMode::getDesktopMode().width;//ширина екрана
                                        float height = VideoMode::getDesktopMode().height;//висота екрана
                                        ClientCode(Window, width, height);
                                    }
                                    break;
                                case sf::Keyboard::Escape:
                                    snake::sound_menu_navigate.play();
                                    if (!snake::game_over && snake::game_started) {
                                        snake::game_paused = false;
                                    }
                                    break;
                                }
                            }
                            else if (snake::current_menu == MENU_SETTINGS) {
                                switch (event.key.code) {
                                case sf::Keyboard::Up:
                                    snake::sound_menu_navigate.play();
                                    snake::current_settings_menu_item_index--;
                                    if (snake::current_settings_menu_item_index < 0) {
                                        snake::current_settings_menu_item_index = snake::text_settings_menu_items.size() - 1;
                                    }
                                    break;
                                case sf::Keyboard::Down:
                                    snake::sound_menu_navigate.play();
                                    snake::current_settings_menu_item_index++;
                                    if (snake::current_settings_menu_item_index > snake::text_settings_menu_items.size() - 1) {
                                        snake::current_settings_menu_item_index = 0;
                                    }
                                    break;
                                case sf::Keyboard::Left:
                                    if (snake::settings_menu_items.at(snake::current_settings_menu_item_index) == MENU_ITEM_VOLUME) {
                                        if (snake::settings_volume > 0) {
                                            snake::settings_volume -= 5;
                                            snake::set_volume();
                                            snake::sound_menu_navigate.play();
                                        }
                                    }
                                    break;
                                case sf::Keyboard::Right:
                                    if (snake::settings_menu_items.at(snake::current_settings_menu_item_index) == MENU_ITEM_VOLUME) {
                                        if (snake::settings_volume < 100) {
                                            snake::settings_volume += 5;
                                            snake::set_volume();
                                            snake::sound_menu_navigate.play();
                                        }
                                    }
                                    break;
                                case sf::Keyboard::Enter:
                                    snake::sound_menu_navigate.play();
                                    if (snake::settings_menu_items.at(snake::current_settings_menu_item_index) == MENU_ITEM_BACK) {
                                        snake::current_menu = MENU_MAIN;
                                    }
                                    break;
                                case sf::Keyboard::Escape:
                                    snake::sound_menu_navigate.play();
                                    snake::current_menu = MENU_MAIN;
                                    break;
                                }
                            }
                        }
                        else {
                            snake::game_over_timeout = 0;
                        }
                    }
                    else {
                        int snake_direction_last = snake_direction_queue.empty() ? snake::game_state.snake_direction : snake_direction_queue.at(0);
                        switch (event.key.code) {
                        case sf::Keyboard::Up:
                            if (snake_direction_last != SNAKE_DIRECTION_UP && snake_direction_last != SNAKE_DIRECTION_DOWN) {
                                if (snake_direction_queue.size() < 2) {
                                    snake_direction_queue.insert(snake_direction_queue.begin(), SNAKE_DIRECTION_UP);
                                }
                            }
                            break;
                        case sf::Keyboard::Right:
                            if (snake_direction_last != SNAKE_DIRECTION_RIGHT && snake_direction_last != SNAKE_DIRECTION_LEFT) {
                                if (snake_direction_queue.size() < 2) {
                                    snake_direction_queue.insert(snake_direction_queue.begin(), SNAKE_DIRECTION_RIGHT);
                                }
                            }
                            break;
                        case sf::Keyboard::Down:
                            if (snake_direction_last != SNAKE_DIRECTION_DOWN && snake_direction_last != SNAKE_DIRECTION_UP) {
                                if (snake_direction_queue.size() < 2) {
                                    snake_direction_queue.insert(snake_direction_queue.begin(), SNAKE_DIRECTION_DOWN);
                                }
                            }
                            break;
                        case sf::Keyboard::Left:
                            if (snake_direction_last != SNAKE_DIRECTION_LEFT && snake_direction_last != SNAKE_DIRECTION_RIGHT) {
                                if (snake_direction_queue.size() < 2) {
                                    snake_direction_queue.insert(snake_direction_queue.begin(), SNAKE_DIRECTION_LEFT);
                                }
                            }
                            break;
                        case sf::Keyboard::Escape:
                            snake::game_paused = true;
                            break;
                        }
                    }
                }
            }

            if (!snake_direction_queue.empty()) {
                snake::game_state.snake_direction = snake_direction_queue.back();
                snake_direction_queue.pop_back();
            }

            if (!snake::game_paused) {
                if (!snake::rollback) {
                    snake::make_move();
                }
                else {
                    if (!snake::game_last_states.empty()) {
                        snake::game_state = snake::game_last_states.back();
                        snake::game_last_states.pop_back();
                    }
                    else {
                        snake::rollback = false;
                    }
                }
            }

            window.clear(sf::Color(183, 212, 168));

            snake::draw_field(window);
            snake::draw_score_bar(window);

            if (snake::game_over) {
                window.draw(snake::text_game_over);
                if (snake::game_over_timeout > 0) {
                    snake::game_over_timeout--;
                }
            }

            if (snake::game_paused && snake::game_over_timeout == 0) {
                switch (snake::current_menu) {
                case MENU_MAIN:
                    snake::draw_main_menu(window);
                    break;
                case MENU_SETTINGS:
                    snake::draw_settings_menu(window);
                    break;
                }
            }

            window.display();

            sf::sleep(sf::milliseconds(100));
        }
    }
};

class Draughts :public Strategy {
public:
    void DoAlgorithm(RenderWindow& Window, int width, int height) const override {
        Window.close();
        ContextSettings settings;
        settings.antialiasingLevel = 16.0;
        RenderWindow window(sf::VideoMode(600, 600), "Draughts", Style::Default, settings);
        Texture gameOverTexture;
        gameOverTexture.loadFromFile("gameover.png");
        sf::Sprite gameOverSprite;
        gameOverSprite.setTexture(gameOverTexture);
        gameOverSprite.setPosition(0, 0); // Встановіть позицію спрайту
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
                    ClientCode(window, 1920, 1080);
                }

                if (event.type == Event::MouseButtonReleased) {
                    if (event.mouseButton.button == Mouse::Left) {
                        selected = !selected;
                    }
                }
            }

            window.clear();

            board.Draw(window);

            if (SelectedPiece != NULL) {
                board.Highlight(window, SelectedPiece->x, SelectedPiece->y);
            }

            if (checkWin(RedPieces, WhitePieces)) {
                // Вивід зображення gameover.png
                window.draw(gameOverSprite);
                window.display();
                // Ви можете додати затримку або інші дії після виводу зображення, наприклад, закрити вікно
                sf::sleep(sf::seconds(3)); // Затримка на 3 секунди
                ClientCode(window, 1920, 1080); // Закрити вікно
                break;
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
    }
};

class TicTacToe :public Strategy {
public:
    void DoAlgorithm(RenderWindow& Window, int width, int height) const override {
        Window.close();
        RenderWindow window(VideoMode(850, 650), L"Tic-Tac-Teo AI");
        Image icon;
        icon.loadFromFile("tic.png");
        window.setIcon(32, 32, icon.getPixelsPtr());
        Texture textureFon;
        Sprite spritefon;
        textureFon.loadFromFile("fon.png");
        spritefon.setTexture(textureFon);
        spritefon.setPosition(0, 0);

        int type_game = 0;
        int x_wins = 0, o_wins = 0, d_wins = 0;
        int move = NULL;
        bool turn = NULL;
        char player_1 = NULL, player_2 = NULL;
        int draw = 0;
        bool game_over = false;
        ticTacToe::Wins* Win = new ticTacToe::Wins;


        while (window.isOpen()) {
            Event event;
            while (window.pollEvent(event)) {
                if (event.type == Event::Closed)
                    window.close();
            }

            if (type_game == 0) {
                window.clear();
                window.draw(spritefon);
                if (game_over == true) {
                    ticTacToe::display_field(window, player_1, player_2, game_over);
                    wins_victory(window, Win);
                }
                ticTacToe::menu_graph(window, &type_game, event, game_over);
                ticTacToe::display_statistic(window, x_wins, o_wins, d_wins);
                window.display();
                if (type_game == 1 || type_game == 2) {
                    ticTacToe::setup(&draw, &game_over);
                    ticTacToe::type_symbol(&turn, &player_1, &player_2, type_game);
                    window.clear();
                    window.draw(spritefon);
                }
            }
            if (type_game == 3)
            {
                int line_bar = 0;
                for (int i = 0; i < ticTacToe::NUMBER_OF_GAMES; i++)
                {
                    if (i % (ticTacToe::NUMBER_OF_GAMES / 700) == 0)
                    {
                        ticTacToe::progress_bar(window, line_bar += 1);
                    }
                    ticTacToe::setup(&draw, &game_over);
                    ticTacToe::type_symbol(&turn, &player_1, &player_2, type_game);
                    ticTacToe::Stack* Hystory = nullptr;
                    int stack_size = 0;
                    while (game_over != true)
                    {
                        move = input_events(&turn, type_game, &Hystory, &stack_size, player_1, player_2);
                        draw++;
                        game_logic(move, &draw, &turn, type_game, player_1, player_2, &x_wins, &o_wins, &d_wins, &game_over, &Hystory, &stack_size, Win);
                    }
                    delete[] Hystory;
                }
                type_game = 0;
            }
            else if (type_game == 1 || type_game == 2)
            {
                window.clear();
                window.draw(spritefon);
                ticTacToe::display_field(window, player_1, player_2);
                ticTacToe::display_statistic(window, x_wins, o_wins, d_wins);
                window.display();
                ticTacToe::Stack* Hystory = nullptr;
                int stack_size = 0;
                if (turn)
                {
                    move = -1;
                    do
                    {
                        move = ticTacToe::move_human(window);

                    } while (move == -1 || ticTacToe::Field[move] == 'X' || ticTacToe::Field[move] == 'O');
                }
                else
                {
                    move = input_events(&turn, type_game, &Hystory, &stack_size, player_1, player_2);
                }
                draw++;
                game_logic(move, &draw, &turn, type_game, player_1, player_2, &x_wins, &o_wins, &d_wins, &game_over, &Hystory, &stack_size, Win);
                if (game_over == true)
                {
                    stack_size = 0; delete[] Hystory;
                    type_game = 0;
                }
            }
        }
        delete Win;
        delete[] ticTacToe::Collections_X;
        delete[] ticTacToe::Collections_O;
    }
};

void ClientCode(RenderWindow& mainWindow, int width, int height)
{
    ::Context* context = new ::Context();
    Vector2u size = mainWindow.getSize();
    if (size.x < 1920 && size.y < 1080) {
        //mainWindow.create(VideoMode(1920,1080), "Minimetropolis menu");
        mainWindow.create(VideoMode::getDesktopMode(), "Minimetropolis menu",Style::Fullscreen);
    }
    RectangleShape mainBackground(Vector2f(width, height));//фон меню
    Texture windowTexture;
    windowTexture.loadFromFile("Images/mainMenuBg.jpg");
    mainBackground.setTexture(&windowTexture);
    Image icon;
    icon.loadFromFile("minimetropolisLogo.png");
    mainWindow.setIcon(468, 464, icon.getPixelsPtr());
    //встановлюємо шрифт для навз категорій
    Font font;
    font.loadFromFile("Fonts/LeagueSpartan-Bold.ttf");
    Text titulText;
    titulText.setFont(font);
    setText(titulText, 480, 50, "Minimetropolis", 150, Color(1, 13, 65), 10);
    //використовуємо клас MainGameMenu
    String menuNames[] = { "Tic tac toe", "Hangman","Snake", "Battleship","Draughts","Uno","Exit game" };
    GameMenu menu(mainWindow, 900, 350, 7, menuNames, 70, 90);
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
                    case 0: {context->set_strategy(new TicTacToe); context->DoSomeBusinessLogic(mainWindow, width, height); }; break;
                    case 1: {context->set_strategy(new HangmanMenu); context->DoSomeBusinessLogic(mainWindow, width, height); }; break;
                    case 2: {context->set_strategy(new Snake); context->DoSomeBusinessLogic(mainWindow, width, height); }; break;
                    case 3: {context->set_strategy(new BattleShipGame); context->DoSomeBusinessLogic(mainWindow, width, height); }; break;
                    case 4: {context->set_strategy(new Draughts); context->DoSomeBusinessLogic(mainWindow, width, height); }; break;
                    case 5: {context->set_strategy(new UnoGame); context->DoSomeBusinessLogic(mainWindow, width, height); }; break;
                    case 6: mainWindow.close(); break;
                    }
                }
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
                mainWindow.close(); break;
            }
            mainWindow.clear();
            mainWindow.draw(mainBackground);
            mainWindow.draw(titulText);
            menu.drawItems();
            mainWindow.display();
        }
    }
    delete context;
}

int main()
{
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    RenderWindow mainWindow;
    //mainWindow.create(VideoMode(1920,1080), "Minimetropolis menu");
    mainWindow.create(VideoMode::getDesktopMode(), "Minimetropolis menu", Style::Fullscreen);
    float width = VideoMode::getDesktopMode().width;//ширина екрана
    float height = VideoMode::getDesktopMode().height;//висота екрана
    ClientCode(mainWindow, width, height);
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

void startHangman(string hmType, RenderWindow& window, int width, int height) {
    Hangman hangmanGame(hmType);
    while (window.isOpen())
    {
        Event event_opt;
        while (window.pollEvent(event_opt))
        {
            if (event_opt.type == Event::Closed) ClientCode(window, width, height);
            if (event_opt.type == Event::KeyPressed)
            {
                if (event_opt.key.code == Keyboard::Escape) {
                    ClientCode(window, width, height);
                }
                if (event_opt.key.code == Keyboard::Right) {
                    hangmanGame.moveRight();
                }
                if (event_opt.key.code == Keyboard::Left) {
                    hangmanGame.moveLeft();
                }
                if (event_opt.key.code == Keyboard::Enter) {
                    if (hangmanGame.checkForCorrectness() == true) {
                        hangmanGame.selectedCorrectly();
                        hangmanGame.music.setMusic("music_right.wav");
                        hangmanGame.showLetter(hangmanGame.alphabetLetters[hangmanGame.selectedLetter]);
                    }
                    else {
                        hangmanGame.selectedWrongly();
                        hangmanGame.music.setMusic("music_wrong.wav");
                        hangmanGame.loseCounter++;
                    }
                }
                if (event_opt.key.code == Keyboard::Add)
                    hangmanGame.music.VolumeUp();
                if (event_opt.key.code == Keyboard::Subtract)
                    hangmanGame.music.VolumeDown();
                if (event_opt.key.code == Keyboard::Space)
                    startHangman(hmType, window, width, height);
            }
        }
        window.clear();
        hangmanGame.showElements(window);
        window.display();
    }
}

//функція, яка сетить значення карти
void setCardParameters(unoCards from, unoCards& to) {
    to.color = from.color;
    to.sprite = from.sprite;
    to.symbol = from.symbol;
}
void playersTurn(Player& player, Opponent& opponent) {
    player.setTurn(true);
    opponent.setTurn(false);
}
void opponentsTurn(Player& player, Opponent& opponent) {
    player.setTurn(false);
    opponent.setTurn(true);
}

