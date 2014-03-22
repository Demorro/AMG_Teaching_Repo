#include "Level1To2TransitionState.h"
#include "Application.h"

Level1To2TransitionState::Level1To2TransitionState(void) : State(State::LEVEL1TO2STATE)
{
	level = nullptr;
}


Level1To2TransitionState::~Level1To2TransitionState(void)
{
	level.reset();
}

bool Level1To2TransitionState::Load()
{
	level = std::unique_ptr<Level>(new Level(SCOTLANDTOMEXICOTRANSITION, ID, false));	//Pass in the path to the level to be loaded, as well as the state ID of the current state.
	level->Load();
	return true;
}



void Level1To2TransitionState::Update(sf::Event events, bool eventFired, double deltaTime)
{
	if(level != nullptr)
	{
		level->Update(events,eventFired,deltaTime);
		CheckIfLevelWantsToSwitchState();
	}
}

void Level1To2TransitionState::CheckIfLevelWantsToSwitchState()
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

void Level1To2TransitionState::Draw(sf::RenderWindow &renderWindow)
{
	if(level != nullptr)
	{
		level->Draw(renderWindow);
	}
}

