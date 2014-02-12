#include "NPC.h"


NPC::NPC(void)
{
	allowedAngleVariance = 1.5f;


}


NPC::~NPC(void)
{
}


void NPC::Update(double deltaTime)
{
	if(isWobbling)
	{
		if(wobbleTimer.getElapsedTime().asSeconds() < timeToWobble)
		{
			setRotation(wobbieIntensity * sin(wobbleSpeed * wobbleTimer.getElapsedTime().asSeconds()));
		}
		else
		{
			 //the angle variance stuff means that it has to come back to 0 angle(within variance) before stopping
			if((getRotation()>-allowedAngleVariance) && (getRotation() < allowedAngleVariance))
			{
				isWobbling = false;
			}
			else
			{
				setRotation(wobbieIntensity * sin(wobbleSpeed * wobbleTimer.getElapsedTime().asSeconds()));
			}
		}

	}
}

void NPC::DoExpressionWobble(float _timeToWobbleInSeconds, float _wobbleSpeed, float _wobbleIntensity)
{
	if(isWobbling) // already wobbling, so what's the point in starting again. There's such a thing as too much wobble you know.
	{
		return;
	}
	isWobbling = true;
	wobbleTimer.restart();

	timeToWobble = _timeToWobbleInSeconds;
	this->wobbleSpeed = _wobbleSpeed;
	this->wobbieIntensity = _wobbleIntensity;
}

bool NPC::IsWobbling()
{
	return isWobbling;
}