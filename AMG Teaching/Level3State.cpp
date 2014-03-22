#include "Level3State.h"
#include "Application.h"

Level3State::Level3State(void) : State(State::LEVEL3_STATE)
{
	level = nullptr;
}


Level3State::~Level3State(void)
{
	level.reset();
}

bool Level3State::Load()
{
	level = std::unique_ptr<Level>(new Level(JAPANLEVEL, ID, true));	//Pass in the path to the level to be loaded, as well as the state ID of the current state.
	level->Load();
	return true;
}



void Level3State::Update(sf::Event events, bool eventFired, double deltaTime)
{
	if(level != nullptr)
	{
		level->Update(events,eventFired,deltaTime);
		CheckIfLevelWantsToSwitchState();
	}
}

void Level3State::CheckIfLevelWantsToSwitchState()
{
	if(level != nullptr)
	{
		if(level->needToResetState)
		{
			interStateSingleton.StopInterStateMusic();
			interStateSingleton.AdjustInterStateMusicVolume(100);
			SwitchState(level->currentTargetState);
		}
	}
}

void Level3State::Draw(sf::RenderWindow &renderWindow)
{
	if(level != nullptr)
	{
		level->Draw(renderWindow);
	}
}

