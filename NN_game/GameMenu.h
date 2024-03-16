#pragma once
#include <iomanip>
#include <iostream>
#include <SFML/Graphics.hpp>


using namespace std;
using namespace sf;
class GameMenu
{
protected:
	float cordMenuX;//координати меню х
	float cordMenuY;//координати меню у
	int stepWide;//ширина між пунктами меню
	int maxNumOfItems;//максимальна кількість пунктів
	int fontSize;//розмір шрифта
	int menuItemSelected;//номер вибраного пункта
	Font font;//шрифт меню
	Text* menuItems;//масив назв пунктів меню
	Color itemColor;// колір пунктів
	Color pickedItemColor;// колір вибраного пункта
	Color borderColor;//колір обрамки тексту
	void setText(Text& text, string str, float xPos, float yPos);
	void alignText();
	RenderWindow& window;
public:
	GameMenu(RenderWindow& window, float cordMenuX, float cordMenuY, int maxItems, String name[], int fontSize, int stepWide);
	int getMenuSelected() { return menuItemSelected; }
	virtual void drawItems();
	void moveUp();
	void moveDown();
	void setColorsForItems(Color itemColor, Color pickedColor, Color border);
	~GameMenu() {
		delete[] menuItems;
	}
};