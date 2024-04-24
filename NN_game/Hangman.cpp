#include "Hangman.h"

Hangman::Hangman(string fileNameToOpen) :loseCounter{ 0 }, selectedLetter{ 0 }, fileNameToOpen{ fileNameToOpen } {
    bgTexture.loadFromFile("HangMan/bgForHM.jpg");
    bgSprite.setTexture(bgTexture);
    hangerBase.loadFromFile("HangMan/hangmanBase.png");
    hangerBaseSprite.setTexture(hangerBase);
    hangerBaseSprite.setScale(Vector2f(1.3f, 1.3f));
    hangerBaseSprite.setPosition(Vector2f(870.f, 35));
    font.loadFromFile("Fonts/CrackerWinter.ttf");
    alphabet = new Text[26];
    for (int i = 0, xPos = 50; i < 26; i++, xPos += 40) {
        alphabet[i].setFont(font);
        alphabet[i].setFillColor(itemColor);
        alphabet[i].setString(alphabetLetters[i]);
        alphabet[i].setCharacterSize(45);
        alphabet[i].setPosition(xPos, 120);
        alphabet[i].setOutlineThickness(3);
        alphabet[i].setOutlineColor(Color(56, 108, 160));
    }
    string str = setWordToGuess();
    wordLength = str.length();
    wordToGuess = str;
    cout << "wordToGuess: " << wordToGuess << endl;
    hiddenWordToGuess = new char[wordLength];
    for (int i = 0; i < wordLength; i++) {
        hiddenWordToGuess[i] = '_';
    }
    hiddenWordToGuess[wordLength] = 0;
    cout << "hiddenWordToGuess: " << hiddenWordToGuess << endl;

    separatedLettersOfWord = new Text[wordLength];
    for (int i = 0, xPos = 100; i < wordLength; i++, xPos += 100) {
        setHiddenWord(hiddenWordToGuess[i], i, xPos);
    }
}

string Hangman::setWordToGuess() {
    srand(time(NULL));
    int randNumber;
    do {
        randNumber = rand() % 20;
    } while (randNumber % 2 != 0);
    string word;
    string line;
    std::ifstream fileMap(fileNameToOpen);
    if (!fileMap.is_open()) {
        std::cout << "Error" << std::endl;
    }
    else {
        for (int i = 0; i < randNumber + 1; i++) {
            getline(fileMap, word);
            getline(fileMap, line);
            if (i == randNumber) {
                cout << word << endl;
                cout << line << endl;
                break;
            }
        }
    }
    guessingLine.setFont(font);
    guessingLine.setFillColor(Color::White);
    if (line.length() < 70) {
        guessingLine.setCharacterSize(40);
    }
    else
        guessingLine.setCharacterSize(20);
    guessingLine.setString(line);
    guessingLine.setPosition(20, 40);
    return word;
}

void Hangman::setHiddenWord(char hiddenWordToGuess, int index, int xPos) {
    this->separatedLettersOfWord[index].setFont(font);
    this->separatedLettersOfWord[index].setFillColor(Color::Black);
    this->separatedLettersOfWord[index].setCharacterSize(80);
    this->separatedLettersOfWord[index].setPosition(xPos, 880);
    this->separatedLettersOfWord[index].setString(hiddenWordToGuess);
    this->separatedLettersOfWord[index].setOutlineColor(Color::White);
}

void Hangman::showLetter(char letter) {
    for (int i = 0, xPos = 100; i < wordLength; i++, xPos += 100) {
        if (wordToGuess[i] == letter) {
            this->hiddenWordToGuess[i] = letter;
            setHiddenWord(letter, i, xPos);
        }
        else if (wordToGuess[i] == '_') {
            setHiddenWord('_', i, xPos);
        }
    }
}

bool Hangman::checkForCorrectness() {
    for (int i = 0; i < wordLength; i++) {
        if (wordToGuess[i] == alphabetLetters[selectedLetter]) {
            return true;
        }
    }
    return false;
}

void Hangman::setHangmanBodyPart(string fileName) {
    hangmanTexture.loadFromFile(fileName);
    hangmanSprite.setTexture(hangmanTexture);
    hangmanSprite.setScale(Vector2f(1.1f, 1.1f));
    hangmanSprite.setPosition(Vector2f(870.f, 240.f));
}

bool Hangman::wordCompleted() {
    if (loseCounter < 6) {
        for (int i = 0; i < wordLength; i++) {
            if (hiddenWordToGuess[i] == '_') {
                return false;
            }
        }
    }
    return true;
}

void Hangman::showElements(RenderWindow& hangmanWindow) {
    hangmanWindow.draw(bgSprite);
    hangmanWindow.draw(hangerBaseSprite);

    switch (loseCounter) {
    case 1: {setHangmanBodyPart("HangMan/1.png"); }; break;
    case 2: {setHangmanBodyPart("HangMan/2.png"); }; break;
    case 3: {setHangmanBodyPart("HangMan/3.png"); }; break;
    case 4: {setHangmanBodyPart("HangMan/4.png"); }; break;
    case 5: {setHangmanBodyPart("HangMan/5.png"); }; break;
    case 6: {setHangmanBodyPart("HangMan/6.png"); gameText text("You lost. Correct word was " + wordToGuess,
        "Fonts/LeagueSpartan-Bold.ttf"); text.setColor(Color::Black);
        text.showString(hangmanWindow, 100, 450); }; break;
    }
    if (wordCompleted() == true && loseCounter < 6) {
        gameText text("You won. \nPress 'Escape' to exit, \n'Space' to continue playing",
            "Fonts/LeagueSpartan-Bold.ttf"); text.setColor(Color::Black);
        text.showString(hangmanWindow, 100, 450);
    }
    hangmanWindow.draw(hangmanSprite);
    showAlhpabet(hangmanWindow);
    showHiddenWord(hangmanWindow);
    hangmanWindow.draw(guessingLine);
    
}

void Hangman::moveRight() {
    selectedLetter++;
    if (selectedLetter < 26) {
        alphabet[selectedLetter - 1].setFillColor(itemColor);
        alphabet[selectedLetter].setFillColor(pickedColor);
    }
    else {
        alphabet[25].setFillColor(itemColor);
        selectedLetter = 0;
        alphabet[selectedLetter].setFillColor(pickedColor);
    }
}

void Hangman::moveLeft() {
    selectedLetter--;
    if (selectedLetter >= 0) {
        alphabet[selectedLetter].setFillColor(pickedColor);
        alphabet[selectedLetter + 1].setFillColor(itemColor);
    }
    else {
        alphabet[0].setFillColor(itemColor);
        selectedLetter = 25;
        alphabet[selectedLetter].setFillColor(pickedColor);
    }
}