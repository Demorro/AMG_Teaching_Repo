#pragma once

#include "LoadedLevel.h"
#include "Assets.h"
#include "Camera.h"
#include "Player.h"
#include "PauseMenu.h"
#include "XBoxButtons.h"
#include "Chronometer.h"
#include "EndingSequence.h"
#include "State.h"

class Level
{
public:
	Level(std::string levelPath, State::StateID levelState);
	~Level(void);
	
	void Load();
	void Update(sf::Event events, bool eventFired, double deltaTime);
	void Draw(sf::RenderWindow &renderWindow);

	//This is neccesary because we need to restart the state if we hit restart in the pause menu, or quit the state, or go to the new level.
	//The bool is neccesary cause sometimes we want to reset the state to the state we're currently in, so we cant just do a compare or it wont pick up
	bool needToResetState;
	//Since we cant access switchstate from inside here, the level class stores a StateID "stateToSwitchToOnChange." The level state then looks at this to see what state to go too.
	State::StateID stateToSwitchToOnChange;

private:
	//Loads in and stored data representations of the level, as well as rendering it
	std::unique_ptr<LoadedLevel> loadedLevel;
	
	//Controls the camera for this stage
	std::unique_ptr<Camera> stageCam;

	//The main player character
	std::unique_ptr<Player> player;

	//The ending sequence text objects
	std::unique_ptr<EndingSequence> endingSequence;

	//Loads level specific config values, passes the win times to the end sequence directly
	void LoadLevelConfigValues(std::string docPath, EndingSequence &endingSequence);

	//runs logic to make the pause menu work
	void PauseMenuLogic(sf::Event events, bool eventFired, double deltaTime);

	//Runs the checkpointLogic, updating the current checkpoint and updating the animations if they have any.
	void RunCheckPointLogic(Player &player);

	//resets the pause tweens and clock and whatnot
	void ResetPause(bool isGamePaused);

	//The pause menu, plus bool for determining whether the game is paused, (used for stopping all the level update logic)
	std::unique_ptr<PauseMenu> pauseMenu;
	bool gameIsPaused;
	std::vector<sf::Keyboard::Key> pauseKeys;
	std::vector<std::pair<int,int>> pauseControllerButtons;
	//dont want to be able to spam the pause menu too much, so use a timer;
	sf::Clock pauseMenuTimer;
	float timeBetweenPauses;

	//Checks if the player has reached the end of the level,
	bool PlayerHasMadeItToTheEnd();
	//reacts accordingly
	void ReactToPlayerWinning();

	//these functions are used in the pause menu
	void ResumeGameFromPaused();
	void RestartLevel();
	void QuitGame();
	

	bool isSoundOn;

	//Level timer
	sftools::Chronometer gameTimer;
	sf::Font timerFont;
	sf::Text gameTimerText;

};

