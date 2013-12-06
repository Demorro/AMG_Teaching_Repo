#pragma once
#include "SFML\Graphics.hpp"

//Struct used in returning the moving objects
struct MovingCollider
{
	sf::Rect<float> collisionBound;
	sf::Vector2f velocity;
	sf::Vector2f centerPos;
	sf::Vector2f lastCenterPos;

	MovingCollider::MovingCollider()
	{
		velocity = sf::Vector2f(0,0);
		centerPos = sf::Vector2f(0,0);
		lastCenterPos = sf::Vector2f(0,0);
	}
};