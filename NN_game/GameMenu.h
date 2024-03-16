#pragma once
#include <iomanip>
#include <iostream>
#include <SFML/Graphics.hpp>


using namespace std;
using namespace sf;
class GameMenu
{
protected:
	float cordMenuX;//���������� ���� �
	float cordMenuY;//���������� ���� �
	int stepWide;//������ �� �������� ����
	int maxNumOfItems;//����������� ������� ������
	int fontSize;//����� ������
	int menuItemSelected;//����� ��������� ������
	Font font;//����� ����
	Text* menuItems;//����� ���� ������ ����
	Color itemColor;// ���� ������
	Color pickedItemColor;// ���� ��������� ������
	Color borderColor;//���� ������� ������
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