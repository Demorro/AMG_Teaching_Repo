#pragma once

#include <iostream>
#include "State.h"
#include "MenuState.h"
#include "Level1State.h"
#include "ClockHud.h"
#include "FrameClock.h"

class StateManager
{
public:
	// Default constructor. 
	StateManager();

	// Default destructor. 
	~StateManager();

	
	//Switches the curState to the specified target state.  DO NOT CALL THIS DIRECTLY, CALL SWITCHSTATE(StateID) FROM INSIDE CURRENT STATE INSTEAD
	void SwitchState(State::StateID stateID);

	// Main loop which updates the current state. 
	void Update(sf::Event events, bool eventFired);

	// Draws the current state. 
	void Draw(sf::RenderWindow& window);

private:
	// A pointer to the current state. 
	State* curState;

	//Finds the delta time each frame. Multiply any time dependent action by deltaTime to get an action that moves at a speed independent of framerate
	void FindDeltaTime();

	//The deltaTime variable and two storage variables for finding it in FindDeltaTime()
	double deltaTime;
	sf::Int64 newTime;
	sf::Clock deltaTimeClock;
};
