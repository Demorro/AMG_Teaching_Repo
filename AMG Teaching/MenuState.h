#pragma once
#include "state.h"
#include "Assets.h"
#include "MotionSpline.h"
#include <memory>
#include "InterStateSingleTon.h"
#include "MenuButton.h"


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

	sf::Texture backgroundImage;
	sf::Sprite backGroundSprite;

	std::vector<std::unique_ptr<MenuButton>> menuElements;
	MenuButton* selectedButton;
	
	//buttons that can be used to hit a menu button
	std::vector<sf::Keyboard::Key> selectionButtons;

	void ToggleVolume();
	void GoToFirstLevelState();
	void GoToControlsState();

};

