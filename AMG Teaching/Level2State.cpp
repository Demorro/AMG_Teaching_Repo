#include "Level2State.h"
#include "Application.h"

Level2State::Level2State(void) : State(State::LEVEL2_STATE)
{
	level = nullptr;
}


Level2State::~Level2State(void)
{
}

bool Level2State::Load()
{
	level = std::unique_ptr<Level>(new Level(MEXICOLEVEL, ID));	//Pass in the path to the level to be loaded, as well as the state ID of the current state.
	level->Load();
	return true;
}



void Level2State::Update(sf::Event events, bool eventFired, double deltaTime)
{
	if(level != nullptr)
	{
		level->Update(events,eventFired,deltaTime);
		CheckIfLevelWantsToSwitchState();
	}
}

void Level2State::CheckIfLevelWantsToSwitchState()
{
	if(level != nullptr)
	{
		if(level->needToResetState)
		{
			interStateSingleton.StopInterStateMusic();
			interStateSingleton.AdjustInterStateMusicVolume(100);
			SwitchState(level->stateToSwitchToOnChange);
		}
	}
}

void Level2State::Draw(sf::RenderWindow &renderWindow)
{
	if(level != nullptr)
	{
		level->Draw(renderWindow);
	}
}

