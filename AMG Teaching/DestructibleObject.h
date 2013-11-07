#pragma once
#include "SFML\Graphics.hpp"

class DestructibleObject
{
public:
	DestructibleObject(sf::Sprite intactSprite, sf::Sprite destroyedSprite, bool startIntact = true);
	~DestructibleObject(void);
	
	//Renders the correct sprite, whether destroyed or not
	void Render(sf::RenderWindow &window);

	sf::Rect<float> GetCollisionRect();
	bool IsIntact();

	//Destroys the object, replacing the sprite and displaying any destroy animations
	void Destroy();

private:
	sf::Sprite intactSprite;
	sf::Sprite destroyedSprite;

	//Taken directly from the intactSprite, as it cant be loaded from the collision layer in the editor because it isn't static, i.e when this object is destroyed it dosen't have collision.
	sf::Rect<float> collisionRect;
	
	//Keeps track of whether the object is destroyed or not, so we can render the correct sprite/do/do-not do collision
	bool isIntact;
};

