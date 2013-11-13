#include "Level1State.h"
#include "Application.h"

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
		std::cout << "Level failed to load for some reason" << std::endl;
		return false;
	}
	
	stageCam = std::unique_ptr<Camera>(new Camera(Application::GetWindow(),1.0f,sf::Vector2f(0,0)));
	player = std::unique_ptr<Player>(new Player(PLAYERTEXTURE,PLAYERSTART,audioManager));


	return true;
}

void Level1State::Update(sf::Event events, bool eventFired, double deltaTime)
{
	//Update things here, remember about deltatime for framerate independent movement

	//Update the player, handles movement and collision and every other darn thing
	player->Update(events,eventFired,deltaTime,loadedLevel.GetCollisionBounds(),loadedLevel.GetDestructibleObjects());
	//Camera update, follow the player
	stageCam->Update(events, eventFired, &player->GetPosition(),sf::Vector2f(0,0));
	//Update the level logic
	loadedLevel.Update(deltaTime);
}

void Level1State::Draw(sf::RenderWindow &renderWindow)
{
	//Draw things here
	//Draw the level first
	loadedLevel.Draw(renderWindow);
	player->Render(renderWindow);

}
