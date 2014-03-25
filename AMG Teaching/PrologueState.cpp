#include "PrologueState.h"
#include "Application.h"

PrologueState::PrologueState(void) : State(State::PROLOGUE_STATE)
{
	level = nullptr;
}


PrologueState::~PrologueState(void)
{
	level.reset();
}

bool PrologueState::Load()
{
	level = std::unique_ptr<Level>(new Level(PROLOGUELEVEL, ID, false, true, false));	//Pass in the path to the level to be loaded, as well as the state ID of the current state.
	level->Load();
	return true;
}


void PrologueState::Update(sf::Event events, bool eventFired, double deltaTime)
{
	if(level != nullptr)
	{
		level->Update(events,eventFired,deltaTime);
		CheckIfLevelWantsToSwitchState();
	}
}

void PrologueState::CheckIfLevelWantsToSwitchState()
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

void PrologueState::Draw(sf::RenderWindow &renderWindow)
{
	if(level != nullptr)
	{
		level->Draw(renderWindow);
	}
}

