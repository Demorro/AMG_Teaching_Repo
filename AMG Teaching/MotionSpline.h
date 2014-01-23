#pragma once
#include "SFML\Graphics.hpp"
#include "VectorMath.h"
#include <iostream>

//Parametric equations for the spline, t is time, iterate from 0 to 1
//x = At3 + Bt2 + Ct + D
//y = Et3 + Ft3 + Gt + H

/*Formulating the variables for the equation
A = x3 – 3x2 +3x1 – x0
B = 3x2 – 6x1 + 3x0
C = 3x1 – 3x0
D = x0

E = y3 – 3y2 +3y1 – y0
F = 3y2 – 6y1 + 3y0
G = 3y1 – 3y0
H = y0
*/

class MotionSpline
{
public:
	MotionSpline();
	//SimulationTimeOffset is for pt 2 and 3, where you have to speed up/rewind time respectively to obtain them.
	//There might be a problem using deltaTime as sort of a "one off" measuremnt in the rewind/speed up, but i think it'll be fine.
	MotionSpline(sf::Vector2f point1, sf::Vector2f point2, sf::Vector2f point3, sf::Vector2f point4);
	~MotionSpline(void);

	void ReInitialiseSpline(sf::Vector2f point1, sf::Vector2f point2, sf::Vector2f point3, sf::Vector2f point4);
	sf::Vector2f GetPointOnSpline(float t);
	void Render(sf::RenderWindow &window);

	void GetParametricVariables(float &A, float &B, float &C, float &D, float &E, float &F, float &G, float &H);

	std::vector<std::pair<sf::Vector2f,sf::Vector2f>> GetEqualLengthLineSegments(int noOfSegments);

private:
	sf::Vector2f pt1;
	sf::Vector2f pt2;
	sf::Vector2f pt3;
	sf::Vector2f pt4;

	//variables for the paremetric equations
	float A;
	float B;
	float C;
	float D;
	float E;
	float F;
	float G;
	float H;

};

