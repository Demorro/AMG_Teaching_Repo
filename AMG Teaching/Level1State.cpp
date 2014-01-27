#include "Level1State.h"
#include "Application.h"

Level1State::Level1State(void) : State(State::LEVEL1_STATE)
{
}


Level1State::~Level1State(void)
{
}

bool Level1State::Load()
{
	//std::function<void()>(std::bind(&MenuState::GoToFirstLevelState,this))

	pauseMenu = std::unique_ptr<PauseMenu>(new PauseMenu(std::function<void()>(std::bind(&Level1State::ResumeGameFromPaused,this)), std::function<void()>(std::bind(&Level1State::RestartLevel,this)),  std::function<void()>(std::bind(&Level1State::QuitGame,this))));

	//load things here, remember to do load checks if possible
	//Load level 1
	loadedLevel = std::unique_ptr<Level>(new Level(TESTLEVEL,&audioManager));
	
	stageCam = std::unique_ptr<Camera>(new Camera(Application::GetWindow(),PLAYERSTART));
	player = std::unique_ptr<Player>(new Player(PLAYERTEXTURE,PLAYERSTART,sf::IntRect(0,166,126,156),sf::IntRect(0,0,70,150),audioManager));

	pauseKeys.push_back(sf::Keyboard::Escape);
	pauseControllerButtons.push_back(std::pair<int,int>(0,START));
	gameIsPaused = false;
	pauseMenuTimer.restart();
	timeBetweenPauses = 0.15f;

	isSoundOn = interStateSingleton.GetIsVolumeOn();

	//initialise the timer and its graphical display
	int timerCharacterSize = 40;
	int timerXFromLeft = 35;
	int timerYFromTop = 20;
	gameTimer.reset();
	timerFont.loadFromFile(DEFAULTFONT);
	gameTimerText.setFont(timerFont);
	gameTimerText.setCharacterSize(timerCharacterSize);
	gameTimerText.setPosition(timerXFromLeft,timerYFromTop);
	gameTimerText.setString(GetTimerTextFromTime(gameTimer.getElapsedTime()));
	
	return true;
}

void Level1State::Update(sf::Event events, bool eventFired, double deltaTime)
{
	//Update things here, remember about deltatime for framerate independent movement
	
	//Dont run main level logic if game is paused. Doing it with the timer means theres a short delay after unpausing, and means that the button used to unpause wont be registered in the first game iteration, which is good.
	if((pauseMenuTimer.getElapsedTime().asSeconds() > timeBetweenPauses) && (gameIsPaused == false))
	{
		//Need to check if the player is on a moving platform before the level step to make it work right
		player->DetermineIfPlayerIsOnMovingPlatform(loadedLevel->GetSpecialPlatforms());

		//Update the level logic : Before the player so .Intersects() returns true
		loadedLevel->Update(deltaTime, *player, stageCam->GetVelocity());

		//Update the player, handles movement and collision and every other darn thing
		player->Update(events,eventFired,deltaTime,loadedLevel->GetStaticCollisionBounds(),loadedLevel->GetSpecialPlatforms(),loadedLevel->GetDestructibleObjects(),isSoundOn);

		//Camera update, follow the player
		stageCam->Update(events, eventFired,deltaTime, &player->GetPosition());

		//Update the level timer text
		if(!gameTimer.isRunning())
		{
			gameTimer.resume();
		}
		gameTimerText.setString(GetTimerTextFromTime(gameTimer.getElapsedTime()));
	}
	else
	{
		//Dont want the timer running if the game is paused
		if(gameTimer.isRunning())
		{
			gameTimer.pause();
		}
	}

	//you always need to be doing pause menu logic
	PauseMenuLogic(events,eventFired,deltaTime);
}

void Level1State::Draw(sf::RenderWindow &renderWindow)
{
	//Draw things here
	//Draw the level first
	loadedLevel->DrawLayersBehindPlayer(renderWindow);
	player->Render(renderWindow);
	loadedLevel->DrawLayersInFrontOfPlayer(renderWindow);

	//add in this vector(then remove it) to make the object appear in screen space
	sf::Vector2f screenCorrectionMoveVector = stageCam->GetPosition();
	screenCorrectionMoveVector.x -= Application::GetWindow().getSize().x/2;
	screenCorrectionMoveVector.y -= Application::GetWindow().getSize().y/2;
	gameTimerText.move(screenCorrectionMoveVector);
	renderWindow.draw(gameTimerText);
	gameTimerText.move(-screenCorrectionMoveVector);

	if(gameIsPaused)
	{
		pauseMenu->Render(renderWindow, *stageCam);
	}
}

void Level1State::PauseMenuLogic(sf::Event events, bool eventFired, double deltaTime)
{
	//dont do pause menu logic if the game isn't paused
	if(gameIsPaused)
	{
		pauseMenu->Update(events,eventFired,deltaTime);
	}

	if(pauseMenuTimer.getElapsedTime().asSeconds() > timeBetweenPauses)
	{
		if(eventFired)
		{
			if(events.type == sf::Event::KeyReleased) //keyboard key pausing
			{
				for(int i = 0; i < pauseKeys.size(); i++)
				{
					if(events.key.code == pauseKeys[i])
					{
						//toggle between paused/not paused
						if(gameIsPaused == false)
						{
							ResetPause(true);
						}
						else
						{
							ResetPause(false);
						}
					}
				}
			}

			if(events.type == sf::Event::JoystickButtonPressed) //controller button pausing
			{
				for(int i = 0; i < pauseControllerButtons.size(); i++)
				{
					if((events.joystickButton.joystickId == pauseControllerButtons[i].first) && (events.joystickButton.button == pauseControllerButtons[i].second))
					{
						//toggle between paused/not paused
						if(gameIsPaused == false)
						{
							ResetPause(true);
						}
						else
						{
							ResetPause(false);
						}
					}
				}
			}
		}
	}
}

void Level1State::ResetPause(bool isGamePaused)
{
	pauseMenu->ResetTweens();
	gameIsPaused = isGamePaused;
	pauseMenuTimer.restart();
}

void Level1State::ResumeGameFromPaused()
{
	pauseMenu->ResetTweens();
	gameIsPaused = false;
	pauseMenuTimer.restart();
}


void Level1State::RestartLevel()
{
	SwitchState(State::LEVEL1_STATE);
}

void Level1State::QuitGame()
{
	stageCam->JumpToPoint(Application::GetWindow().getSize().x/2,Application::GetWindow().getSize().y/2); //if we dont do this the menustate has a fucked up viewport
	SwitchState(State::MENU_STATE);
}

//take in a time object, and return a formatted H:M:S string
std::string Level1State::GetTimerTextFromTime(sf::Time time)
{
	int milliSeconds = time.asMilliseconds();
	int seconds;
	int minutes;
	int hours;

	hours = milliSeconds / (1000*60*60);
	minutes = (milliSeconds % (1000*60*60)) / (1000*60);
	seconds = ((milliSeconds % (1000*60*60)) % (1000*60)) / 1000;


	std::string returnableTimeString;
	returnableTimeString = "Time : ";

	std::stringstream timeStream1(std::stringstream::in | std::stringstream::out);
	timeStream1 << hours;
	returnableTimeString += timeStream1.str();
	returnableTimeString += " : ";

	std::stringstream timeStream2(std::stringstream::in | std::stringstream::out);
	timeStream2 << minutes;
	returnableTimeString += timeStream2.str();
	returnableTimeString += " : ";

	std::stringstream timeStream3(std::stringstream::in | std::stringstream::out);
	timeStream3 << seconds;
	returnableTimeString += timeStream3.str();

	return returnableTimeString;
}