#pragma once
#include "State.h"
#include "Assets.h"
#include <memory>
#include <functional>
#include "MenuButton.h"
#include "MenuSystem.h"

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

	MenuSystem controlScreenMenuSystem;

	void GoBackToMenuState();
};

