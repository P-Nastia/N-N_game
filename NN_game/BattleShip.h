#pragma once
#include <iostream>
#include <fstream>
#include <ctime>
#include <SFML/Graphics.hpp>
#include <vector>
#include <map>
#include <string>
#include "gameText.h"
using namespace sf;
using namespace std;
class BattleShip
{
protected:
	string whose;//�� ���� ������ �� ���������
	vector<vector<char>> fullField;//����, � ����� ��������� ���������
	vector<vector<char>> shotsVector;//����, � ���� ��������� ������ ����
	vector<vector<char>> playingField;//����, � ����� �������� ���� ���� ������ 
	ifstream mapFile;//���� ��� ����������� ��������� �������
	string fileName;//����, � ����� ������������� ����
	vector<vector<Texture>> cellTexture;//�������� �������
	vector<vector<Sprite>> cellSprite;//������ �������
	vector<Texture> shipsTextures;//�������� ��� �������� �������
	vector<Sprite> shipsSprite;//�������� ������� ��������, �� ���� ���������� ������ ���������� �����
	bool myTurn;//��� ���
	Texture coordsTexture[20];//�������� ��� ���� � ����
	Sprite coordsSprite[20];//������ ��� ���
	multimap<int, int> firstShipFor3;//���� �� ������ �� ����� �������� � checkForCompleteness, ���������� ������� ������ ���������� � ���
	multimap<int, int> secondShipFor3;
	multimap<int, int> shipFor4;//���� �� ������ �� ����� �������� � checkForCompleteness, ���������� ������� ������ ���������� � ���
	int countForShips[4];//����� � ������� ������ �������
	gameText countForShipsText[4] = { {"0 x ","Fonts/LeagueSpartan-Bold.ttf"},{"0 x ","Fonts/LeagueSpartan-Bold.ttf"},
		{"0 x ","Fonts/LeagueSpartan-Bold.ttf" },{"0 x ","Fonts/LeagueSpartan-Bold.ttf"} };//����� ��� ������ ������ � ������� ������ �������
	bool isFirstHorizontal = false;//�� ������ �������� �� 3 ������� ������������ �������������
	bool isSecondHorizontal = false;
	bool whichShip = false;//�� ���������� ����������� � �������(false) �� �������(true) ������ �� 3 �������
	void loadForEnemy(int num);//����������� ����� ���� ��� ���������
	void loadForPlayer(int num);//����������� ����� ���� ��� ������
	void loadField();//��������� ���� � fullField
	void loadCommonFeature();//��������� �������� ���� �� ���� � ������, ���������� �����
	void loadPlayerCoords();//��������� ���������� ���� �� ���� ��� ������
	void loadEnemyCoords();//��������� ���������� ���� �� ���� ��� ���������
	void loadShipFor3();//��������� ���������� � firstShipFor3 �� secondShipFor3
	void changeCounter(int shotInto);//������ ������� ������ �������
public:
	BattleShip(string whoseField);//�����������: ������� loadField,loadCommonFeature, ��������� ��� � �������, ������ shotsVector, playingField
	bool checkForCorrectness(int x, int y, RenderWindow& window);//�������� �� ���������
	void showField(RenderWindow& window);//�������� ���� ��� ��� �� �����
	void showCoords(RenderWindow& window);//�������� ����� �� ����� �� �����
	bool checkForCompleteness(int x, int y);//��������, �� ���� �������� �����
	bool isMyTurn()
	{
		return myTurn;
	}
	void setTurn(bool turn) { myTurn = turn; }
	bool checkForRepiteness(int vertical, int horizontal)//�������� �� ������ ���� (���������� ��� ����, ������� ���������� ��� �����������),���� ���������� ��� -- ������� true 
	{
		return (shotsVector.at(horizontal).at(vertical) == 'f') ? false : true;
	}
	~BattleShip();
};
