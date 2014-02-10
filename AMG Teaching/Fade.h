#pragma once

#include "SFML\Graphics.hpp"
#include <iostream>
#include <sstream>
#include "VectorMath.h"
#include "Camera.h"

class Fade
{
public:
	Fade(sf::RenderWindow &window);
	~Fade(void);


	bool dofadeout(Camera Camera);
	bool dofadein(Camera Camera);
	bool Render(sf::RenderWindow &window);
	
private:
	sf::RenderWindow* window;

	//reset
	void reset();

	bool showfade;
	sf::Vector2u size;
	sf::RectangleShape fade;
	int minAlpha;
	int maxAlpha;
	int Alpha1;
	int Alpha2;

	sf::Clock DelayClock;
};


