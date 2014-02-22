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

	MenuSystem mainMenuSystem;

	void ToggleVolume();
	void GoToFirstLevelState();
	void QuitApplication();
	void GoToControlsState();
	
};

