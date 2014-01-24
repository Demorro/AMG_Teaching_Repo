#include "AnimatedSprite.h"


AnimatedSprite::AnimatedSprite(sf::IntRect startRect)
{
	currentAnimation = NULL;
	repeating = false;
	currentFrame = 0;
	shouldPlay = false;
	setTextureRect(startRect);
	this->startRect = startRect;
}


AnimatedSprite::~AnimatedSprite(void)
{
}


bool AnimatedSprite::IsPlaying()
{
	return shouldPlay;
}

void AnimatedSprite::UpdateAnimations()
{
	if(shouldPlay)
	{
		if(currentAnimation != NULL)
		{
			if(animationTimer.getElapsedTime().asSeconds() > currentAnimation->frameTime)
			{
				animationTimer.restart();
				currentFrame++;

				//check to see if we need to loop back to the start of the anim
				if(currentFrame >= currentAnimation->animationFrames.size())
				{
					currentFrame = 0;

					if(!repeating)
					{
						shouldPlay = false;
						return;
					}
				}

				setTextureRect(currentAnimation->animationFrames[currentFrame]);
			}
		}
	}
}

void AnimatedSprite::Play()
{
	currentFrame = 0;
	shouldPlay = true;
}

void AnimatedSprite::Play(int startFrame)
{
	if(currentAnimation->animationFrames.size() >= startFrame)
	{
		currentFrame = startFrame;
	}
	else
	{
		std::cout << "Attempting to start animation from a frame that dosen't exist, starting from the start." << std::endl;
		currentFrame = 0;
	}
	shouldPlay = true;
}

void AnimatedSprite::Play(std::string animationName, int startFrame = 0)
{
	//this is true if the key exists
	if(animations.count(animationName))
	{
		currentAnimation = &animations[animationName];
		if(currentAnimation->animationFrames.size() <= startFrame)
		{
			currentFrame = startFrame;
		}
		else
		{
			std::cout << "Attempting to start animation from a frame that dosen't exist, starting from the start." << std::endl;
			currentFrame = 0;
		}
		shouldPlay = true;
	}
	else
	{
		std::cout << "Attempting to play animation that dosent exist" << std::endl;
	}
}

void AnimatedSprite::Pause()
{
	shouldPlay = false;
}

void AnimatedSprite::Resume()
{
	shouldPlay = true;
}

void AnimatedSprite::Stop()
{
	shouldPlay = false;
	currentFrame = 0;
}

void AnimatedSprite::StopAndReset()
{
	shouldPlay = false;
	currentFrame = 0;
	setTextureRect(startRect);

}

void AnimatedSprite::SetCurrentAnimation(std::string animationName)
{
	//this is true if the key exists
	if(animations.count(animationName))
	{
		currentAnimation = &animations[animationName];
	}
	else
	{
		std::cout << "Attempting to set animation that dosent exist" << std::endl;
	}
}

void AnimatedSprite::SetRepeating(bool _repeating)
{
	repeating = _repeating;
}

void AnimatedSprite::AddAnimation(std::string animationName, std::vector<sf::IntRect> frames, float frameTime)
{
	Animation animationToAdd(frames,frameTime);
	animations[animationName] = animationToAdd;

	if(currentAnimation == NULL)
	{
		currentAnimation = &animations[animationName];
		currentFrame = 0;
	}
}

void AnimatedSprite::changeAnimSpeed(std::string animationName, float frameTime)
{
	if(animations.count(animationName))
	{
		animations[animationName].frameTime = frameTime;
	}
	else
	{
		std::cout << "Attempting to set animation that dosent exist" << std::endl;
	}
}
