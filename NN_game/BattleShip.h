#pragma once
#include <iostream>
#include <fstream>
#include <ctime>
#include <SFML/Graphics.hpp>
#include <vector>
#include <map>
using namespace sf;
using namespace std;
class BattleShip
{
protected:
	vector<vector<char>> fullField;//поле, з якого звіряється попадання
	vector<vector<char>> shotsVector;//поля, з яких звіряється повтор ходу
	vector<vector<char>> playingField;//поле, в якому містяться лише ходи гравця 
	ifstream mapFile;//файл для закачування розміщення кораблів
	string fileName;//файл, з якого завантажується поле
	vector<vector<Texture>> cellTexture;//текстура клітинок
	vector<vector<Sprite>> cellSprite;//спрайт клітинок
	bool myTurn;//чий хід
	Texture coordsTexture[20];//текстури для цифр і букв
	Sprite coordsSprite[20];//спрайт для них
	multimap<int, int> firstShipFor3;//щоби не робити ще більше перевірок в checkForCompleteness, координати корабля будуть зберігатися в мапі
	multimap<int, int> secondShipFor3;
	multimap<int, int> shipFor4;//щоби не робити ще більше перевірок в checkForCompleteness, координати корабля будуть зберігатися в мапі
	bool isFirstHorizontal = false;//чи перший корабель на 3 клітинки розташований горизонтально
	bool isSecondHorizontal = false;
	bool whichShip = false;//чи координати знаходяться в першому(false) чи другому(true) кораблі на 3 клітинки
	void loadForEnemy(int num);//завантажити назву мапи для суперника
	void loadForPlayer(int num);//завантажити назву мапи для іграка
	void loadField(string whoseField);//завантажує поле в fullField
	void loadCommonFeature();//завантажує текстуру цифр та букв у спрайт, встановлює розмір
	void loadPlayerCoords();//завантажує координати цифр та букв для іграка
	void loadEnemyCoords();//завантажує координати цифр та букв для суперника
	void loadShipFor3();//завантажує координати в firstShipFor3 та secondShipFor3

public:
	BattleShip(string whoseField);//конструктор: викликає loadField,loadCommonFeature, завантажує все в спрайти, вектор shotsVector, playingField
	bool checkForCorrectness(int x, int y, RenderWindow& window);//перевірка на попадання
	void showField(RenderWindow& window);//виводить поле для гри на екран
	void showCoords(RenderWindow& window);//виводить цифри та букви на екран
	bool checkForCompleteness(int x, int y);//перевірка, чи весь корабель збито
	bool isMyTurn()
	{
		return myTurn;
	}
	void setTurn(bool turn) { myTurn = turn; }
	bool checkForRepiteness(int vertical, int horizontal)//перевірка на повтор ходу (здебільшого для бота, оскільки координати самі генеруються),якщо повторився хід -- повертає true 
	{
		return (shotsVector.at(horizontal).at(vertical) == 'f') ? false : true;
	}
	~BattleShip();
};