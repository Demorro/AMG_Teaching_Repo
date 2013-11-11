#include "DestructibleObject.h"


DestructibleObject::DestructibleObject(sf::Sprite intactSprite, sf::Sprite destroyedSprite, bool startIntact)
{
	this->intactSprite = intactSprite;
	this->destroyedSprite = destroyedSprite;

	if(startIntact)
	{
		SetDestructibleState(Intact);
	}
	else
	{
		SetDestructibleState(Destroyed);
	}

	collisionRect = intactSprite.getGlobalBounds();
	
}


DestructibleObject::~DestructibleObject(void)
{
}

void DestructibleObject::Update(double deltaTime)
{
	if(GetDestructibleState() == BeingDestroyed)
	{
		SetDestructibleState(Destroyed);
	}
}

void DestructibleObject::Destroy()
{
	//only bother destroying if it isnt already destroyed
	if(GetDestructibleState() == Intact)
	{
		SetDestructibleState(BeingDestroyed);
	}
}


void DestructibleObject::SetDestructibleState(DestructibleObject::DestroyedState state)
{
	destructibleState = state;
}

DestructibleObject::DestroyedState DestructibleObject::GetDestructibleState()
{
	return destructibleState;
}

sf::Rect<float> DestructibleObject::GetCollisionRect()
{
	return collisionRect;
}

void DestructibleObject::Render(sf::RenderWindow &window)
{
	if(GetDestructibleState() == Intact)
	{
		window.draw(intactSprite);
	}
	else
	{
		window.draw(destroyedSprite);
	}
}

