#pragma once
#include "state.h"
#include "Level.h"
#include "Assets.h"
#include "Camera.h"
#include "Player.h"
#include "AudioManager.h"
#include "PauseMenu.h"
#include "XBoxButtons.h"
#include "Chronometer.h"

#define DEFAULTPLAYERSTART sf::Vector2f(300,545)

class Level1State : public State
{
public:
	Level1State();
	~Level1State(void);

	/** 
	 * Loads Level1State content.
	 * Returns true if successful, otherwise returns false.
	 */
	bool Load();
	
	/** Main loop. */
	void Update(sf::Event events, bool eventFired, double deltaTime);

	/** Renders all of the applicable state content. */
	void Draw(sf::RenderWindow &renderWindow);

private:

	//runs logic to make the pause menu work
	void PauseMenuLogic(sf::Event events, bool eventFired, double deltaTime);

	//resets the pause tweens and clock and whatnot
	void ResetPause(bool isGamePaused);

	//Loads in and stored data representations of the level, as well as rendering it
	std::unique_ptr<Level> loadedLevel;
	
	//Controls the camera for this stage
	std::unique_ptr<Camera> stageCam;

	//The main player character
	std::unique_ptr<Player> player;

	//The pause menu, plus bool for determining whether the game is paused, (used for stopping all the level update logic)
	std::unique_ptr<PauseMenu> pauseMenu;
	bool gameIsPaused;
	std::vector<sf::Keyboard::Key> pauseKeys;
	std::vector<std::pair<int,int>> pauseControllerButtons;
	//dont want to be able to spam the pause menu too much, so use a timer;
	sf::Clock pauseMenuTimer;
	float timeBetweenPauses;

	AudioManager audioManager;

	void ResumeGameFromPaused();
	void RestartLevel();
	void QuitGame();

	bool isSoundOn;

	//Level timer
	sftools::Chronometer gameTimer;
	sf::Font timerFont;
	sf::Text gameTimerText;

	std::string GetTimerTextFromTime(sf::Time time);

};