#include "MotionSpline.h"


MotionSpline::MotionSpline()
{
	ReInitialiseSpline(sf::Vector2f(0,0),sf::Vector2f(0,0),sf::Vector2f(0,0),sf::Vector2f(0,0));
}

MotionSpline::MotionSpline(sf::Vector2f point1, sf::Vector2f point2, sf::Vector2f point3, sf::Vector2f point4)
{
	pt1.x = (int)point1.x;
	pt1.y = (int)point1.y;
	pt2.x = (int)point2.x;
	pt2.y = (int)point2.y;
	pt3.x = (int)point3.x;
	pt3.y = (int)point3.y;
	pt4.x = (int)point4.x;
	pt4.y = (int)point4.y;

	
	float RecursiveMathA=3*pt3.x;
	float RecursiveMathB=3*pt2.x;
	float RecursiveMathC=6*pt2.x;
	float RecursiveMathD=3*pt1.x;
	float RecursiveMathE=3*pt3.y;
	float RecursiveMathF=3*pt2.y;
	float RecursiveMathG=6*pt2.y;
	float RecursiveMathH=3*pt1.y;


	A = pt4.x - RecursiveMathA + RecursiveMathB - pt1.x;
	B = RecursiveMathA - RecursiveMathC + RecursiveMathD;
	C = RecursiveMathB - RecursiveMathD;
	D = pt1.x;
	E = pt4.y - RecursiveMathE +RecursiveMathF - pt1.y;
	F = RecursiveMathE - RecursiveMathG + RecursiveMathH;
	G = RecursiveMathF - RecursiveMathH;
	H = pt1.y;	
}


MotionSpline::~MotionSpline(void)
{
}

void MotionSpline::ReInitialiseSpline(sf::Vector2f point1, sf::Vector2f point2, sf::Vector2f point3, sf::Vector2f point4)
{
	pt1.x = (int)point1.x;
	pt1.y = (int)point1.y;
	pt2.x = (int)point2.x;
	pt2.y = (int)point2.y;
	pt3.x = (int)point3.x;
	pt3.y = (int)point3.y;
	pt4.x = (int)point4.x;
	pt4.y = (int)point4.y;


	float RecursiveMathA=3*pt3.x;
	float RecursiveMathB=3*pt2.x;
	float RecursiveMathC=6*pt2.x;
	float RecursiveMathD=3*pt1.x;
	float RecursiveMathE=3*pt3.y;
	float RecursiveMathF=3*pt2.y;
	float RecursiveMathG=6*pt2.y;
	float RecursiveMathH=3*pt1.y;


	A = pt4.x - RecursiveMathA + RecursiveMathB - pt1.x;
	B = RecursiveMathA - RecursiveMathC + RecursiveMathD;
	C = RecursiveMathB - RecursiveMathD;
	D = pt1.x;
	E = pt4.y - RecursiveMathE +RecursiveMathF - pt1.y;
	F = RecursiveMathE - RecursiveMathG + RecursiveMathH;
	G = RecursiveMathF - RecursiveMathH;
	H = pt1.y;	
}





sf::Vector2f MotionSpline::GetPointOnSpline(float t)
{

	float x = A * ToPowerOf(t,3) + B * ToPowerOf(t,2) + C * t + D;
	float y = E * ToPowerOf(t,3) + F * ToPowerOf(t,2) + G * t + H;

	return sf::Vector2f(x,y);


}

//From when i was trying to do proper arc-paramitisation. Screw that, just faking it now with approximated line segments. No need to delete a good getter function though.
void MotionSpline::GetParametricVariables(float &A, float &B, float &C, float &D, float &E, float &F, float &G, float &H)
{
	A = this->A;
	B = this->B;
	C = this->C;
	D = this->D;
	E = this->E;
	F = this->F;
	G = this->G;
	H = this->H;
}

std::vector<std::pair<sf::Vector2f,sf::Vector2f>> MotionSpline::GetEqualLengthLineSegments(int noOfSegments)
{
	std::vector<std::pair<sf::Vector2f,sf::Vector2f>> lineSegments;


	//get full arc length at specified detail
	float fullArcLength = 0;
	float timeIterator = 0;
	float segmentIterationDistance = float((float)1/noOfSegments);

	sf::Vector2f lastSegmentPoint = GetPointOnSpline(0);
	while(timeIterator <= 1)
	{
		sf::Vector2f newSegmentPoint = GetPointOnSpline(timeIterator);
		fullArcLength += VectorDistance(lastSegmentPoint, newSegmentPoint);
		lastSegmentPoint = newSegmentPoint;
		timeIterator += segmentIterationDistance;
	}

	//divide the full arc length up into equal length chunks
	float segmentLength = fullArcLength/noOfSegments;
	float lastTValue = 0.0f;
	sf::Vector2f lastSegmentStart = GetPointOnSpline(lastTValue);
	float fidelity = 0.01f; //fidelity is how much t value we increment to check distance, the smaller the better, but beware of a performance hit. For exacmple, 0.01 will make the loop go round 100 times
	while(lastTValue < 1.0f)
	{
		lastTValue += fidelity;
		sf::Vector2f candidatePoint = GetPointOnSpline(lastTValue);
		//then this segment is the right length
		if(VectorDistance(lastSegmentStart,candidatePoint) >= segmentLength)
		{
			lineSegments.push_back(std::pair<sf::Vector2f, sf::Vector2f>(lastSegmentStart,candidatePoint));
			lastSegmentStart = candidatePoint;
		}
	}

	//Add the final segment
	lineSegments.push_back(std::pair<sf::Vector2f, sf::Vector2f>(lastSegmentStart,GetPointOnSpline(1.0f)));


	return lineSegments;

}

//HEAVILY INNEFICIENT. Only for debugging.
void MotionSpline::Render(sf::RenderWindow &window)
{
	std::vector<std::pair<sf::Vector2f,sf::Vector2f>> currentSplineLineSegments = GetEqualLengthLineSegments(8);
	if(currentSplineLineSegments.size() > 0)
	{
		for(int i = 0; i < (currentSplineLineSegments.size() - 1); i++)
		{
			sf::Vertex line[] =
			{
				sf::Vertex(sf::Vector2f(currentSplineLineSegments[i].first.x, currentSplineLineSegments[i].first.y)),
				sf::Vertex(sf::Vector2f(currentSplineLineSegments[i].second.x, currentSplineLineSegments[i].second.y))
			};

			window.draw(line, 2, sf::Lines);
		}
	}


	sf::CircleShape splineRepresentationShape;
	splineRepresentationShape.setFillColor(sf::Color::Transparent);
	splineRepresentationShape.setOutlineThickness(1.0f);
	splineRepresentationShape.setOutlineColor(sf::Color::Green);
	splineRepresentationShape.setRadius(10.0f);
	splineRepresentationShape.setOrigin(10,10);


	splineRepresentationShape.setOutlineThickness(3.0f);
	splineRepresentationShape.setOutlineColor(sf::Color::White);
	splineRepresentationShape.setPosition(pt1);
	window.draw(splineRepresentationShape);

	splineRepresentationShape.setOutlineColor(sf::Color::Green);
	splineRepresentationShape.setPosition(pt2);
	window.draw(splineRepresentationShape);

	splineRepresentationShape.setOutlineColor(sf::Color::Blue);
	splineRepresentationShape.setPosition(pt3);
	window.draw(splineRepresentationShape);

	splineRepresentationShape.setOutlineColor(sf::Color::Red);
	splineRepresentationShape.setPosition(pt4);
	window.draw(splineRepresentationShape);



}

