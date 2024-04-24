#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include <iomanip>
#include <string>
class gameText {
public:
	sf::Font textFont;//�������� �������� ���������� ��� ��, �� ���� ��������� �����
	sf::Text textString;//��� �����
	sf::Vector2f textPosition;//�� ������������� �����
	std::string str;
	gameText(std::string inputString, std::string fontStr);
	gameText() {};
	void showString(sf::RenderWindow& window, int posX, int posY);
	void setString(std::string newString);
	void setFontsize(int size);
	void setColor(sf::Color color);
	void setFontString(std::string fontString) { textFont.loadFromFile(fontString); }
};