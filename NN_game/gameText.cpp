#include "gameText.h"

gameText::gameText(std::string inputString, std::string fontStr) {//стрічка і файл з шрифтом
	textFont.loadFromFile(fontStr);//розширення ttf
	textString.setFont(textFont);
	textString.setCharacterSize(50);
	textString.setFillColor(sf::Color::Blue);
	textString.setStyle(sf::Text::Bold);
	textString.setString(inputString);
	textString.setPosition(sf::Vector2f(20.f, 20.f));
}


void gameText::showString(sf::RenderWindow& window, int posX, int posY) {
	textString.setPosition(posX, posY);
	window.draw(textString);
}

void gameText::setString(std::string newString) {
	textString.setString(newString);
	str = newString;
}

void gameText::setFontsize(int size)
{
	textString.setCharacterSize(size);
}

void gameText::setColor(sf::Color color)
{
	textString.setFillColor(color);
}