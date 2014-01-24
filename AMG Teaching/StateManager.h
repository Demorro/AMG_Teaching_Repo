#pragma once

#include <iostream>
#include "State.h"
#include "MenuState.h"
#include "Level1State.h"
#include "ControlState.h"
#include "ClockHud.h"
#include "FrameClock.h"
#include <queue>

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
	double newTime;
	sf::Clock deltaTimeClock;

	//Not entirely sure if this is crazy or not, but im storing the last 20 or so values and using them to smooth the current deltaTime
	std::queue<double> deltaTimeSmoothBuffer;
	int smoothBufferLength;
};
