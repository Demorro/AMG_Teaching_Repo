#pragma once

#include "SFML\Graphics.hpp"
#include "SFML\Audio.hpp"
#include "Assets.h"

//This is probably a really bad idea, but i honestly can't think of another way to communicate between states (other than scrapping this state system altogether, which might be a good idea to be honest, it's starting to show its age)

// Macro for quickly grabbing the singleton instance
#define interStateSingleton InterStateSingleton::GetInstance()

// Singleton design pattern - global access, created once and safely destroyed
class InterStateSingleton
{
public:
	/** Grabs the singleton instance of the class. */
	static InterStateSingleton& GetInstance();

	void SetIsVolumeOn(bool volume);
	bool GetIsVolumeOn();

	//For Music that needs to keep playing even through state changes/reset
	bool LoadInterStateMusicFile(std::string menuMusicFilePath);
	void PlayInterStateMusic();
	void AdjustInterStateMusicVolume(float volume);
	void StopInterStateMusic();
	bool InterStateMusicIsPlaying();
	void SetInterStateMusicLooping(bool looping);

private:
	InterStateSingleton();
	InterStateSingleton(InterStateSingleton const&); // Don't implement in .cpp
	void operator=(InterStateSingleton const&); // Don't implement in .cpp

	//Stuff that needs to be shared between states
	bool isVolumeOn;

	//Since the menu jumps between multiple states, put the menu music stuff here, so it's consistent
	sf::Music interStateMusic;

};

