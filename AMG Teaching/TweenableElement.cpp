#include "TweenableElement.h"
#include "Application.h"


TweenableElement::TweenableElement(bool shouldTweenIn, sf::Vector2f restPosition)
{
	this->shouldTweenIn = shouldTweenIn;

	tweenInSpline = std::unique_ptr<MotionSpline>(new MotionSpline(restPosition,restPosition,restPosition,restPosition));

}

TweenableElement::~TweenableElement(void)
{
}

void TweenableElement::GenerateTween(sf::Vector2f restPosition, sf::Rect<float> objectBounds, TweenInDirection tweenDirection, float tweenBobAmount, float tweenSpeed)
{
	tweenInSpeed = tweenSpeed;

	sf::Vector2f tweenStartPosition; //This position is just off the screen in the tween direction, so you cant see the button
	sf::Vector2f tweenBobPosition; //The final position + the bob offset
	sf::Vector2f endTweenPosition = restPosition;
	if(tweenDirection == Top)
	{
		tweenStartPosition = restPosition;
		tweenStartPosition.y = 0 - objectBounds.height;
		tweenBobPosition = restPosition;
		tweenBobPosition.y = tweenBobPosition.y + tweenBobAmount;
	}
	else if(tweenDirection == Bottom)
	{
		tweenStartPosition = restPosition;
		tweenStartPosition.y = Application::GetWindow().getSize().y + objectBounds.height;
		tweenBobPosition = restPosition;
		tweenBobPosition.y = tweenBobPosition.y - tweenBobAmount;
	}
	else if(tweenDirection == Left)
	{
		tweenStartPosition = restPosition;
		tweenStartPosition.x = 0 - objectBounds.width;
		tweenBobPosition = restPosition;
		tweenBobPosition.x = tweenBobPosition.x + tweenBobAmount;
	}
	else if(tweenDirection == Right)
	{
		tweenStartPosition = restPosition;
		tweenStartPosition.x = Application::GetWindow().getSize().x + objectBounds.width;
		tweenBobPosition = restPosition;
		tweenBobPosition.x = tweenBobPosition.x - tweenBobAmount;
	}
	tweenInSpline = std::unique_ptr<MotionSpline>(new MotionSpline(tweenStartPosition,tweenBobPosition,endTweenPosition,endTweenPosition));

	ResetTween();
}

void TweenableElement::ResetTween()
{
	tweenProgressIterator = 0.0f;
}

void TweenableElement::DoTweenLogic(double deltaTime, sf::Transformable &object)
{
	if(shouldTweenIn)
	{
		//OutputDebugString(L"w");
		if(tweenInSpline != nullptr)
		{
			object.setPosition(tweenInSpline->GetPointOnSpline(tweenProgressIterator));
			tweenProgressIterator += deltaTime * tweenInSpeed;
			if(tweenProgressIterator > 1.0f)
			{
				tweenProgressIterator = 1.0f;
			}
		}
		else
		{
				
			std::cout << "Cannot tween object, no spline\n" << std::endl;
		}
	}
}