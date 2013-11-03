#pragma once

#include <iostream>
#include "State.h"
#include "MenuState.h"
#include "Level1State.h"

#define SIMULATIONTIMESTEP 16.666f

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

	//Accumulates the deltatime and is subtracted from when the simulation runs an update step. Refer to www.http://gafferongames.com/game-physics/fix-your-timestep/ for more info
	double deltaTimeAccumulator;
};
