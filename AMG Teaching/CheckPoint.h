#pragma once
#include "AnimatedSprite.h"
#include "Assets.h"

class CheckPoint : public AnimatedSprite
{
public:
	CheckPoint(sf::IntRect startRect);
	~CheckPoint(void);

	bool IsActiveCheckPoint();

	void Activate();
	void Disable();

private:
	void SetIsTheActiveCheckpoint(bool active);
	bool isTheActiveCheckpoint;
};

