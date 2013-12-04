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
	loadedLevel = std::unique_ptr<Level>(new Level(TESTLEVEL,&audioManager));
	
	stageCam = std::unique_ptr<Camera>(new Camera(Application::GetWindow(),PLAYERSTART));
	player = std::unique_ptr<Player>(new Player(PLAYERTEXTURE,PLAYERSTART,sf::IntRect(0,166,126,156),sf::IntRect(0,0,70,150),audioManager));


	return true;
}

void Level1State::Update(sf::Event events, bool eventFired, double deltaTime)
{
	//Update things here, remember about deltatime for framerate independent movement

	//Update the level logic : Before the player so .Intersects() returns true
	loadedLevel->Update(deltaTime, *player, stageCam->GetVelocity());

	//Update the player, handles movement and collision and every other darn thing
	player->Update(events,eventFired,deltaTime,loadedLevel->GetCollisionBounds(),loadedLevel->GetDestructibleObjects());
	//Camera update, follow the player
	stageCam->Update(events, eventFired,deltaTime, &player->GetPosition());
}

void Level1State::Draw(sf::RenderWindow &renderWindow)
{
	//Draw things here
	//Draw the level first
	loadedLevel->Draw(renderWindow);
	player->Render(renderWindow);

}
