#include "State.h"

State::State(StateID ID)
{
	this->ID = targetID = ID;
}

State::~State()
{
}

State::StateID State::GetTarget()
{
	return targetID;
}

bool State::Switch()
{
	if(ID != targetID)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void State::SwitchState(State::StateID state)
{
	targetID = state;

	//This sets this ID to the restart state if you try and switch to this state, meaning that if you try and switch to this state, the state will reload.
	if(targetID == state)
	{
		this->ID = RESTART_STATE;
	}
}
