#include "EndingState.h"
#include "Application.h"

EndingState::EndingState(void) : State(State::ENDING_STATE)
{
	level = nullptr;
}


EndingState::~EndingState(void)
{
	level.reset();
}

bool EndingState::Load()
{
	level = std::unique_ptr<Level>(new Level(ENDINGLEVEL, ID, false, false, false));	//Pass in the path to the level to be loaded, as well as the state ID of the current state.
	level->Load();
	return true;
}


void EndingState::Update(sf::Event events, bool eventFired, double deltaTime)
{
	credits.Update(deltaTime);
	if(level != nullptr)
	{
		level->Update(events,eventFired,deltaTime);
		CheckIfLevelWantsToSwitchState();
	}
}

void EndingState::CheckIfLevelWantsToSwitchState()
{
	if(level != nullptr)
	{
		if(level->needToResetState)
		{
			interStateSingleton.StopInterStateMusic();
			interStateSingleton.AdjustInterStateMusicVolume(100);
			SwitchState(level->currentTargetState);
		}

		if(credits.ShouldSwitchOutOfCreditsState())
		{
			interStateSingleton.StopInterStateMusic();
			interStateSingleton.AdjustInterStateMusicVolume(100);
			level->GetStageCam().Reset();
			SwitchState(State::MENU_STATE);
		}
	}
}

void EndingState::Draw(sf::RenderWindow &renderWindow)
{
	if(level != nullptr)
	{
		level->Draw(renderWindow);
	}
	credits.Render(renderWindow);
}

