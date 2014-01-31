#pragma once
#include "state.h"
#include "Assets.h"
#include "MotionSpline.h"
#include <memory>
#include "InterStateSingleTon.h"
#include "MenuButton.h"
#include "VectorMath.h"
#include "XBoxButtons.h"
#include "MenuSystem.h"
#include "BigSprite.h"


class MenuState : public State
{
public:
	MenuState(void);
	~MenuState(void);

	/** 
	 * Loads MenuState content.
	 * Returns true if successful, otherwise returns false.
	 */
	bool Load();
	
	/** Main loop. */
	void Update(sf::Event events, bool eventFired, double deltaTime);

	/** Renders all of the applicable state content. */
	void Draw(sf::RenderWindow &renderWindow);

private:
	sf::Image backgroundImage;
	std::unique_ptr<BigSprite> backGroundSprite;

	/*
	void HandleButtonMouseSelection(sf::Event events, bool eventFired, double deltaTime);
	void HandleButtonControllerSelection();
	void HandleButtonActivation(sf::Event events, bool eventFired);
	void MoveToButton(MenuButton* button);

	//Need a timer, else we'll move through all the buttons all at once when we hit left of right
	sf::Clock canMoveButtonTimer;
	float timeTillCanMoveButton;

	

	std::vector<std::unique_ptr<MenuButton>> menuElements;
	MenuButton* selectedButton;
	
	//buttons that can be used to hit a menu button
	std::vector<sf::Keyboard::Key> selectionButtons;
	//The first int is the joy, 0 normally for player 1, while the second is the keycoke
	std::vector<std::pair<int,int>> joystickSelectionButtons;
	*/

	MenuSystem mainMenuSystem;

	void ToggleVolume();
	void GoToFirstLevelState();
	void QuitApplication();
	void GoToControlsState();
	
};

