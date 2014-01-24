#pragma once

#include "SFML\Graphics.hpp"
#include "SFML\Network.hpp"

//This is probably a really bad idea, but i honestly can't think of another way to communicate between states (other than scrapping this state system altogether, which might be a good idea to be honest, it's starting to show its age)
//This is for the client side only, quite important to remember ... i think. I forgot why.

// Macro for quickly grabbing the singleton instance
#define interStateSingleton InterStateSingleton::GetInstance()

// Singleton design pattern - global access, created once and safely destroyed
class InterStateSingleton
{
public:
	/** Grabs the singleton instance of the class. */
	static InterStateSingleton& GetInstance();

	void SetIsVolumeOn(bool isVolumeOn);
	bool GetIsVolumeOn();

private:
	InterStateSingleton();
	InterStateSingleton(InterStateSingleton const&); // Don't implement in .cpp
	void operator=(InterStateSingleton const&); // Don't implement in .cpp

	//Stuff that needs to be shared between states
	bool isVolumeOn;

};

