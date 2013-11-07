#include "DestructibleObject.h"


DestructibleObject::DestructibleObject(sf::Sprite intactSprite, sf::Sprite destroyedSprite, bool startIntact)
{
	this->intactSprite = intactSprite;
	this->destroyedSprite = destroyedSprite;

	isIntact = startIntact;

	collisionRect = intactSprite.getGlobalBounds();
}


DestructibleObject::~DestructibleObject(void)
{
}

void DestructibleObject::Destroy()
{
	if(IsIntact())
	{
		isIntact = false;
	}
}

bool DestructibleObject::IsIntact()
{
	return isIntact;
}

sf::Rect<float> DestructibleObject::GetCollisionRect()
{
	return collisionRect;
}

void DestructibleObject::Render(sf::RenderWindow &window)
{
	if(IsIntact())
	{
		window.draw(intactSprite);
	}
	else
	{
		window.draw(destroyedSprite);
	}
}