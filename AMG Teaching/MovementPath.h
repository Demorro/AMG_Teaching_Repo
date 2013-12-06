#pragma once
#include "SFML\Graphics.hpp"
#include <iostream>
//This path is loaded in from the leveleditor, used mainly in making a track for objects to follow.

class MovementPath
{
public:
	MovementPath();
	MovementPath(std::vector<sf::Vector2f> pathPoints, bool isLooping);
	~MovementPath(void);

	std::vector<sf::Vector2f> GetPathPoints();
	void SetPathPoints(std::vector<sf::Vector2f> pathPoints);

	void SetPointAtIndex(int index, sf::Vector2f newPoint);
	sf::Vector2f GetPointAtIndex(int index);

	void SetIsLooping(bool _looping);
	bool IsALoopingPath();

private:
	std::vector<sf::Vector2f> pathPoints;
	bool isLooping;

};

