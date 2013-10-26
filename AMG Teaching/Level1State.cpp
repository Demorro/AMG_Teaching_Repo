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
		return false;
	}
	
	stageCam = std::unique_ptr<Camera>(new Camera(Application::GetWindow(),1.0f,sf::Vector2f(0,0)));
	player = std::unique_ptr<Player>(new Player(PLAYERTEXTURE,PLAYERSTART));


	return true;
}

void Level1State::Update(sf::Event events, bool eventFired, double deltaTime)
{
	//Update things here, remember about deltatime for framerate independent movement
	stageCam->Update(events, eventFired, &player->GetPosition(),sf::Vector2f(0,-200));

	//temporary
	player->Move(deltaTime * 50.0,0.0f);
}

void Level1State::Draw(sf::RenderWindow &renderWindow)
{
	//Draw things here
	//Draw the level first
	loadedLevel.Draw(renderWindow);
	player->Render(renderWindow);


	if(DEBUG_STATE)
	{
		sf::RectangleShape playerDebugDrawRect;
		playerDebugDrawRect.setSize(sf::Vector2f(player->GetSprite().getGlobalBounds().width,player->GetSprite().getGlobalBounds().height));
		playerDebugDrawRect.setPosition(player->GetSprite().getGlobalBounds().left,player->GetSprite().getGlobalBounds().top);
		playerDebugDrawRect.setFillColor(sf::Color::Transparent);
		playerDebugDrawRect.setOutlineThickness(1.0f);
		playerDebugDrawRect.setOutlineColor(sf::Color::Green);
		renderWindow.draw(playerDebugDrawRect);
	}
}
