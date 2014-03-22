#include "Level2To3TransitionState.h"
#include "Application.h"

Level2To3TransitionState::Level2To3TransitionState(void) : State(State::LEVEL2TO3STATE)
{
	level = nullptr;
}


Level2To3TransitionState::~Level2To3TransitionState(void)
{
	level.reset();
}

bool Level2To3TransitionState::Load()
{
	level = std::unique_ptr<Level>(new Level(MEXICOTOJAPANTRANSITION, ID, false));	//Pass in the path to the level to be loaded, as well as the state ID of the current state.
	level->Load();
	return true;
}



void Level2To3TransitionState::Update(sf::Event events, bool eventFired, double deltaTime)
{
	if(level != nullptr)
	{
		level->Update(events,eventFired,deltaTime);
		CheckIfLevelWantsToSwitchState();
	}
}

void Level2To3TransitionState::CheckIfLevelWantsToSwitchState()
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

void Level2To3TransitionState::Draw(sf::RenderWindow &renderWindow)
{
	if(level != nullptr)
	{
		level->Draw(renderWindow);
	}
}

