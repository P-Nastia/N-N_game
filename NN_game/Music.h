#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <string>

class Music
{
public:
	sf::SoundBuffer soundBuffer;
	sf::Sound mouseClick;
	std::string strMusic;
	sf::Music mainFonMusic;

	void setMusic(std::string strMusic);
	void setFoneMusic(std::string strMusic);

	void VolumeUp();

	void VolumeDown();
};