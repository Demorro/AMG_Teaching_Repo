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
	//Load level 1
	if(!loadedLevel.LoadLevel(TESTLEVEL))
	{
		return false;
	}

	//Set the colour of the sky, cleared to this before anything else in draw.
	skyColour.r = 80;
	skyColour.g = 160;
	skyColour.b = 230;

	return true;
}

void Level1State::Update(sf::Event events, bool eventFired, double deltaTime)
{
	//Update things here, remember about deltatime for framerate independent movement
}

void Level1State::Draw(sf::RenderWindow &renderWindow)
{
	//Draw things here
	//Clear the screen to the sky colour
	renderWindow.clear(skyColour);

	//Draw the level
	loadedLevel.Draw(renderWindow);
}
