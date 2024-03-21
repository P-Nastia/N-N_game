#include <iostream>
#include <fstream>
#include <ctime>
#include <SFML/Graphics.hpp>
#include "Music.h"
#include "gameText.h"
using namespace std;
using namespace sf;
class Hangman//public GameMenu
{
private:
    //фон 
    Texture bgTexture;
    Sprite bgSprite;
    //спрайт для частин чоловічка 
    Texture hangmanTexture;
    Sprite hangmanSprite;
    //спрайт для каркасу шибениці
    Texture hangerBase;;
    Sprite hangerBaseSprite;
    RenderWindow hangmanWindow;
    Text* alphabet;//масив з літер алфавіту, в який сетиться alphabetLetters
    //кольори для замальовки (не)правильно використаних букв
    Color itemColor = Color(1, 13, 65);
    Color pickedColor = Color(101, 9, 128);
    Color wrongLetter = Color(217, 0, 5);
    Color CorrectLetter = Color(39, 183, 53);
    Font font;//шрифт
    Text guessingLine;//опис слова, яке потрібно відгадати
    string wordToGuess;//саме слово, яке потрібно вгадати
    char* hiddenWordToGuess;//лише невідгадані символи "_" на екрані, які сетяться в separatedLettersOfWord
    Text* separatedLettersOfWord;//символи "_" на екрані
    char alphabetLetters[26] = { 'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z' };
    int loseCounter;//кількість неправильно використаних букв
    int selectedLetter;//номер букви, яку користувач обрав
    string fileNameToOpen;//файл, з якого завантажуємо слово, яке обирається по рандомайзеру
    int wordLength;
    class::Music music;//для звукового оголошення обирання (не)правильної літери

public:
    Hangman(string fileNameToOpen);
    string setWordToGuess();//через рандомайзер вибирається стрічка слова та його опису
    void setHiddenWord(char hiddenWordToGuess, int index, int xPos);//встановлюємо "_" для невгаданих букв та за потреби вгадані букви
    void showLetter(char letter);//якщо відгадали букву, змінюємо "_" на неї
    void showHiddenWord() {//виводиться все слово
        for (int i = 0; i < wordLength; i++) {
            hangmanWindow.draw(separatedLettersOfWord[i]);
        }
    }
    void showAlhpabet() {//виводиться алфавіт, по якому власне гравець рухом клавіш проходиться та обирає букву
        for (int i = 0; i < 26; i++) {
            hangmanWindow.draw(alphabet[i]);
        }
    }
    bool checkForCorrectness();//перевірка, чи обрана буква є в слові
    void setHangmanBodyPart(string fileName);//сетимо частину повішеного чоловічка, залежно від кількості неправильних літер
    bool wordCompleted();//перевірка, чи слово повінстю складене
    void startGame();
    void moveRight();//проходиться по літерах алфавіту вправо
    void moveLeft();//проходиться по літерах алфавіту вліво
    //зміна кольору букви, залежно від правильності 
    void selectedCorrectly() {
        alphabet[selectedLetter].setFillColor(CorrectLetter);
    }
    void selectedWrongly() {
        alphabet[selectedLetter].setFillColor(wrongLetter);
    }
    Sprite getHmSprite() { return hangmanSprite; }
    Sprite getbgSprite() { return bgSprite; }
};