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

	deathSequenceTimer.restart();
	deathSequenceTime = 1.0f;
	isOnDeathSequence = false;
	activeCheckPointPosition = loadedLevel->GetSpawnPosition();

	//Set the spawn point
	player->SetPosition(loadedLevel->GetSpawnPosition());
	groundLevel = player->GetPosition().y;
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
	//StateToSwitchToOnChange is the current state at the start, so this is fine
	LoadLevelConfigValues(*endingSequence, stateToSwitchToOnChange);

	if(stateToSwitchToOnChange == State::LEVEL1_STATE)
	{
		LoadLevelAudio(LEVEL1AUDIOCONFIG, stateToSwitchToOnChange);
	}
	else if(stateToSwitchToOnChange == State::LEVEL2_STATE)
	{
		LoadLevelAudio(LEVEL2AUDIOCONFIG, stateToSwitchToOnChange);
	}
	else if(stateToSwitchToOnChange == State::LEVEL3_STATE)
	{
		LoadLevelAudio(LEVEL3AUDIOCONFIG, stateToSwitchToOnChange);
	}

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

		//Check if the player has died, and react accordingly
		HandlePlayerDeaths(*player);

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
			ReactToPlayerWinning(deltaTime);
		}

		endingSequence->Update(events,eventFired,deltaTime);
		//Do the crossfading between ambient sounds depending on height, or fade em out completely if we're at the end sequence
		if(endingSequence->IsActive() == false)
		{
			FadeAmbientSoundsAccordingToHeight(player->GetPosition(),ambientCrossFadeMinHeight,ambientCrossFadeMaxHeight);
		}
		else
		{
			const float ambientAudioFadeSpeed = 100.0f;
			FadeOutAmbientTracks(ambientAudioFadeSpeed,deltaTime);
		}
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

void Level::HandlePlayerDeaths(Player &player)
{
	if(isOnDeathSequence == false)
	{
		//check for deaths
		for(int i = 0; i < loadedLevel->GetDeathZones().size(); i++)
		{
			if(player.GetCollider().intersects(loadedLevel->GetDeathZones()[i])) //then the player is in a death zone
			{
				//PLAYER IS DIEING
				activeCheckPointPosition = loadedLevel->GetSpawnPosition();

				deathSequenceTimer.restart();
				isOnDeathSequence = true;
				stageCam->SetLocked(true);
				player.SetIsAcceptingInput(false);

				//reload the falling platforms cause they might have been destroyed
				loadedLevel->ReloadFallingPlatforms();

				//find the active checkpoint
				for(int j = 0; j < loadedLevel->GetCheckPoints().size(); j++)
				{
					if(loadedLevel->GetCheckPoints()[j].IsActiveCheckPoint()) //this will work even if there is more than one active checkpoint, but there never should be
					{
						//this bit of maths maked it so the player spawns on the ground, rather than in the center origin of the checkpoint( or rather the players lower bound is equal to the checkpoints lower bound)
						sf::Vector2f positionToSpawnAt = loadedLevel->GetCheckPoints()[j].getPosition();
						positionToSpawnAt.y += ((loadedLevel->GetCheckPoints()[j].getGlobalBounds().height/2) - player.GetCollider().height/2);
						activeCheckPointPosition = positionToSpawnAt;
					}
				}
			}
		}
	}
	else
	{
		//if the death sequence is already going, run the stuff
		if(deathSequenceTimer.getElapsedTime().asSeconds() > deathSequenceTime)
		{
			//the time has run, time to respawn
			player.SetIsAcceptingInput(true);
			isOnDeathSequence = false;
			player.Respawn(activeCheckPointPosition);
			stageCam->SetLocked(false);
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
void Level::ReactToPlayerWinning(float deltaTime)
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
	//stateToSwitchToOnChange = State::LEVEL1_STATE;
	needToResetState = true;
}

void Level::QuitGame()
{
	stageCam->JumpToPoint(Application::GetWindow().getSize().x/2,Application::GetWindow().getSize().y/2); //if we dont do this the menustate has a fucked up viewport
	stateToSwitchToOnChange = State::MENU_STATE;
	needToResetState = true;
}


void Level::LoadLevelConfigValues(EndingSequence &endingSequence, State::StateID levelState)
{
	std::string docPath = LEVEL1CONFIG;

	if(levelState == State::StateID::LEVEL1_STATE)
	{
		docPath = LEVEL1CONFIG;
	}
	else if(levelState == State::StateID::LEVEL2_STATE)
	{
		docPath = LEVEL2CONFIG;
	}
	else if(levelState == State::StateID::LEVEL3_STATE)
	{
		docPath = LEVEL3CONFIG;
	}

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

void Level::FadeAmbientSoundsAccordingToHeight(sf::Vector2f playerPosition, float crossFadeMinHeightLevel, float crossFadeMaxHeightLevel)
{
	if((playerPosition.y - groundLevel < -crossFadeMinHeightLevel) && (playerPosition.y - groundLevel > -crossFadeMaxHeightLevel))
	{
		float skyMultiplicationValue = ((abs(playerPosition.y - groundLevel) - crossFadeMinHeightLevel) / (crossFadeMaxHeightLevel - crossFadeMinHeightLevel));
		skyLevelAmbience.setVolume(skyAmbienceVolume * skyMultiplicationValue);
		groundLevelAmbience.setVolume(groundAmbienceVolume * (1.0f - skyMultiplicationValue)); 
	}
}

void Level::FadeOutAmbientTracks(const float fadeSpeed, float deltaTime)
{
	if(skyLevelAmbience.getVolume() > 0)
	{
		if(skyLevelAmbience.getVolume() - (fadeSpeed * deltaTime) < 0)
		{
			skyLevelAmbience.setVolume(0);
		}
		else
		{
			skyLevelAmbience.setVolume(skyLevelAmbience.getVolume() - (fadeSpeed * deltaTime));
		}
	}

	if(groundLevelAmbience.getVolume() > 0)
	{
		if(groundLevelAmbience.getVolume() - (fadeSpeed * deltaTime) < 0)
		{
			groundLevelAmbience.setVolume(0);
		}
		else
		{
			groundLevelAmbience.setVolume(groundLevelAmbience.getVolume() - (fadeSpeed * deltaTime));
		}
	}
}

//This whole method is just bad. Dayum son. L2Code.
void Level::LoadLevelAudio(std::string audioConfigFilePath, State::StateID levelState)
{
	pugi::xml_document configDoc;
	LoadXMLDoc(configDoc,audioConfigFilePath);


	std::string musicPathNodeName = "MusicPath";
	std::string musicVolumeNodeName = "MusicVolume";
	std::string groundAmbienceNodeName = "GroundAmbiencePath";
	std::string skyAmbienceNodeName = "SkyAmbiencePath";
	std::string groundAmbienceVolumeNodeName = "GroundAmbienceVolume";
	std::string skyAmbienceVolumeNodeName = "SkyAmbienceVolume";
	std::string ambientCrossFadeMinHeightNodeName = "AmbienceCrossFadeMinHeight";
	std::string ambientCrossFadeMaxHeightNodeName = "AmbienceCrossFadeMaxHeight";

	std::string groundAmbiencePath;
	std::string skyAmbiencePath;
	groundAmbienceVolume = 100;
	skyAmbienceVolume = 100;
	musicVolume = 100;
	ambientCrossFadeMinHeight = 500;
	ambientCrossFadeMaxHeight = 1000;

	//Music to default to if we cant find any
	std::string musicPath = "";


	//Work through all the variables we need to load and load em, checking for if they're there or not each time
	//TODO : This whole things needs refactoring into a generic loader function, this violates DRY like mad.
	pugi::xml_node rootNode = configDoc.child("AudioConfig");

	//Load in level  music, ground and sky ambience paths and volume
	LoadTextValue(musicPath,rootNode,musicPathNodeName);
	LoadTextValue(groundAmbiencePath,rootNode,groundAmbienceNodeName);
	LoadTextValue(skyAmbiencePath,rootNode,skyAmbienceNodeName);
	LoadNumericalValue(musicVolume,rootNode,musicVolumeNodeName);
	LoadNumericalValue(groundAmbienceVolume,rootNode,groundAmbienceVolumeNodeName);
	LoadNumericalValue(skyAmbienceVolume,rootNode,skyAmbienceVolumeNodeName);

	//ambience crossfade height
	LoadNumericalValue(ambientCrossFadeMinHeight, rootNode,  ambientCrossFadeMinHeightNodeName);
	LoadNumericalValue(ambientCrossFadeMaxHeight, rootNode, ambientCrossFadeMaxHeightNodeName);

	groundLevelAmbienceBuffer.loadFromFile(groundAmbiencePath);
	skyLevelAmbienceBuffer.loadFromFile(skyAmbiencePath);
	groundLevelAmbience.setBuffer(groundLevelAmbienceBuffer);
	groundLevelAmbience.setVolume(groundAmbienceVolume);
	groundLevelAmbience.setLoop(true);
	skyLevelAmbience.setBuffer(skyLevelAmbienceBuffer);
	skyLevelAmbience.setVolume(skyAmbienceVolume);
	skyLevelAmbience.setLoop(true);


	skyLevelAmbience.setVolume(0); // Set this to 0 immediately, as we start at ground level and dont want both playing right off the bat
	groundLevelAmbience.play();
	skyLevelAmbience.play();

	//load in the music for this level
	interStateSingleton.LoadInterStateMusicFile(musicPath);

	//Start the music, it's loaded in the singleton
	if(interStateSingleton.InterStateMusicIsPlaying() == false)
	{
		if(interStateSingleton.GetIsVolumeOn() == true)
		{
			interStateSingleton.AdjustInterStateMusicVolume(musicVolume);
			interStateSingleton.SetInterStateMusicLooping(true);
			interStateSingleton.PlayInterStateMusic();
		}
	}
}