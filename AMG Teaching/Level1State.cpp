#include "Level1State.h"
#include "Application.h"

Level1State::Level1State(void) : State(State::LEVEL1_STATE)
{
	level = nullptr;
}


Level1State::~Level1State(void)
{
}

bool Level1State::Load()
{
	level = std::unique_ptr<Level>(new Level(SCOTLANDLEVEL, ID));	//Pass in the path to the level to be loaded, as well as the state ID of the current state.
	level->Load();
	return true;
}



void Level1State::Update(sf::Event events, bool eventFired, double deltaTime)
{
	if(level != nullptr)
	{
		level->Update(events,eventFired,deltaTime);
		CheckIfLevelWantsToSwitchState();
	}
}

void Level1State::CheckIfLevelWantsToSwitchState()
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

void Level1State::Draw(sf::RenderWindow &renderWindow)
{
	if(level != nullptr)
	{
		level->Draw(renderWindow);
	}
}

