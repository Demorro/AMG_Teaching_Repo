#pragma once
#include "State.h"
#include "Assets.h"
#include <memory>
#include <functional>
#include "MenuButton.h"

class ControlState : public State
{
public:
	ControlState(void);
	~ControlState(void);

	/** 
	 * Loads ControlState content.
	 * Returns true if successful, otherwise returns false.
	 */
	bool Load();
	
	/** Main loop. */
	void Update(sf::Event events, bool eventFired, double deltaTime);

	/** Renders all of the applicable state content. */
	void Draw(sf::RenderWindow &renderWindow);

private:
	sf::Texture controllerTexture;
	sf::Sprite controllerSprite;

	std::vector<std::unique_ptr<MenuButton>> menuElements;
	MenuButton* selectedButton;

	//buttons that can be used to hit a menu button
	std::vector<sf::Keyboard::Key> selectionButtons;

	void GoBackToMenuState();
};

