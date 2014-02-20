#include "InterStateSingleton.h"



InterStateSingleton::InterStateSingleton()
{
	//Load up the menu music
	interStateSingleton.SetIsVolumeOn(true);
}

InterStateSingleton& InterStateSingleton::GetInstance()
{
	// Instantiated on first use, reference passed thereafter
	static InterStateSingleton instance;
	return instance;
}

void InterStateSingleton::SetIsVolumeOn(bool volume)
{
	isVolumeOn = volume;
}

bool InterStateSingleton::GetIsVolumeOn()
{
	return isVolumeOn;
}


//For the interstate menu music
bool InterStateSingleton::LoadInterStateMusicFile(std::string menuMusicFilePath)
{
	if(!interStateMusic.openFromFile(menuMusicFilePath))
	{
		return false;
	}
	return true;
}
void InterStateSingleton::PlayInterStateMusic()
{
	interStateMusic.play();
}
void InterStateSingleton::AdjustInterStateMusicVolume(float volume)
{
	interStateMusic.setVolume(volume);
}
void InterStateSingleton::StopInterStateMusic()
{
	interStateMusic.stop();
}
bool InterStateSingleton::InterStateMusicIsPlaying()
{
	if(interStateMusic.getStatus() == sf::SoundSource::Status::Playing)
	{
		return true;
	}
	else
	{

		return false;
	}
}
void InterStateSingleton::SetInterStateMusicLooping(bool looping)
{
	interStateMusic.setLoop(looping);
}