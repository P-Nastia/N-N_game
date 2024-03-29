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
using namespace std;
using namespace sf;
#include "Hangman.h"
#include "GameMenu.h"
#include "BattleShip.h"
#include "gameText.h"
#include "Board.h"
#include "Piece.h"

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

void showHangManRules(RenderWindow& rules, int width, int height);
void mainMenuStart(RenderWindow& mainWindow, int width, int height);
void hangmanMenu(RenderWindow& hangmanMenu, int width, int height);
void battleShipMenu(RenderWindow& bsWindow, int width, int height);
void startBattleship(RenderWindow& window, int width, int height);
int checkersMenu(RenderWindow& window, int width, int height);
int startCheckers(RenderWindow& window, int width, int height);
namespace snake {
   

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

    int main()
    {
        init_game();

        sf::RenderWindow window(sf::VideoMode(window_width, window_height), "Snake", sf::Style::Close);

        std::vector<int> snake_direction_queue;

        while (window.isOpen())
        {
            sf::Event event;
            while (window.pollEvent(event))
            {
                if (event.type == sf::Event::Closed) {
                    snake_direction_queue.clear();
                    window.close();
                }
                if (event.type == sf::Event::KeyPressed) {
                    if (game_paused) {
                        if (game_over_timeout == 0) {
                            if (current_menu == MENU_MAIN) {
                                switch (event.key.code) {
                                case sf::Keyboard::Up:
                                    sound_menu_navigate.play();
                                    current_main_menu_item_index--;
                                    if (current_main_menu_item_index < 0) {
                                        current_main_menu_item_index = text_main_menu_items.size() - 1;
                                    }
                                    break;
                                case sf::Keyboard::Down:
                                    sound_menu_navigate.play();
                                    current_main_menu_item_index++;
                                    if (current_main_menu_item_index > text_main_menu_items.size() - 1) {
                                        current_main_menu_item_index = 0;
                                    }
                                    break;
                                case sf::Keyboard::Enter:
                                    sound_menu_navigate.play();
                                    if (main_menu_items.at(current_main_menu_item_index) == MENU_ITEM_START) {
                                        if (!game_over && game_started) {
                                            game_paused = false;
                                        }
                                        else {
                                            start_game();
                                        }
                                    }
                                    if (main_menu_items.at(current_main_menu_item_index) == MENU_ITEM_SETTINGS) {
                                        current_menu = MENU_SETTINGS;
                                        current_settings_menu_item_index = 0;
                                    }
                                    if (main_menu_items.at(current_main_menu_item_index) == MENU_ITEM_QUIT) {
                                        window.close();
                                    }
                                    break;
                                case sf::Keyboard::Escape:
                                    sound_menu_navigate.play();
                                    if (!game_over && game_started) {
                                        game_paused = false;
                                    }
                                    break;
                                }
                            }
                            else if (current_menu == MENU_SETTINGS) {
                                switch (event.key.code) {
                                case sf::Keyboard::Up:
                                    sound_menu_navigate.play();
                                    current_settings_menu_item_index--;
                                    if (current_settings_menu_item_index < 0) {
                                        current_settings_menu_item_index = text_settings_menu_items.size() - 1;
                                    }
                                    break;
                                case sf::Keyboard::Down:
                                    sound_menu_navigate.play();
                                    current_settings_menu_item_index++;
                                    if (current_settings_menu_item_index > text_settings_menu_items.size() - 1) {
                                        current_settings_menu_item_index = 0;
                                    }
                                    break;
                                case sf::Keyboard::Left:
                                    if (settings_menu_items.at(current_settings_menu_item_index) == MENU_ITEM_VOLUME) {
                                        if (settings_volume > 0) {
                                            settings_volume -= 5;
                                            set_volume();
                                            sound_menu_navigate.play();
                                        }
                                    }
                                    break;
                                case sf::Keyboard::Right:
                                    if (settings_menu_items.at(current_settings_menu_item_index) == MENU_ITEM_VOLUME) {
                                        if (settings_volume < 100) {
                                            settings_volume += 5;
                                            set_volume();
                                            sound_menu_navigate.play();
                                        }
                                    }
                                    break;
                                case sf::Keyboard::Enter:
                                    sound_menu_navigate.play();
                                    if (settings_menu_items.at(current_settings_menu_item_index) == MENU_ITEM_BACK) {
                                        current_menu = MENU_MAIN;
                                    }
                                    break;
                                case sf::Keyboard::Escape:
                                    sound_menu_navigate.play();
                                    current_menu = MENU_MAIN;
                                    break;
                                }
                            }
                        }
                        else {
                            game_over_timeout = 0;
                        }
                    }
                    else {
                        int snake_direction_last = snake_direction_queue.empty() ? game_state.snake_direction : snake_direction_queue.at(0);
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
                            game_paused = true;
                            break;
                        }
                    }
                }
            }
            
            if (!snake_direction_queue.empty()) {
                game_state.snake_direction = snake_direction_queue.back();
                snake_direction_queue.pop_back();
            }

            if (!game_paused) {
                if (!rollback) {
                    make_move();
                }
                else {
                    if (!game_last_states.empty()) {
                        game_state = game_last_states.back();
                        game_last_states.pop_back();
                    }
                    else {
                        rollback = false;
                    }
                }
            }

            window.clear(sf::Color(183, 212, 168));

            draw_field(window);
            draw_score_bar(window);

            if (game_over) {
                window.draw(text_game_over);
                if (game_over_timeout > 0) {
                    game_over_timeout--;
                }
            }

            if (game_paused && game_over_timeout == 0) {
                switch (current_menu) {
                case MENU_MAIN:
                    draw_main_menu(window);
                    break;
                case MENU_SETTINGS:
                    draw_settings_menu(window);
                    break;
                }
            }

            window.display();

            sf::sleep(sf::milliseconds(100));
        }

        return 0;
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

int main()
{
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    RenderWindow mainWindow;
    mainWindow.create(VideoMode::getDesktopMode(), "Menu", Style::Fullscreen);
    float width = VideoMode::getDesktopMode().width;//ширина екрана
    float height = VideoMode::getDesktopMode().height;//висота екрана
    mainMenuStart(mainWindow, width, height);
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
//BattleShip player("player");
//BattleShip enemy("enemy");

void startBattleship(RenderWindow& window, int width, int height) {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    srand(time(NULL));
    BattleShip player("player");
    BattleShip enemy("enemy");
    RectangleShape bg(Vector2f(VideoMode::getDesktopMode().width, VideoMode::getDesktopMode().height));
    Texture bgTexture;
    bgTexture.loadFromFile("battleship/paper.jpg");
    bg.setTexture(&bgTexture);
    gameText textForShots(" ", "fonts/LeagueSpartan-Bold.ttf");
    gameText textForTurn(" ", "fonts/LeagueSpartan-Bold.ttf");
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
    gameText playerShoot(to_string(countOfDestroyed_by_player) + "/10", "fonts/LeagueSpartan-Bold.ttf");
    playerShoot.setFontsize(40);
    playerShoot.setColor(Color::Black);
    gameText opponentShoot(to_string(countOfDestroyed_by_opponent) + "/10", "fonts/LeagueSpartan-Bold.ttf");
    opponentShoot.setFontsize(40);
    opponentShoot.setColor(Color::Black);
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
        playerShoot.showString(window, 1800, 20);
        opponentShoot.showString(window, 30, 20);
        player.showCoords(window);
        enemy.showCoords(window);
        player.showField(window);
        enemy.showField(window);
        if (countOfDestroyed_by_opponent == 10 && countOfDestroyed_by_player < 10) {
            gameText opponentWin("You lost. Press 'Escape'", "fonts/LeagueSpartan-Bold.ttf");
            opponentWin.setColor(Color::Black);
            opponentWin.setFontsize(100);
            opponentWin.showString(window, 500, 540);
        }
        else if (countOfDestroyed_by_player == 10 && countOfDestroyed_by_opponent < 10) {
            gameText playerWin("You won !!! Press 'Escape'", "fonts/LeagueSpartan-Bold.ttf");
            playerWin.setColor(Color::Black);
            playerWin.setFontsize(100);
            playerWin.showString(window, 500, 540);
        }
        window.display();
    }
}

void battleShipMenu(RenderWindow& bsWindow, int width, int height) {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    //візуалізація меню
    RectangleShape mainBackground(Vector2f(width, height));//фон меню
    Texture windowTexture;
    windowTexture.loadFromFile("battleship/BSMenuBG.jpeg");
    mainBackground.setTexture(&windowTexture);
    //встановлюємо шрифт для навз категорій
    Font font;
    font.loadFromFile("fonts/LeagueSpartan-Bold.ttf");
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
    font.loadFromFile("fonts/LeagueSpartan-Bold.ttf");
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
                    case 2: {snake::main(); } break;
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
    windowTexture.loadFromFile("images/hangmanMenu.png");
    mainBackground.setTexture(&windowTexture);
    //встановлюємо шрифт для навз категорій
    Font font;
    font.loadFromFile("fonts/LeagueSpartan-Bold.ttf");
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

int startCheckers(RenderWindow& Window, int width, int height) {
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
    VideoMode desktop = VideoMode::getDesktopMode();
    RenderWindow window(desktop,"Checkers",Style::Fullscreen);
    for (int i = 0; i < 12; i++) {
        WhitePieces[i].color = Color::White;
        RedPieces[i].color = Color::Red;
    }

    Setup(window, RedPieces, WhitePieces);

    while (window.isOpen()) {
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) {
                window.close();
                checkersMenu(Window, width, height);
            }
            if (event.type == Event::MouseButtonReleased) {
                if (event.mouseButton.button == Mouse::Left) {
                    selected = !selected;
                }
            }
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
            window.close();
            checkersMenu(Window, width, height);
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