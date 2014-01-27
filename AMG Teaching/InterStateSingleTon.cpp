#include "InterStateSingleton.h"



InterStateSingleton::InterStateSingleton()
{
	//Load up the menu music
	interStateSingleton.SetIsVolumeOn(true);
	interStateSingleton.LoadMenuMusicFile(MENUBACKINGMUSIC);
	interStateSingleton.SetMenuMusicLooping(true);
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
bool InterStateSingleton::LoadMenuMusicFile(std::string menuMusicFilePath)
{
	if(!menuMusic.openFromFile(menuMusicFilePath))
	{
		return false;
	}
	return true;
}
void InterStateSingleton::PlayMenuMusic()
{
	menuMusic.play();
}
void InterStateSingleton::AdjustMenuMusicVolume(float volume)
{
	menuMusic.setVolume(volume);
}
void InterStateSingleton::StopMenuMusic()
{
	menuMusic.stop();
}
bool InterStateSingleton::MenuMusicIsPlaying()
{
	if(menuMusic.getStatus() == sf::SoundSource::Status::Playing)
	{
		return true;
	}
	else
	{

		return false;
	}
}
void InterStateSingleton::SetMenuMusicLooping(bool looping)
{
	menuMusic.setLoop(looping);
}