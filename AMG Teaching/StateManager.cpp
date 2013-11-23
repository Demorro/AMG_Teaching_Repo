#include "StateManager.h"

StateManager::StateManager()
{
	curState = NULL;
	//Define what state we start with.
	SwitchState(State::LEVEL1_STATE);

	deltaTime = 0.0f;
	newTime = 0.0f;

}

StateManager::~StateManager()
{
}

void StateManager::Update(sf::Event events, bool eventFired)
{
	if(curState != NULL)
	{
		// Switch the state if a signal has been given from the current state
		if(curState->Switch())
		{
			SwitchState(curState->GetTarget());
		}

		//Run the update loop for the current state
		curState->Update(events, eventFired, deltaTime);
	}

	FindDeltaTime();
}

void StateManager::Draw(sf::RenderWindow& window)
{
	if(curState != NULL)
	{
		curState->Draw(window);
	}

}

//Automatically detects if the state needs to be switched and then handles the switching.
void StateManager::SwitchState(State::StateID stateID)
{
	// Delete previous state, if set
	if(curState != NULL)
	{
		delete curState;
		curState = NULL;
	}

	std::cout << "Switched to: ";

	switch(stateID)
	{
		case State::MENU_STATE:
			std::cout << "MENU." << std::endl;
			curState = new MenuState();
		break;

		case State::LEVEL1_STATE:
			std::cout << "LEVEL1." << std::endl;
			curState = new Level1State();
		break;
	}

	if(curState->Load() == false)
	{
		std::cout << "Failed to load state with ID : " << stateID << std::endl;
	}
}


//Works out the deltatime, called each frame
void StateManager::FindDeltaTime()
{
	// Using microseconds give us a high enough accuracy to get smooth movement.
	sf::Int64 oldTime;
	oldTime = newTime;
	newTime = deltaTimeClock.getElapsedTime().asMicroseconds();

	deltaTime = double(newTime - oldTime)/1000000;
	//Things get weird if deltaTime is huuuuuge, i.e less that 8fps or so, so cap it at that. The reason for this is because the colliders will be moving in huge jumps, they might just clip through a collider and bugger off.
	if(deltaTime > 0.125f)
	{
		deltaTime = 0.125f;
	}
}
