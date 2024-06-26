#include <iostream>
#include <fstream>
#include <ctime>
#include <SFML/Graphics.hpp>
#include "Music.h"
#include "gameText.h"
#include <Windows.h>
#include <string>
using namespace std;
using namespace sf;
class Hangman//public GameMenu
{
private:

    //��� 
    Texture bgTexture;
    Sprite bgSprite;
    //������ ��� ������ �������� 
    Texture hangmanTexture;
    Sprite hangmanSprite;
    //������ ��� ������� ��������
    Texture hangerBase;;
    Sprite hangerBaseSprite;
    //RenderWindow hangmanWindow;
    Text* alphabet;//����� � ���� �������, � ���� �������� alphabetLetters
    //������� ��� ���������� (��)��������� ������������ ����
    Color itemColor = Color(1, 13, 65);
    Color pickedColor = Color(101, 9, 128);
    Color wrongLetter = Color(217, 0, 5);
    Color CorrectLetter = Color(39, 183, 53);
    Font font;//�����
    Text guessingLine;//���� �����, ��� ������� ��������
    string wordToGuess;//���� �����, ��� ������� �������
    char* hiddenWordToGuess;//���� ���������� ������� "_" �� ������, �� �������� � separatedLettersOfWord
    Text* separatedLettersOfWord;//������� "_" �� ������
    string fileNameToOpen;//����, � ����� ����������� �����, ��� ��������� �� ������������
    int wordLength;
    Texture t;//�������� ��� �������� ��� ��������� ���
    Sprite gameOver;

public:
    Hangman(string fileNameToOpen);
    string setWordToGuess();//����� ����������� ���������� ������ ����� �� ���� �����
    void setHiddenWord(char hiddenWordToGuess, int index, int xPos);//������������ "_" ��� ���������� ���� �� �� ������� ������� �����
    void showLetter(char letter);//���� �������� �����, ������� "_" �� ��
    void showHiddenWord(RenderWindow& hangmanWindow) {//���������� ��� �����
        for (int i = 0; i < wordLength; i++) {
            hangmanWindow.draw(separatedLettersOfWord[i]);
        }
    }
    void showAlhpabet(RenderWindow& hangmanWindow) {//���������� ������, �� ����� ������ ������� ����� ����� ����������� �� ����� �����
        for (int i = 0; i < 26; i++) {
            hangmanWindow.draw(alphabet[i]);
        }
    }
    bool checkForCorrectness();//��������, �� ������ ����� � � ����
    void setHangmanBodyPart(string fileName);//������ ������� ��������� ��������, ������� �� ������� ������������ ����
    bool wordCompleted();//��������, �� ����� ������� ��������
    void showElements(RenderWindow& hangmanWindow);
    void moveRight();//����������� �� ������ ������� ������
    void moveLeft();//����������� �� ������ ������� ����
    //���� ������� �����, ������� �� ����������� 
    void selectedCorrectly() {
        alphabet[selectedLetter].setFillColor(CorrectLetter);
    }
    void selectedWrongly() {
        alphabet[selectedLetter].setFillColor(wrongLetter);
    }
    Sprite getHmSprite() { return hangmanSprite; }
    Sprite getbgSprite() { return bgSprite; }
    class::Music music;//��� ��������� ���������� �������� (��)��������� �����
    int loseCounter;//������� ����������� ������������ ����
    char alphabetLetters[26] = { 'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z' };
    int selectedLetter;//����� �����, ��� ���������� �����
};