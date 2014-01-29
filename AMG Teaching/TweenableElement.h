#pragma once

#include<SFML\Graphics.hpp>
#include <memory>
#include "MotionSpline.h"

//Abstract base class for a tweenable element on the screen. Used heavily in making the menus work

class TweenableElement
{
public:
	enum TweenInDirection
	{
		Top,
		Bottom,
		Left,
		Right
	};

	TweenableElement(bool shouldTweenIn, sf::Vector2f restPosition);
	virtual ~TweenableElement(void);
	virtual void Update(sf::Event events, bool eventFired, double deltaTime) = 0; 
	virtual void Render(sf::RenderWindow& window) = 0;

	void ResetTween();
	void GenerateTween(sf::Vector2f restPosition, sf::Rect<float> objectBounds, TweenInDirection tweenDirection, float tweenBobAmount, float tweenSpeed);

protected:
	void DoTweenLogic(double deltaTime, sf::Transformable &object);
	bool shouldTweenIn;

private:
	float tweenInSpeed;
	float tweenProgressIterator; //Iterate from 0 to 1 for the tween
	std::unique_ptr<MotionSpline> tweenInSpline;
	TweenInDirection tweenInDirection; 
};

