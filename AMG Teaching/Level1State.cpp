#include "Level1State.h"
#include "Application.h"

Level1State::Level1State(void) : State(State::LEVEL1_STATE)
{
	level1 = nullptr;
}


Level1State::~Level1State(void)
{
}

bool Level1State::Load()
{
	level1 = std::unique_ptr<Level>(new Level(TESTLEVEL, ID));	//Pass in the path to the level to be loaded, as well as the state ID of the current state.
	level1->Load();
	return true;
}

void Level1State::Update(sf::Event events, bool eventFired, double deltaTime)
{
	if(level1 != nullptr)
	{
		level1->Update(events,eventFired,deltaTime);
		CheckIfLevelWantsToSwitchState();
	}
}

void Level1State::CheckIfLevelWantsToSwitchState()
{
	if(level1 != nullptr)
	{
		if(level1->needToResetState)
		{
			SwitchState(level1->stateToSwitchToOnChange);
		}
	}
}

void Level1State::Draw(sf::RenderWindow &renderWindow)
{
	if(level1 != nullptr)
	{
		level1->Draw(renderWindow);
	}
}

