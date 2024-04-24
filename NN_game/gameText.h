#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include <iomanip>
#include <string>
class gameText {
public:
	sf::Font textFont;//дозволяє зберагіти інформацію про те, як буде виглядати текст
	sf::Text textString;//сам текст
	sf::Vector2f textPosition;//де розташовувати текст
	std::string str;
	gameText(std::string inputString, std::string fontStr);
	gameText() {};
	void showString(sf::RenderWindow& window, int posX, int posY);
	void setString(std::string newString);
	void setFontsize(int size);
	void setColor(sf::Color color);
	void setFontString(std::string fontString) { textFont.loadFromFile(fontString); }
};