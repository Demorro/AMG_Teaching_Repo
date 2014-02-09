#include "CheckPoint.h"


CheckPoint::CheckPoint(sf::IntRect startRect) : AnimatedSprite(startRect)
{
	isTheActiveCheckpoint = false;

	//Load the flag unfurl animation
	std::string unfurlAnimationName = "CheckpointActivate";
	LoadSingleAnimation(sf::Vector2i(0,0),120,5,120,260,0.03f,unfurlAnimationName);
	//SetCurrentAnimation(unfurlAnimationName);
}


CheckPoint::~CheckPoint(void)
{
}

void CheckPoint::Activate()
{
	SetIsTheActiveCheckpoint(true);
	Play();
}

void CheckPoint::Disable()
{
	SetIsTheActiveCheckpoint(false);
	StopAndReset();
}

bool CheckPoint::IsActiveCheckPoint()
{
	return isTheActiveCheckpoint;
}

void CheckPoint::SetIsTheActiveCheckpoint(bool active)
{
	isTheActiveCheckpoint = active;
}