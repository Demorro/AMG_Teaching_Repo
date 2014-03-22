#pragma once
#include "state.h"
#include "Level.h"
#include "Assets.h"
#include "Camera.h"
#include "Player.h"
#include "PauseMenu.h"
#include "XBoxButtons.h"
#include "Chronometer.h"
#include "EndingSequence.h"

#define DEFAULTPLAYERSTART sf::Vector2f(300,545)

class Level1To2TransitionState : public State
{
public:
	Level1To2TransitionState();
	~Level1To2TransitionState(void);

	/** 
	 * Loads Level1To2TransitionState content.
	 * Returns true if successful, otherwise returns false.
	 */
	bool Load();
	
	/** Main loop. */
	void Update(sf::Event events, bool eventFired, double deltaTime);

	/** Renders all of the applicable state content. */
	void Draw(sf::RenderWindow &renderWindow);

private:
	std::unique_ptr<Level> level;

	//The level will sometimes need to trigger state changes, but since it cant access SwitchState, this checks if the state needs to be switched. 
	void CheckIfLevelWantsToSwitchState();
	
};