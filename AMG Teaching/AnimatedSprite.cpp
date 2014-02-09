#include "AnimatedSprite.h"


AnimatedSprite::AnimatedSprite(sf::IntRect startRect)
{
	currentAnimation = Animation();
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
		//if(currentAnimation != nullptr)
		//{
			if(animationTimer.getElapsedTime().asSeconds() > currentAnimation.frameTime)
			{
				animationTimer.restart();
				currentFrame++;

				//check to see if we need to loop back to the start of the anim
				if(currentFrame >= currentAnimation.animationFrames.size())
				{
					currentFrame = 0;

					if(!repeating)
					{
						shouldPlay = false;
						return;
					}
				}
				if(currentFrame <= currentAnimation.animationFrames.size())
				{
					setTextureRect(currentAnimation.animationFrames[currentFrame]);
				}
			}
		//}
	}
}

void AnimatedSprite::Play()
{
	currentFrame = 0;
	shouldPlay = true;
}

void AnimatedSprite::Play(int startFrame)
{
	if(currentAnimation.animationFrames.size() >= startFrame)
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
		//currentAnimation = &animations[animationName];
		currentAnimation = animations[animationName];
		if(currentAnimation.animationFrames.size() <= startFrame)
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
	if(animations.find(animationName) == animations.end())
	{
		std::cout << "Attempting to set animation that dosent exist" << std::endl;
	}
	else
	{
		//currentAnimation = &animations[animationName];
		currentAnimation = animations[animationName];
	}
}

void AnimatedSprite::SetRepeating(bool _repeating)
{
	repeating = _repeating;
}

void AnimatedSprite::AddAnimation(std::string animationName, std::vector<sf::IntRect> frames, float frameTime)
{
	Animation animationToAdd(frames,frameTime,animationName);
	animations[animationName] = animationToAdd;

	//if(currentAnimation == nullptr)
	//{
		//currentAnimation = &animations[animationName];
		currentAnimation = animations[animationName];
		currentFrame = 0;
	//}
}

void AnimatedSprite::ChangeAnimSpeed(std::string animationName, float frameTime)
{
	if(animations.count(animationName))
	{
		animations[animationName].frameTime = frameTime;

		//since the currentanimation dosent use a pointer anymore, need to change the current animation copy speed too
		if(currentAnimation.animationName == animationName)
		{
			currentAnimation.frameTime = frameTime;
		}
	}
	else
	{
		std::cout << "Attempting to set animation that dosent exist" << std::endl;
	}
}

void AnimatedSprite::LoadSingleAnimation(sf::Vector2i startFrame, int xFrameOffset, int noOfFrames, int frameWidth, int frameHeight, float frameTime, std::string animationName)
{
	std::vector<sf::IntRect> anim = std::vector<sf::IntRect>();

	sf::Rect<int> frameSize;
	frameSize.width = frameWidth;
	frameSize.height = frameHeight;
	for(int i = 0; i < noOfFrames; i++)
	{
		anim.push_back(sf::IntRect((i * xFrameOffset) + startFrame.x,0 + startFrame.y,frameSize.width,frameSize.height));
	}

	AddAnimation(animationName,anim,frameTime);
}
