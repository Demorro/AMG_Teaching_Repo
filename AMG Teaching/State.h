#pragma once
#include <SFML\Graphics.hpp>

//The base class for all states

class State
{
public:

	//This enum defines the names of all the states, and acts as a control to switch between them via the SwitchState() function
	enum StateID
	{
		RESTART_STATE,
		MENU_STATE,
		LEVEL1_STATE,
		LEVEL2_STATE,
		LEVEL3_STATE,
		CONTROL_STATE
	};

	// Constructor intended to set the inital ID. 
	State(StateID ID);

	// Base destructor. 
	virtual ~State();

	// Abstract method intended to act as the main loop of the state.
	virtual void Update(sf::Event events, bool eventFired, double deltaTime) = 0;

	// Abstract method intended to render all objects of the state. 
	virtual void Draw(sf::RenderWindow& window) = 0;

	/**
		* Abstract method intended to load state content.
		* For instance: positioning objects, loading images, setting parameters.
		* Returns true if the operation was successful, otherwise returns false.
	*/
	virtual bool Load() = 0;

	// Returns the targetID variable. 
	StateID GetTarget();
		
	// Returns true if targetID differs from ID, otherwise returns false.
	bool Switch();

protected:
	// The unique ID of the state. 
	StateID ID;

	// Sets the state targetID value to the param.
	void SwitchState(StateID state);

private:
	// The unique ID of the desired state to switch to.
	StateID targetID;
};
