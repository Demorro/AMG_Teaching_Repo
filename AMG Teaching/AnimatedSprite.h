#pragma once
#include "SFML\Graphics.hpp"
#include <iostream>

class AnimatedSprite : public sf::Sprite
{
public:
	AnimatedSprite(sf::IntRect startRect);
	~AnimatedSprite(void);

	void UpdateAnimations();
	void Play();
	void Play(std::string animation);

	void Pause();
	void Stop();
	void StopAndReset();
	void Resume();

	bool IsPlaying();

	void SetCurrentAnimation(std::string animation);
	void SetRepeating(bool _repeating);

	void AddAnimation(std::string animationName, std::vector<sf::IntRect> frames, float frameTime);

private:

	sf::Clock animationTimer;
	bool repeating;
	bool shouldPlay;
	int currentFrame;
	sf::IntRect startRect;
	
	//Stores information about individual animations
	struct Animation
	{
		std::vector<sf::IntRect> animationFrames;
		float frameTime;

		Animation::Animation()
		{
			frameTime = 0.2f;
		}

		Animation::Animation(std::vector<sf::IntRect> animationFrames, float frameTime = 0.2f)
		{
			this->animationFrames = animationFrames;
			this->frameTime = frameTime;
		}
	};

	Animation* currentAnimation;
	std::map<std::string, Animation> animations;

};

