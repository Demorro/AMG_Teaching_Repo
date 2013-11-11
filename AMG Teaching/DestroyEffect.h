#pragma once
#include "SFML\Graphics.hpp"
#include <iostream>

//TODO, make this extend rather than include sprite, couldnt do it right now as I had no internet and coudlnt google an error
class DestroyEffect
{
public:

	enum EffectState
	{
		FadingIn,
		Active,
		FadingOut,
		Dead
	};

	EffectState effectState;

	DestroyEffect::DestroyEffect(sf::Sprite effectSprite);
	DestroyEffect::~DestroyEffect();

	sf::Vector2f GetVelocity();
	float GetRotationRate();
	float GetFadeInSpeed();
	float GetFadeOutSpeed();
	EffectState GetEffectState();

	void SetVelocity(sf::Vector2f velocity);
	void SetRotationRate(float rotationRate);
	void SetFadeOutSpeed(float fadeSpeed);
	void SetFadeInSpeed(float fadeSpeed);
	void SetEffectState(EffectState effectState);

	void Update(double deltaTime);
	void Render(sf::RenderWindow &window);

	void Move(float x, float y);

private:
	
	sf::Sprite effectSprite;

	sf::Vector2f velocity;
	float rotationRate;
	//Used for fading out
	sf::Color fadeColour;
	float fadeOutSpeed;
	float fadeInSpeed;
	//Because alpha is a uint8 for some reason, so you need to store this if you want to vary the fades in anything other than an int
	float fadeAccumulator;

};
