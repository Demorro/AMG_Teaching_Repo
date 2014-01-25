#pragma once

#include <math.h>
#include "SFML\System.hpp"

/*
	Since sfml has no vector math facilities, thought it would be convenient to have a place to put some functions
*/

#define PI 3.14159265359

inline float VectorDistance(sf::Vector2i p1, sf::Vector2i p2)
{
	return sqrt((float)((p2.x - p1.x) * (float)(p2.x - p1.x)) + (float)((p2.y - p1.y) * (float)(p2.y - p1.y)));
}

inline float VectorDistance(sf::Vector2f p1, sf::Vector2f p2)
{
	return sqrt(((p2.x - p1.x) * (p2.x - p1.x)) + ((p2.y - p1.y) * (p2.y - p1.y)));
}

inline float VectorDotProduct(sf::Vector2i p1, sf::Vector2i p2)
{
	return ((float)(p1.x * p2.x) + (float)(p1.y * p2.y));
}

inline float VectorDotProduct(sf::Vector2f p1, sf::Vector2f p2)
{
	return ((p1.x * p2.x) + (p1.y * p2.y));
}

inline float VectorMagnitude(sf::Vector2f vector)
{
	return sqrt((vector.x * vector.x) + (vector.y * vector.y));
}

inline void NormaliseVector(sf::Vector2f &vector)
{
	vector /= VectorMagnitude(vector);
}

inline float RadToDeg(float radian)
{
	return radian * (float(180) / float(PI));
}

inline float DegToRad(float degree)
{
	return degree * (float(PI) / float(180));
}

//This Code Segment taken from here : (www.github.com/kg/PlatformerStarterKit/blob/master/RectangleExtensions.cs)
inline sf::Vector2f GetIntersectionDepth (sf::Rect<float> rectA, sf::Rect<float> rectB)
{
    // Calculate half sizes.
	float halfWidthA = rectA.width / 2.0f;
    float halfHeightA = rectA.height / 2.0f;
    float halfWidthB = rectB.width / 2.0f;
    float halfHeightB = rectB.height / 2.0f;

    // Calculate centers.
	sf::Vector2f centerA = sf::Vector2f(rectA.left + halfWidthA, rectA.top + halfHeightA);
    sf::Vector2f centerB = sf::Vector2f(rectB.left + halfWidthB, rectB.top + halfHeightB);

    // Calculate current and minimum-non-intersecting distances between centers.
	float distanceX = centerA.x - centerB.x;
    float distanceY = centerA.y - centerB.y;
    float minDistanceX = halfWidthA + halfWidthB;
    float minDistanceY = halfHeightA + halfHeightB;

    // If we are not intersecting at all, return (0, 0).
    if (abs(distanceX) >= minDistanceX || abs(distanceY) >= minDistanceY)
		return sf::Vector2f(0,0);

    // Calculate and return intersection depths.
    float depthX = distanceX > 0 ? minDistanceX - distanceX : -minDistanceX - distanceX;
    float depthY = distanceY > 0 ? minDistanceY - distanceY : -minDistanceY - distanceY;
	return sf::Vector2f(depthX, depthY);
}

inline float ToPowerOf(float input, float toPowerOf)
{
	float accumulator = input;
	for(int i = 0; i < (toPowerOf - 1); i++)
	{
		accumulator =  accumulator * input;
	}

	return accumulator;
}

//Clip n between lower and upper, for example, (n = 3,4,6) will come out as 4, while (n = 7,4,6) will be 6, while (n = 5,4,6) will remain 5
inline float clip(float &n, float lower, float upper) {
  return std::max(lower, std::min(n, upper));
}
