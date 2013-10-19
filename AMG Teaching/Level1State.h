#pragma once
#include "state.h"
#include "TextureManager.h"

class Level1State : public State
{
public:
	Level1State(void);
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

};