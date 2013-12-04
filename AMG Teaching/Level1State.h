#pragma once
#include "state.h"
#include "Level.h"
#include "Assets.h"
#include "Camera.h"
#include "Player.h"
#include "AudioManager.h"

#define PLAYERSTART sf::Vector2f(300,545)

class Level1State : public State
{
public:
	Level1State();
	~Level1State(void);

	/** 
	 * Loads Level1State content.
	 * Returns true if successful, otherwise returns false.
	 */
	bool Load();
	
	/** Main loop. */
	void Update(sf::Event events, bool eventFired, double deltaTime);

	/** Renders all of the applicable state content. */
	void Draw(sf::RenderWindow &renderWindow);

private:
	//Loads in and stored data representations of the level, as well as rendering it
	std::unique_ptr<Level> loadedLevel;
	
	//Controls the camera for this stage
	std::unique_ptr<Camera> stageCam;

	//The main player character
	std::unique_ptr<Player> player;

	AudioManager audioManager;

};