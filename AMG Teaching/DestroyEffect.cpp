#include "DestroyEffect.h"

DestroyEffect::DestroyEffect(sf::Sprite effectSprite)
{
	this->effectSprite = effectSprite;
	fadeColour = sf::Color::White;
	fadeColour.a = 0;
	fadeAccumulator = 0.0f;
	SetEffectState(FadingIn);
	effectSprite.setColor(fadeColour);
}

DestroyEffect::~DestroyEffect()
{
}

void DestroyEffect::Update(double deltaTime)
{
	//effectSprite.move(velocity.x * deltaTime, velocity.y * deltaTime);
	effectSprite.rotate(rotationRate * deltaTime);

	if(GetEffectState() == FadingIn)
	{
		if((int)fadeColour.a >= 254)
		{
			SetEffectState(FadingOut);
		}
		else
		{
			if(fadeAccumulator + (GetFadeInSpeed() * deltaTime) < 255)
			{
				fadeAccumulator += (GetFadeInSpeed() * deltaTime);
				fadeColour.a = fadeAccumulator;
				effectSprite.setColor(fadeColour);
			}
			else
			{
				fadeAccumulator = 255;
				fadeColour.a = fadeAccumulator;
				effectSprite.setColor(fadeColour);
			}

		}
	}
	else if(GetEffectState() == FadingOut)
	{
		if((int)fadeColour.a <= 1)
		{
			SetEffectState(Dead);
		}
		else
		{
			if(fadeAccumulator - (GetFadeInSpeed() * deltaTime) > 0)
			{
				fadeAccumulator -= (GetFadeInSpeed() * deltaTime);
				fadeColour.a = fadeAccumulator;
				effectSprite.setColor(fadeColour);
			}
			else
			{
				fadeAccumulator = 0;
				fadeColour.a = fadeAccumulator;
				effectSprite.setColor(fadeColour);
			}
		}

	}

}
void DestroyEffect::Render(sf::RenderWindow &window)
{
	window.draw(effectSprite);
}

sf::Vector2f DestroyEffect::GetVelocity()
{
	return velocity;
}
float DestroyEffect::GetRotationRate()
{
	return rotationRate;
}
float DestroyEffect::GetFadeOutSpeed()
{
	return fadeOutSpeed;
}
float DestroyEffect::GetFadeInSpeed()
{
	return fadeInSpeed;
}
DestroyEffect::EffectState DestroyEffect::GetEffectState()
{
	return effectState;
}

void DestroyEffect::SetVelocity(sf::Vector2f velocity)
{
	this->velocity = velocity;
}
void DestroyEffect::SetRotationRate(float rotationRate)
{
	this->rotationRate = rotationRate;
}
void DestroyEffect::SetFadeOutSpeed(float fadeSpeed)
{
	this->fadeOutSpeed = fadeSpeed;
}
void DestroyEffect::SetFadeInSpeed(float fadeSpeed)
{
	this->fadeInSpeed = fadeSpeed;
	
}
void DestroyEffect::SetEffectState(DestroyEffect::EffectState effectState)
{
	this->effectState = effectState;
	if(effectState == FadingIn)
	{
		fadeColour.a = 0;
		fadeAccumulator = 0.0f;
	}
}

void DestroyEffect::Move(float x, float y)
{
	effectSprite.move(x,y);
}