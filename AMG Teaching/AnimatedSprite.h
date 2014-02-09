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
	void Play(int startFrame);
	void Play(std::string animation, int startFrame);

	void Pause();
	void Stop();
	void StopAndReset();
	void Resume();

	bool IsPlaying();

	void SetCurrentAnimation(std::string animation);
	void SetRepeating(bool _repeating);

	void AddAnimation(std::string animationName, std::vector<sf::IntRect> frames, float frameTime);

	//An easier way of loading an animation strip into an animation without passing a vector of intrects.
	void LoadSingleAnimation(sf::Vector2i startFrame, int xFrameOffset, int noOfFrames, int frameWidth, int frameHeight, float frameTime, std::string animationName);

	void ChangeAnimSpeed(std::string, float);

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
		std::string animationName;

		Animation::Animation()
		{
			frameTime = 0.2f;
			animationFrames = std::vector<sf::IntRect>();
			animationName = "DefaultAnimation";
		}

		Animation::Animation(std::vector<sf::IntRect> _animationFrames, float _frameTime = 0.2f, std::string _animationName = "DefaultAnimation")
		{
			this->animationFrames = _animationFrames;
			this->frameTime = _frameTime;
			this->animationName = _animationName;
		}
	};

	Animation currentAnimation;
	std::map<std::string, Animation> animations;



};

