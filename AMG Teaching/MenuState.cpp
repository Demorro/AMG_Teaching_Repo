#include "MenuState.h"


MenuState::MenuState(void) : State(State::MENU_STATE)
{
}


MenuState::~MenuState(void)
{
}

bool MenuState::Load()
{
	//load things here, remember to do load checks if possible
	return true;
}

void MenuState::Update(sf::Event events, bool eventFired, double deltaTime)
{
	//Update things here, remember about deltatime for framerate independent movement
}

void MenuState::Draw(sf::RenderWindow &renderWindow)
{
	//Draw things here
}

