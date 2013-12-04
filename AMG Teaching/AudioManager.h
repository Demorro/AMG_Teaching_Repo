#pragma once
#include "SFML\Audio.hpp"
#include <map>
#include <memory>
#include <iostream>

class AudioManager
{
public:
	enum GameSound
	{
		Jump,
		Kick,
		Fart,
		PlatformFall
	};

	AudioManager(void);
	~AudioManager(void);

	//Loads a sound into the memory, ensuring no duplicates
	void LoadSoundFile(std::string filePath, GameSound sound);
	//Get a previously loaded sound from memory, using the Gamesound enum to define. This should be loaded into sf::sprite
	sf::SoundBuffer &GetSoundFile(GameSound soundFile);

private:
	
	//This map stores the sounds used in the game, uses a hashmap with a key(normally the filename of the sound file) to ensure to duplication, thus sounding memory.
	std::map<GameSound, sf::SoundBuffer> loadedAudioFiles;
};

