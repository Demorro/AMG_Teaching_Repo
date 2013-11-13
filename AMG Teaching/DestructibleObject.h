#pragma once
#include "SFML\Graphics.hpp"
#include "SFML\Audio.hpp"


class DestructibleObject
{
public:

	//Being destroyed means the animations are playing etc, full destroyed is when the thing doesent have to update no more
	enum DestroyedState
	{
		Intact,
		BeingDestroyed,
		Destroyed
	};


	DestructibleObject(sf::Sprite intactSprite, sf::Sprite destroyedSprite, sf::Sound destructibleSound, bool startIntact = true);
	~DestructibleObject(void);
	
	//Renders the correct sprite, whether destroyed or not
	void Render(sf::RenderWindow &window);

	sf::Rect<float> GetCollisionRect();
	
	void SetDestructibleState(DestructibleObject::DestroyedState state);
	DestroyedState GetDestructibleState();

	//Runs the update logic, should only really be called when neccesary.
	void Update(double deltaTime);

	//Destroys the object, replacing the sprite and displaying any destroy animations
	void Destroy();

private:
	sf::Sprite intactSprite;
	sf::Sprite destroyedSprite;
	sf::Sound destructionSound;

	//Taken directly from the intactSprite, as it cant be loaded from the collision layer in the editor because it isn't static, i.e when this object is destroyed it dosen't have collision.
	sf::Rect<float> collisionRect;

	DestroyedState destructibleState;


};
