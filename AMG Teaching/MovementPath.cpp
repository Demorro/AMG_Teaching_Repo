#include "MovementPath.h"


MovementPath::MovementPath(void)
{
	isLooping = false;
}

MovementPath::MovementPath(std::vector<sf::Vector2f> pathPoints, bool isLooping)
{
	this->pathPoints = pathPoints;
	this->isLooping = isLooping;
}

MovementPath::~MovementPath(void)
{

}

std::vector<sf::Vector2f> MovementPath::GetPathPoints()
{
	return pathPoints;
}

void MovementPath::SetPathPoints(std::vector<sf::Vector2f> pathPoints)
{
	this->pathPoints = pathPoints;
}

sf::Vector2f MovementPath::GetPointAtIndex(int index)
{
	if(pathPoints.size() > index)
	{
		return pathPoints[index];
	}
	else
	{
		return pathPoints[0];
		std::cout << "Cannot get movement path point at index : " << index << " as it dosen't seem to exist" << std::endl;
	}
}

void MovementPath::SetPointAtIndex(int index, sf::Vector2f newPoint)
{
	if(pathPoints.size() > index)
	{
		pathPoints[index] = newPoint;
	}
	else
	{
		std::cout << "Cannot set movement path point at index : " << index << " as it dosen't seem to exist" << std::endl;
	}
}

void MovementPath::SetIsLooping(bool _looping)
{
	isLooping = _looping;
}

bool MovementPath::IsALoopingPath()
{
	return isLooping;
}