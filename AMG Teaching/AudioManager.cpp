#include "AudioManager.h"


AudioManager::AudioManager(void)
{
}


AudioManager::~AudioManager(void)
{
}


void AudioManager::LoadSoundFile(std::string filePath, AudioManager::GameSound sound)
{
	sf::SoundBuffer newLoadedAudio;

	std::cout << "Loading Sound : " << filePath << std::endl;
	if(!newLoadedAudio.loadFromFile(filePath))
	{

		std::cout << "Cannot load sound" << filePath << std::endl;
	}
	loadedAudioFiles[sound] = std::move(newLoadedAudio);
}

sf::SoundBuffer &AudioManager::GetSoundFile(AudioManager::GameSound soundToGet)
{
	if (loadedAudioFiles.find(soundToGet) == loadedAudioFiles.end())
	{
		//the audio is not in the map
		return sf::SoundBuffer();
	} 
	else 
	{
		//the audio is in the map
		return loadedAudioFiles[soundToGet];
	}
	/*
	switch(soundToGet)
	{
		case AudioManager::GameSound::Jump :
			return loadedAudioFiles[AudioManager::GameSound::Jump];
			break;
		case AudioManager::GameSound::Kick :
			return loadedAudioFiles[AudioManager::GameSound::Kick];
			break;
		case AudioManager::GameSound::Fart :
			return loadedAudioFiles[AudioManager::GameSound::Fart];
			break;
		case AudioManager::GameSound::PlatformFall :
			return loadedAudioFiles[AudioManager::GameSound::Fart];
			break;
		default:
			return sf::SoundBuffer();
			break;
	}
	return sf::SoundBuffer();
	*/
}