#include "Fade.h"


Fade::Fade(sf::RenderWindow &window)
{

	this->window = &window;
	size = this->window->getSize();
	
		maxAlpha = 255;
		minAlpha = 0;
		Alpha1 = 255;
		Alpha2 = 0;
		
		

		showfade = true;
}


Fade::~Fade(void)
{
}

void Fade::reset()
{
	maxAlpha = 255;
	minAlpha = 0;
	Alpha1 = 255;
	Alpha2 = 0;
}

bool Fade::dofadein(Camera Camera)
{
   

	if(Alpha1 >= minAlpha)
	{
		fade = sf::RectangleShape(sf::Vector2f(size.x,size.y));
		fade.setPosition(Camera.Center_Point.x - (size.x/2), Camera.Center_Point.y - (size.y/2));
		fade.setFillColor(sf::Color(0,0,0,Alpha1));
		if(DelayClock.getElapsedTime().asMilliseconds() > 50)
		{
        Alpha1 -= 5;
		DelayClock.restart();
		}
	}
	
	if(Alpha1 != minAlpha)
	{
		return false;
	}
	else
	{
		showfade = false;
		reset();
		return true;
	}
}


bool Fade::dofadeout(Camera Camera)
{
	if(Alpha2 <= maxAlpha)
	{
		fade = sf::RectangleShape(sf::Vector2f(size.x,size.y));
		fade.setPosition(Camera.Center_Point.x - (size.x/2), Camera.Center_Point.y - (size.y/2));
		fade.setFillColor(sf::Color(0,0,0,Alpha2));
		if(DelayClock.getElapsedTime().asMilliseconds() > 50)
		{
        Alpha2 += 5;
		DelayClock.restart();
		}
		showfade = true;
	}
	
	if(Alpha2 != maxAlpha)
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool Fade::Render(sf::RenderWindow &window)
{
	if(showfade)
	{
	window.draw(fade);
	}
	return true;
}