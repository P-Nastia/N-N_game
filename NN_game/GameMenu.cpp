#include "GameMenu.h"

void GameMenu::setText(Text& text, string str, float xPos, float yPos)
{
	text.setFont(font);
	text.setFillColor(itemColor);
	text.setString(str);
	text.setCharacterSize(fontSize);
	text.setPosition(xPos, yPos);
	text.setOutlineThickness(3);
	text.setOutlineColor(borderColor);
}

void GameMenu::alignText()
{
	float centreCord;
	for (int i = 0; i < maxNumOfItems; i++) {
		centreCord = menuItems[i].getLocalBounds().width / 2;
		menuItems[i].setPosition(menuItems[i].getPosition().x - centreCord, menuItems[i].getPosition().y);
	}
}

GameMenu::GameMenu(RenderWindow& window, float cordMenuX, float cordMenuY, int maxItems, String name[], int fontSize, int stepWide) :window{ window },
cordMenuX{ cordMenuX }, cordMenuY{ cordMenuY }, maxNumOfItems{ maxItems }, fontSize{ fontSize }, stepWide{ stepWide }, menuItemSelected{ 0 },
itemColor{ Color(1, 13, 65) }, pickedItemColor{ Color(217, 0, 5) }, borderColor{ Color(56, 108, 160) }
{
	font.loadFromFile("fonts/CrackerWinter.ttf");
	menuItems = new Text[maxNumOfItems];
	//розміщення пунктів
	for (int i = 0, yPos = cordMenuY; i < maxNumOfItems; i++, yPos += stepWide) {
		setText(menuItems[i], name[i], cordMenuX, yPos);
	}
	alignText();
}

void GameMenu::drawItems()
{
	for (int i = 0; i < maxNumOfItems; i++) {
		window.draw(menuItems[i]);
	}
}

void GameMenu::moveUp()
{
	menuItemSelected--;
	if (menuItemSelected >= 0) {
		menuItems[menuItemSelected].setFillColor(pickedItemColor);
		menuItems[menuItemSelected + 1].setFillColor(itemColor);
	}
	else {
		menuItems[0].setFillColor(itemColor);
		menuItemSelected = maxNumOfItems - 1;
		menuItems[menuItemSelected].setFillColor(pickedItemColor);
	}
}

void GameMenu::moveDown() {
	menuItemSelected++;
	if (menuItemSelected <= maxNumOfItems) {
		menuItems[menuItemSelected - 1].setFillColor(itemColor);

		menuItems[menuItemSelected].setFillColor(pickedItemColor);
	}
	else {
		menuItems[maxNumOfItems - 1].setFillColor(itemColor);
		menuItemSelected = 0;
		menuItems[menuItemSelected].setFillColor(pickedItemColor);
	}
}

void GameMenu::setColorsForItems(Color itemColor, Color pickedColor, Color border)
{
	this->itemColor = itemColor;
	this->pickedItemColor = pickedColor;
	this->borderColor = border;
}