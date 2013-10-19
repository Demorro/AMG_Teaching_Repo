#include "Level1State.h"


Level1State::Level1State(void) : State(State::MENU_STATE)
{
}


Level1State::~Level1State(void)
{
}

bool Level1State::Load()
{
	//load things here, remember to do load checks if possible
	return true;
}

void Level1State::Update(sf::Event events, bool eventFired, double deltaTime)
{
	//Update things here, remember about deltatime for framerate independent movement
}

void Level1State::Draw(sf::RenderWindow &renderWindow)
{
	//Draw things here
}
