#include "BattleShip.h"

BattleShip::BattleShip(string whoseField) {

	whose = whoseField;
	//завантаження картинок кораблів
	shipsTextures.resize(4);
	shipsTextures.at(0).loadFromFile("battleship/ship1.png");
	shipsTextures.at(1).loadFromFile("battleship/ship2.png");
	shipsTextures.at(2).loadFromFile("battleship/ship3.png");
	shipsTextures.at(3).loadFromFile("battleship/ship4.png");
	shipsSprite.resize(4);
	for (int i = 0; i < 4; i++) {
		shipsSprite.at(i).setTexture(shipsTextures.at(i));
		shipsSprite.at(i).setScale(Vector2f(0.5f, 0.5f));
		countForShips[i] = 0;
		countForShipsText[i].setFontsize(25);
		countForShipsText[i].setColor(Color::Black);
	}
	loadCommonFeature();
	if (whose == "player")
		loadPlayerCoords();
	else
		loadEnemyCoords();
	loadField();
	//встановлюємо розміщення клітинок
	cellTexture.resize(10);
	cellSprite.resize(10);
	for (int i = 0; i < 10; i++) {
		cellTexture[i].resize(10);
		cellSprite[i].resize(10);
	}
	if (whose == "enemy") {
		for (int i = 0, yPos = 232; i < 10; i++, yPos += 58) {
			for (int j = 0, xPos = 1044; j < 10; j++, xPos += 58) {//більше до правого краю екрану
				cellTexture.at(i).at(j).loadFromFile("battleship/emptySquare.png");
				cellSprite.at(i).at(j).setTexture(cellTexture.at(i).at(j));
				cellSprite.at(i).at(j).setScale(Vector2f(0.2, 0.2));
				cellSprite.at(i).at(j).setPosition(Vector2f(xPos, yPos));
			}
		}
		for (int i = 0, Y = 870; i < 4; i++, Y += 50) {
			shipsSprite.at(i).setPosition(Vector2f(1100.f, Y));
		}
	}

	else {
		for (int i = 0, yPos = 232; i < 10; i++, yPos += 58) {
			for (int j = 0, xPos = 232; j < 10; j++, xPos += 58) {//більше до лівого краю екрану
				if (fullField.at(i).at(j) == '-') {
					cellTexture.at(i).at(j).loadFromFile("battleship/emptySquare.png");
				}
				else
					cellTexture.at(i).at(j).loadFromFile("battleship/shipSquare.png");
				cellSprite.at(i).at(j).setTexture(cellTexture.at(i).at(j));
				cellSprite.at(i).at(j).setScale(Vector2f(0.2, 0.2));
				cellSprite.at(i).at(j).setPosition(Vector2f(xPos, yPos));
			}
		}
		for (int i = 0, Y = 870; i < 4; i++, Y += 50) {
			shipsSprite.at(i).setPosition(Vector2f(290.f, Y));
		}
	}
	myTurn = true;
	shotsVector.resize(10);
	for (int i = 0; i < shotsVector.size(); i++) {
		shotsVector.at(i).resize(10);
	}
	for (int i = 0; i < shotsVector.size(); i++) {
		for (int j = 0; j < 10; j++) {
			shotsVector.at(i).at(j) = 'f';//заповнюються всі значення false, бо ще не було хода
		}
	}
	playingField.resize(10);
	for (int i = 0; i < 10; i++) {
		playingField.at(i).resize(10);
	}
	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 10; j++) {
			playingField.at(i).at(j) = '-';//заповнюються всі значення '-', бо ще не було хода
		}
	}
	for (int x = 0; x < 10; x++) {
		for (int y = 0; y < 10; y++) {
			if (fullField.at(x).at(y) == '4')
				shipFor4.emplace(x, y);//вставляються координати корабля на 4 клітинки
		}
	}
	loadShipFor3();//завантаження координат кораблів на 3 клітинки
}

void BattleShip::showField(RenderWindow& window) {
	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 10; j++) {
			window.draw(cellSprite[i][j]);
		}
	}
	if (whose == "player") {
		for (int i = 0, Y = 870; i < 4; i++, Y += 50) {
			window.draw(shipsSprite[i]);
			countForShipsText[i].setString(to_string(countForShips[i]) + " x ");
			countForShipsText[i].showString(window, 232, Y);
		}
	}
	else {
		for (int i = 0, Y = 870; i < 4; i++, Y += 50) {
			window.draw(shipsSprite[i]);
			countForShipsText[i].setString(to_string(countForShips[i]) + " x ");
			countForShipsText[i].showString(window, 1044, Y);
		}
	}
}

bool BattleShip::checkForCompleteness(int x, int y)
{
	bool wasCompleted = true;
	if (fullField.at(y).at(x) == '1') {
		changeCounter(1);
		return true;
	}
	if (fullField.at(y).at(x) == '2') {//якщо це корабель на 2 клітинки
		if (x == 0 && y == 0) {//лівий верхній кут
			if (playingField.at(y).at(x + 1) == '-' && fullField.at(y).at(x + 1) != '-')
				wasCompleted = false;
			if (playingField.at(y + 1).at(x) == '-' && fullField.at(y + 1).at(x) != '-')
				wasCompleted = false;
			if (wasCompleted != false) {
				changeCounter(2);
				return true;
			}
			return false;
		}
		else if (x == 9 && y == 9) {//правий нижній кут
			if (playingField.at(y).at(x - 1) == '-' && fullField.at(y).at(x - 1) != '-')
				wasCompleted = false;
			if (playingField.at(y - 1).at(x) == '-' && fullField.at(y - 1).at(x) != '-')
				wasCompleted = false;
			if (wasCompleted != false) {
				changeCounter(2);
				return true;
			}
			return false;
		}
		else if (x == 0 && y == 9) {//лівий нижній кут
			if (playingField.at(y - 1).at(x) == '-' && fullField.at(y - 1).at(x) != '-')
				wasCompleted = false;
			if (playingField.at(y).at(x + 1) == '-' && fullField.at(y).at(x + 1) != '-')
				wasCompleted = false;
			if (wasCompleted != false) {
				changeCounter(2);
				return true;
			}
			return false;
		}
		else if (x == 9 && y == 0) {//правий верхній кут
			if (playingField.at(y).at(x - 1) == '-' && fullField.at(y).at(x - 1) != '-')
				wasCompleted = false;
			if (playingField.at(y + 1).at(x) == '-' && fullField.at(y + 1).at(x) != '-')
				wasCompleted = false;
			if (wasCompleted != false) {
				changeCounter(2);
				return true;
			}
			return false;
		}
		else if (x < 9 && y == 0) {//якщо це верхня лінія
			if (playingField.at(y).at(x - 1) == '-' && fullField.at(y).at(x - 1) != '-')
				wasCompleted = false;
			if (playingField.at(y).at(x + 1) == '-' && fullField.at(y).at(x + 1) != '-')
				wasCompleted = false;
			if (playingField[y + 1][x] == '-' && fullField[y + 1][x] != '-')
				wasCompleted = false;
			if (wasCompleted != false) {
				changeCounter(2);
				return true;
			}
			return false;
		}
		else if (x < 9 && y == 9) {//якщо це нижня лінія
			if (playingField.at(y).at(x - 1) == '-' && fullField.at(y).at(x - 1) != '-')
				wasCompleted = false;
			if (playingField.at(y).at(x + 1) == '-' && fullField.at(y).at(x + 1) != '-')
				wasCompleted = false;
			if (playingField.at(y - 1).at(x) == '-' && fullField.at(y - 1).at(x) != '-')
				wasCompleted = false;
			if (wasCompleted != false) {
				changeCounter(2);
				return true;
			}
			return false;
		}
		else if (x == 0 && y < 9) {//якщо це ліва крайня лінія
			if (playingField.at(y + 1).at(x) == '-' && fullField.at(y + 1).at(x) != '-')
				wasCompleted = false;
			if (playingField.at(y - 1).at(x) == '-' && fullField.at(y - 1).at(x) != '-')
				wasCompleted = false;
			if (playingField.at(y).at(x + 1) == '-' && fullField.at(y).at(x + 1) != '-')
				wasCompleted = false;
			if (wasCompleted != false) {
				changeCounter(2);
				return true;
			}
			return false;
		}
		else if (x == 9 && y < 9) {//якщо це права крайня лінія
			if (playingField[y + 1][x] == '-' && fullField[y + 1][x] != '-')
				wasCompleted = false;
			if (playingField.at(y - 1).at(x) == '-' && fullField.at(y - 1).at(x) != '-')
				wasCompleted = false;
			if (playingField.at(y).at(x - 1) == '-' && fullField[y][x - 1] != '-')
				wasCompleted = false;
			if (wasCompleted != false) {
				changeCounter(2);
				return true;
			}
			return false;
		}
		else if (x < 9 && y < 9) {//якщо координати в середині
			if (playingField.at(y).at(x - 1) == '-' && fullField.at(y).at(x - 1) != '-')
				wasCompleted = false;
			if (playingField.at(y).at(x + 1) == '-' && fullField.at(y).at(x + 1) != '-')
				wasCompleted = false;
			if (playingField.at(y + 1).at(x) == '-' && fullField.at(y + 1).at(x) != '-')
				wasCompleted = false;
			if (playingField.at(y - 1).at(x) == '-' && fullField.at(y - 1).at(x) != '-')
				wasCompleted = false;
			if (wasCompleted != false) {
				changeCounter(2);
				return true;
			}
			return false;
		}
	}
	if (fullField[y][x] == '3') {//якщо корабель на 3 клітинки
		cout << "3" << endl;
		if (whichShip == false && firstShipFor3.empty() == true) {
			if (firstShipFor3.empty() == true) {
				changeCounter(3);
				return true;
			}
			else
				return false;
			return true;
		}
		else {
			if (secondShipFor3.empty() == true) {
				changeCounter(3);
				return true;
			}
			else
				return false;
		}
	}

	if (fullField[y][x] == '4') {
		cout << "4" << endl;
		if (shipFor4.empty() == true) {
			changeCounter(4);
			return true;
		}
		else
			return false;
	}
}

bool BattleShip::checkForCorrectness(int x, int y, RenderWindow& window) {
	shotsVector.at(y).at(x) = 't';
	if (fullField.at(y).at(x) == '4') {
		auto it = find_if(//лямбда, щоб видалити пару з multimap
			shipFor4.begin(), shipFor4.end(), [&](const auto& pair) {
				return pair.first == y
				&& pair.second == x;
			});
		if (it != shipFor4.end()) {
			shipFor4.erase(it);
		}
		cout << "shipFor4" << endl;
		for (auto i : shipFor4) {
			cout << i.first << ":" << i.second << endl;
		}
		cout << endl;
	}

	//якщо корабель на 3 клітинки, видаляємо рядок з мапи з цими координатами
	else if (fullField.at(y).at(x) == '3') {
		if (firstShipFor3.empty() == false) {
			auto it1 = find_if(//лямбда, щоб видалити пару з multimap
				firstShipFor3.begin(), firstShipFor3.end(), [&](const auto& pair) {
					return pair.first == y
					&& pair.second == x;
				});
			if (it1 != firstShipFor3.end()) {
				whichShip = false;
				firstShipFor3.erase(it1);
			}
		}
		else {
			whichShip = true;
			auto it2 = find_if(
				secondShipFor3.begin(), secondShipFor3.end(), [&](const auto& pair) {
					return pair.first == y
					&& pair.second == x;
				});
			if (it2 != secondShipFor3.end()) {
				secondShipFor3.erase(it2);
			}
		}
		cout << "FirstShip " << endl << endl;
		for (auto i : firstShipFor3) {
			cout << i.first << ":" << i.second << endl;
		}
		cout << "SecondShip " << endl << endl;
		for (auto i : secondShipFor3) {
			cout << i.first << ":" << i.second << endl;
		}
	}
	if (fullField.at(y).at(x) == '-') {
		playingField.at(y).at(x) = '0';//мимо корабля
		cellTexture.at(y).at(x).loadFromFile("battleship/loseSquare.png");
		cellSprite.at(y).at(x).setTexture(cellTexture.at(y).at(x));
		showField(window);
		return false;
	}
	else {
		playingField.at(y).at(x) = 'x';//попало в корабель
		cellTexture.at(y).at(x).loadFromFile("battleship/inPointSquare.png");
		cellSprite.at(y).at(x).setTexture(cellTexture.at(y).at(x));
		showField(window);
		return true;
	}
}

void BattleShip::showCoords(RenderWindow& window)
{
	for (int i = 0; i < 20; i++) {
		window.draw(coordsSprite[i]);
	}
}

void BattleShip::loadShipFor3()
{
	bool wasFirstTry = false;//булеве значення, щоб записати координати першого корабля і по них орієнтуватися
	int xCord, yCord, count_tries_for_ship1 = 0;
	int xCord2, yCord2;
	bool wasFirstForSecond = false;//чи занеслися перші координати для другого корабля на 3 клітинки
	for (int x = 0; x < 10; x++) {
		for (int y = 0; y < 10; y++) {
			if (wasFirstTry == true && fullField[x][y] == '3') {
				if ((xCord == x && yCord < y && count_tries_for_ship1 < 3) || (yCord == y && count_tries_for_ship1 < 3 && isFirstHorizontal == true)) {
					firstShipFor3.emplace(x, y);
					count_tries_for_ship1++;
				}
				else if (isFirstHorizontal == false || count_tries_for_ship1 == 3) {
					secondShipFor3.emplace(x, y);
					if (wasFirstForSecond == false) {
						wasFirstForSecond = true;
						xCord2 = x;
						yCord2 = y;
					}
					if (wasFirstForSecond == true) {
						if (xCord2 < x && yCord == y)
							isSecondHorizontal = true;
					}
				}
			}
			else if (fullField[x][y] == '3' && wasFirstTry == false) {
				count_tries_for_ship1++;
				firstShipFor3.emplace(x, y);
				xCord = x;
				yCord = y;
				wasFirstTry = true;
				if (fullField[x + 1][y] == '3')
					isFirstHorizontal = true;
			}
		}
	}
}

void BattleShip::changeCounter(int shotInto)
{
	switch (shotInto) {
	case 1: {countForShips[0]++; }; break;
	case 2: {countForShips[1]++; }; break;
	case 3: {countForShips[2]++; }; break;
	case 4: {countForShips[3]++; }; break;
	}
}

void BattleShip::loadField() {
	srand(time(NULL));
	int num = 1 + rand() % 10;
	cout << "num = " << num << endl;
	if (whose == "enemy")
		loadForEnemy(num);
	else
		loadForPlayer(num);
	char var;
	fullField.resize(10);
	for (int i = 0; i < 10; i++) {
		fullField.at(i).resize(10);
	}
	mapFile.open(fileName);
	if (mapFile.is_open()) {
		for (int i = 0; i < 10; i++) {
			for (int j = 0; j < 10; j++) {
				mapFile >> fullField.at(i).at(j);
			}
		}
	}
	mapFile.close();
}

void BattleShip::loadCommonFeature()
{
	//текстура
	coordsTexture[0].loadFromFile("battleShip/a.png");
	coordsTexture[1].loadFromFile("battleShip/b.png");
	coordsTexture[2].loadFromFile("battleShip/c.png");
	coordsTexture[3].loadFromFile("battleShip/d.png");
	coordsTexture[4].loadFromFile("battleShip/e.png");
	coordsTexture[5].loadFromFile("battleShip/f.png");
	coordsTexture[6].loadFromFile("battleShip/g.png");
	coordsTexture[7].loadFromFile("battleShip/h.png");
	coordsTexture[8].loadFromFile("battleShip/i.png");
	coordsTexture[9].loadFromFile("battleShip/j.png");

	coordsTexture[10].loadFromFile("battleShip/n1.png");
	coordsTexture[11].loadFromFile("battleShip/n2.png");
	coordsTexture[12].loadFromFile("battleShip/n3.png");
	coordsTexture[13].loadFromFile("battleShip/n4.png");
	coordsTexture[14].loadFromFile("battleShip/n5.png");
	coordsTexture[15].loadFromFile("battleShip/n6.png");
	coordsTexture[16].loadFromFile("battleShip/n7.png");
	coordsTexture[17].loadFromFile("battleShip/n8.png");
	coordsTexture[18].loadFromFile("battleShip/n9.png");
	coordsTexture[19].loadFromFile("battleShip/n10.png");

	//текстура в спрайт
	coordsSprite[0].setTexture(coordsTexture[0]);
	coordsSprite[1].setTexture(coordsTexture[1]);
	coordsSprite[2].setTexture(coordsTexture[2]);
	coordsSprite[3].setTexture(coordsTexture[3]);
	coordsSprite[4].setTexture(coordsTexture[4]);
	coordsSprite[5].setTexture(coordsTexture[5]);
	coordsSprite[6].setTexture(coordsTexture[6]);
	coordsSprite[7].setTexture(coordsTexture[7]);
	coordsSprite[8].setTexture(coordsTexture[8]);
	coordsSprite[9].setTexture(coordsTexture[9]);

	coordsSprite[10].setTexture(coordsTexture[10]);
	coordsSprite[11].setTexture(coordsTexture[11]);
	coordsSprite[12].setTexture(coordsTexture[12]);
	coordsSprite[13].setTexture(coordsTexture[13]);
	coordsSprite[14].setTexture(coordsTexture[14]);
	coordsSprite[15].setTexture(coordsTexture[15]);
	coordsSprite[16].setTexture(coordsTexture[16]);
	coordsSprite[17].setTexture(coordsTexture[17]);
	coordsSprite[18].setTexture(coordsTexture[18]);
	coordsSprite[19].setTexture(coordsTexture[19]);

	//розмір
	coordsSprite[0].setScale(0.3, 0.3);
	coordsSprite[1].setScale(0.3, 0.3);
	coordsSprite[2].setScale(0.3, 0.3);
	coordsSprite[3].setScale(0.3, 0.3);
	coordsSprite[4].setScale(0.3, 0.3);
	coordsSprite[5].setScale(0.3, 0.3);
	coordsSprite[6].setScale(0.3, 0.3);
	coordsSprite[7].setScale(0.3, 0.3);
	coordsSprite[8].setScale(0.3, 0.3);
	coordsSprite[9].setScale(0.3, 0.3);

	coordsSprite[10].setScale(0.15, 0.15);
	coordsSprite[11].setScale(0.11, 0.11);
	coordsSprite[12].setScale(0.15, 0.15);
	coordsSprite[13].setScale(0.15, 0.15);
	coordsSprite[14].setScale(0.09, 0.09);
	coordsSprite[15].setScale(0.09, 0.09);
	coordsSprite[16].setScale(0.15, 0.15);
	coordsSprite[17].setScale(0.09, 0.09);
	coordsSprite[18].setScale(0.09, 0.09);
	coordsSprite[19].setScale(0.13, 0.13);


}

void BattleShip::loadPlayerCoords()
{
	//позиція
	coordsSprite[0].setPosition(Vector2f(232, 174));
	coordsSprite[1].setPosition(Vector2f(290, 174));
	coordsSprite[2].setPosition(Vector2f(348, 174));
	coordsSprite[3].setPosition(Vector2f(406, 174));
	coordsSprite[4].setPosition(Vector2f(464, 174));
	coordsSprite[5].setPosition(Vector2f(522, 174));
	coordsSprite[6].setPosition(Vector2f(580, 174));
	coordsSprite[7].setPosition(Vector2f(638, 174));
	coordsSprite[8].setPosition(Vector2f(696, 174));
	coordsSprite[9].setPosition(Vector2f(754, 174));

	coordsSprite[10].setPosition(Vector2f(174, 232));
	coordsSprite[11].setPosition(Vector2f(174, 290));
	coordsSprite[12].setPosition(Vector2f(174, 348));
	coordsSprite[13].setPosition(Vector2f(174, 406));
	coordsSprite[14].setPosition(Vector2f(174, 464));
	coordsSprite[15].setPosition(Vector2f(174, 522));
	coordsSprite[16].setPosition(Vector2f(177, 580));
	coordsSprite[17].setPosition(Vector2f(174, 638));
	coordsSprite[18].setPosition(Vector2f(174, 696));
	coordsSprite[19].setPosition(Vector2f(174, 754));
}

void BattleShip::loadEnemyCoords()
{
	//позиція
	coordsSprite[0].setPosition(Vector2f(1044, 174));
	coordsSprite[1].setPosition(Vector2f(1102, 174));
	coordsSprite[2].setPosition(Vector2f(1160, 174));
	coordsSprite[3].setPosition(Vector2f(1218, 174));
	coordsSprite[4].setPosition(Vector2f(1276, 174));
	coordsSprite[5].setPosition(Vector2f(1334, 174));
	coordsSprite[6].setPosition(Vector2f(1392, 174));
	coordsSprite[7].setPosition(Vector2f(1450, 174));
	coordsSprite[8].setPosition(Vector2f(1508, 174));
	coordsSprite[9].setPosition(Vector2f(1566, 174));

	coordsSprite[10].setPosition(Vector2f(986, 232));
	coordsSprite[11].setPosition(Vector2f(986, 290));
	coordsSprite[12].setPosition(Vector2f(986, 348));
	coordsSprite[13].setPosition(Vector2f(986, 406));
	coordsSprite[14].setPosition(Vector2f(986, 464));
	coordsSprite[15].setPosition(Vector2f(986, 522));
	coordsSprite[16].setPosition(Vector2f(989, 580));
	coordsSprite[17].setPosition(Vector2f(986, 638));
	coordsSprite[18].setPosition(Vector2f(986, 696));
	coordsSprite[19].setPosition(Vector2f(986, 754));
}

void BattleShip::loadForPlayer(int num) {//номер назви файлу для гравця
	switch (num) {
	case 1: {fileName = "player`sShips/map1.txt"; }break;
	case 2: {fileName = "player`sShips/map2.txt"; }break;
	case 3: {fileName = "player`sShips/map3.txt"; }break;
	case 4: {fileName = "player`sShips/map4.txt"; }break;
	case 5: {fileName = "player`sShips/map5.txt"; }break;
	case 6: {fileName = "player`sShips/map6.txt"; }break;
	case 7: {fileName = "player`sShips/map7.txt"; }break;
	case 8: {fileName = "player`sShips/map8.txt"; }break;
	case 9: {fileName = "player`sShips/map9.txt"; }break;
	case 10: {fileName = "player`sShips/map10.txt"; }break;
	}
}

void BattleShip::loadForEnemy(int num) {//номер назви файлу для суперника
	switch (num) {
	case 1: {fileName = "rival`sShips/map1.txt"; }break;
	case 2: {fileName = "rival`sShips/map2.txt"; }break;
	case 3: {fileName = "rival`sShips/map3.txt"; }break;
	case 4: {fileName = "rival`sShips/map4.txt"; }break;
	case 5: {fileName = "rival`sShips/map5.txt"; }break;
	case 6: {fileName = "rival`sShips/map6.txt"; }break;
	case 7: {fileName = "rival`sShips/map7.txt"; }break;
	case 8: {fileName = "rival`sShips/map8.txt"; }break;
	case 9: {fileName = "rival`sShips/map9.txt"; }break;
	case 10: {fileName = "rival`sShips/map10.txt"; }break;
	}
}

BattleShip::~BattleShip() {
	{
		for (int i = 0; i < shotsVector.size(); i++) {
			shotsVector[i].clear();
		}
		for (int i = 0; i < fullField.size(); i++) {
			fullField[i].clear();
		}
		for (int i = 0; i < playingField.size(); i++) {
			playingField[i].clear();
		}
		for (int i = 0; i < cellTexture.size(); i++) {
			cellTexture[i].clear();
		}
		for (int i = 0; i < cellSprite.size(); i++) {
			cellSprite[i].clear();
		}

		shotsVector.clear();
		fullField.clear();
		playingField.clear();
		cellTexture.clear();
		cellSprite.clear();
		shipsTextures.clear();
		shipsSprite.clear();
		firstShipFor3.erase(firstShipFor3.begin(), firstShipFor3.end());
		secondShipFor3.erase(secondShipFor3.begin(), secondShipFor3.end());
		shipFor4.erase(shipFor4.begin(), shipFor4.end());
	}
}