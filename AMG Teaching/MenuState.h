#pragma once
#include "state.h"
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
};

