#include "Level.h"
#include "Application.h"

Level::Level(std::string levelPath, State::StateID levelState)
{
	//Load level
	loadedLevel = std::unique_ptr<LoadedLevel>(new LoadedLevel(levelPath));

	stateToSwitchToOnChange = levelState;
	needToResetState = false;
}


Level::~Level(void)
{


}

void Level::Load()
{
	//load things here, remember to do load checks if possible
	pauseMenu = std::unique_ptr<PauseMenu>(new PauseMenu(std::function<void()>(std::bind(&Level::ResumeGameFromPaused,this)), std::function<void()>(std::bind(&Level::RestartLevel,this)),  std::function<void()>(std::bind(&Level::QuitGame,this))));

	stageCam = std::unique_ptr<Camera>(new Camera(Application::GetWindow(),DEFAULTPLAYERSTART));
	player = std::unique_ptr<Player>(new Player(PLAYERTEXTURE,DEFAULTPLAYERSTART,sf::IntRect(0,166,126,156),sf::IntRect(0,0,70,150)));

	endingSequence = std::unique_ptr<EndingSequence>(new EndingSequence(stageCam.get()));

	//Set the spawn point
	player->SetPosition(loadedLevel->GetSpawnPosition());
	stageCam->JumpToPoint(player->GetPosition().x, player->GetPosition().y);

	pauseKeys.push_back(sf::Keyboard::Escape);
	pauseControllerButtons.push_back(std::pair<int,int>(0,START));
	gameIsPaused = false;
	pauseMenuTimer.restart();
	timeBetweenPauses = 0.15f;

	isSoundOn = interStateSingleton.GetIsVolumeOn();

	//initialise the timer and its graphical display
	int timerCharacterSize = 40;
	int timerXFromLeft = 25;
	int timerYFromTop = 10;
	gameTimer.reset();
	timerFont.loadFromFile(DEFAULTFONT);
	gameTimerText.setFont(timerFont);
	gameTimerText.setCharacterSize(timerCharacterSize);
	gameTimerText.setPosition(timerXFromLeft,timerYFromTop);
	gameTimerText.setString(GetTimerTextFromTime(gameTimer.getElapsedTime(),true));

	//Load level specific config values
	LoadLevelConfigValues(LEVEL1CONFIG, *endingSequence);

	needToResetState = false;
}

void Level::Update(sf::Event events, bool eventFired, double deltaTime)
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
			if(endingSequence->IsActive() == false)
			{
				gameTimer.resume();
			}
		}
		gameTimerText.setString(GetTimerTextFromTime(gameTimer.getElapsedTime(),true));

		//Run the logic for activating checkpoints and stuff
		RunCheckPointLogic(*player);

		//Check for victory
		if(PlayerHasMadeItToTheEnd())
		{
			gameTimer.pause();
			ReactToPlayerWinning();
		}

		endingSequence->Update(events,eventFired,deltaTime);
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

void Level::Draw(sf::RenderWindow &renderWindow)
{
	//Draw things here
	//Draw the level first
	loadedLevel->DrawLayersBehindPlayer(renderWindow);
	player->Render(renderWindow);
	loadedLevel->DrawLayersInFrontOfPlayer(renderWindow);

	//add in this vector(then remove it) to make the object appear in screen space
	sf::Vector2f screenCorrectionMoveVector = stageCam->GetScreenSpaceOffsetVector();
	gameTimerText.move(screenCorrectionMoveVector);
	renderWindow.draw(gameTimerText);
	gameTimerText.move(-screenCorrectionMoveVector);

	if(endingSequence->IsActive())
	{
		endingSequence->Render(renderWindow);
	}

	if(gameIsPaused)
	{
		pauseMenu->Render(renderWindow, *stageCam);
	}
}

void Level::RunCheckPointLogic(Player &player)
{

	if(loadedLevel != nullptr)
	{
		for(int i = 0; i < loadedLevel->GetCheckPoints().size(); i++)
		{
			if(player.GetCollider().intersects(loadedLevel->GetCheckPoints()[i].getGlobalBounds())) //if the player is touching a checkpoint
			{
				if(loadedLevel->GetCheckPoints()[i].IsActiveCheckPoint() == false)
				{
					//we need to activate this checkpoint
					//disable all the checkpoints
					for(int j = 0; j < loadedLevel->GetCheckPoints().size(); j++)
					{
						loadedLevel->GetCheckPoints()[j].Disable();
					}
					//enable this one
					loadedLevel->GetCheckPoints()[i].Activate();
				}
			}
		}
	}
}

void Level::PauseMenuLogic(sf::Event events, bool eventFired, double deltaTime)
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

bool Level::PlayerHasMadeItToTheEnd()
{
	for(int i = 0; i < loadedLevel->GetEndZones().size(); i++)
	{
		if(player->GetCollider().intersects(loadedLevel->GetEndZones()[i]))
		{
			return true;
		}
	}
	return false;
}

//remember this is run all the time when the player is in the ending collider. Might need to remove that little "quirk," but left it in as it could be useful
void Level::ReactToPlayerWinning()
{
	//So what we wanna do here is lock the camera, force the player to run offscreen, and then display the win statistics.
	stageCam->SetLocked(true);
	player->SetIsAcceptingInput(false);
	player->SetInputs(false,true,false,false,false);
	
	if(endingSequence->IsActive() == false)
	{
		endingSequence->SetIsActive(true);
		endingSequence->ResetEndingSequence(gameTimer.getElapsedTime().asMilliseconds());
	}
}

void Level::ResetPause(bool isGamePaused)
{
	pauseMenu->ResetTweens();
	gameIsPaused = isGamePaused;
	pauseMenuTimer.restart();
}

void Level::ResumeGameFromPaused()
{
	pauseMenu->ResetTweens();
	gameIsPaused = false;
	pauseMenuTimer.restart();
}

//Since we cant access switchstate from inside here, the level class stores a StateID "stateToSwitchToOnChange." The level state then looks at this, sees if its different, and if so, switches
void Level::RestartLevel()
{
	stateToSwitchToOnChange = State::LEVEL1_STATE;
	needToResetState = true;
}

void Level::QuitGame()
{
	stageCam->JumpToPoint(Application::GetWindow().getSize().x/2,Application::GetWindow().getSize().y/2); //if we dont do this the menustate has a fucked up viewport
	stateToSwitchToOnChange = State::MENU_STATE;
	needToResetState = true;
}


void Level::LoadLevelConfigValues(std::string docPath, EndingSequence &endingSequence)
{
	pugi::xml_document levelConfigDoc;

	LoadXMLDoc(levelConfigDoc,docPath);
	pugi::xml_node configRoot = levelConfigDoc.child("LevelSpecificConfig");

	float aPlusGradeMaxTime = 0;
	float aGradeMaxTime = 0;
	float bGradeMaxTime = 0;
	LoadNumericalValue(aPlusGradeMaxTime,configRoot,"APlusGrade");
	LoadNumericalValue(aGradeMaxTime,configRoot,"AGrade");
	LoadNumericalValue(bGradeMaxTime,configRoot,"BGrade");

	endingSequence.SetGradeTimes(aPlusGradeMaxTime, aGradeMaxTime, bGradeMaxTime);
}