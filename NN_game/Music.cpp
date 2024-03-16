#include "Music.h"

void Music::setMusic(std::string strMusic) {
	this->strMusic = strMusic;
	//��� ��������� ������������� �����
	soundBuffer.loadFromFile("Music/" + this->strMusic);
	mouseClick.setBuffer(soundBuffer);
	mouseClick.play();
}

void Music::setFoneMusic(std::string strMusic) {
	this->strMusic = strMusic;
	//��� ��������� ������ �����
	mainFonMusic.openFromFile("Music/" + this->strMusic);//���������� wav
	mainFonMusic.play();
	mainFonMusic.setLoop(true);//���� ����� �������
}

void Music::VolumeUp() {
	if ((mainFonMusic.getVolume() + 2) <= 100) {
		mainFonMusic.setVolume(mainFonMusic.getVolume() + 2);
	}
}

void Music::VolumeDown() {
	if ((mainFonMusic.getVolume() - 2) > 0) {
		mainFonMusic.setVolume(mainFonMusic.getVolume() - 2);
	}
}