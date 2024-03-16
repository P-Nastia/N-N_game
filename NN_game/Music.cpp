#include "Music.h"

void Music::setMusic(std::string strMusic) {
	this->strMusic = strMusic;
	//для зберігання короткочасних звуків
	soundBuffer.loadFromFile("Music/" + this->strMusic);
	mouseClick.setBuffer(soundBuffer);
	mouseClick.play();
}

void Music::setFoneMusic(std::string strMusic) {
	this->strMusic = strMusic;
	//для зберігання довгих звуків
	mainFonMusic.openFromFile("Music/" + this->strMusic);//розширення wav
	mainFonMusic.play();
	mainFonMusic.setLoop(true);//буде грати постійно
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