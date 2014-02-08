#include "SpecialPlatform.h"


SpecialPlatform::SpecialPlatform(sf::Sprite &platformSprite, float fallDelay, float fallGravity, float fallMaxSpeed, float movementSpeed, float distanceTillGoToNextPathNode, std::vector<sf::Rect<float>>* killPlatformZones,  sf::SoundBuffer &fallingSound) : sf::Sprite(platformSprite)
{
	fallSound.setBuffer(fallingSound);

	hasBeenStoodOn = false;
	shouldBeFalling = false;
	shouldDestroyThisObject = false;

	this->fallDelay = fallDelay;
	this->fallGravity = fallGravity;
	this->fallMaxSpeed = fallMaxSpeed;
	this->killPlatformZones = killPlatformZones;

	this->movementSpeed = movementSpeed;
	this->distanceTillGoToNextPathNode = distanceTillGoToNextPathNode;
	currentMovePathNode = 0;
	isGoingBackDownPath = false;

	velocity = sf::Vector2f(0,0);

	isAFallingPlatform = false;
	isAMovingPlatform = false;
	movementPath = NULL;
}


SpecialPlatform::~SpecialPlatform(void)
{
	killPlatformZones = NULL;
}

void SpecialPlatform::SetFallDelay(float delay)
{
	fallDelay = delay;
}

float SpecialPlatform::GetFallDelay()
{
	return fallDelay;
}

//Call when something stands on the platform
void SpecialPlatform::GotStoodOn()
{
	hasBeenStoodOn = true;
	fallClock.restart();

}

void SpecialPlatform::Fall()
{
	shouldBeFalling = true;
	velocity.x = 0;
	fallSound.play();
}

void SpecialPlatform::Update(float deltaTime,sf::Rect<float> &playerBounds,  sf::Vector2f playerVelocity)
{
	if(IsAFallPlatform())
	{
		DoFallingPlatormUpdateLogic(deltaTime, playerBounds, playerVelocity);
	}
	if(IsAMovingPlatform())
	{
		if(GetPlatformFollowPath() != NULL)
		{
			DoMovingPlatformUpdateLogic(deltaTime, playerBounds);
		}
	}

	lastPosition = getPosition();
	move(velocity * deltaTime);
}

void SpecialPlatform::DoFallingPlatormUpdateLogic(float deltaTime, sf::Rect<float> &playerBounds, sf::Vector2f playerVelocity)
{
	//Check if the thing needs to fall
	//If we dont add the velocity the player will have reset itself to not be colliding, so it wont trigger
	sf::FloatRect playerPlusVelocityCollider = playerBounds;
	playerPlusVelocityCollider.left += (playerVelocity.x * deltaTime);
	playerPlusVelocityCollider.top += (playerVelocity.y * deltaTime);

	if(playerPlusVelocityCollider.intersects(getGlobalBounds()))
	{
		//if we're standing on the platform from the top, ie are we above it
		if((playerBounds.top + playerBounds.height) < getGlobalBounds().top)
		{
			GotStoodOn();
		}
	}

	if((hasBeenStoodOn) && (shouldBeFalling == false))
	{
		if(fallClock.getElapsedTime().asSeconds() > fallDelay)
		{
			Fall();
		}
	}

	//make the thing fall if it needs to be
	if(shouldBeFalling == true)
	{
		if(velocity.y + (fallGravity * deltaTime) < fallMaxSpeed)
		{
			velocity.y = velocity.y + (fallGravity * deltaTime);
		}
		else
		{
			velocity.y = fallMaxSpeed;
		}

		//It shouldn't equal null, but better safe than sorry
		if(killPlatformZones != NULL)
		{
			//Check to see if we're in the DEATH ZONES, and if we are set our destroy flag to true
			for(int i = 0; i < killPlatformZones->size(); i++)
			{
				if(getGlobalBounds().intersects(killPlatformZones->at(i)))
				{
					shouldDestroyThisObject = true;
				}
			}
		}
	}
}

void SpecialPlatform::DoMovingPlatformUpdateLogic(float deltaTime, sf::Rect<float> &playerBounds)
{
	//Dont do anything if this platform is falling
	if(shouldBeFalling)
	{
		return;
	}
	else
	{
		//the position of the current node
		sf::Vector2f targetPos = movementPath->GetPointAtIndex(currentMovePathNode);


		//Move towards the current node
		sf::Vector2f moveVelocity = (targetPos - getPosition());
		NormaliseVector(moveVelocity);
		velocity = moveVelocity * movementSpeed;


		//Check to see if we need to go to the next node
		if(VectorDistance(getPosition(),targetPos) < distanceTillGoToNextPathNode)
		{
			//Then we've arrived, cycle to the next node
			if(isGoingBackDownPath == false)
			{

				//if we're going the normal direction up the path
				if(movementPath->GetPathPoints().size() < (currentMovePathNode + 2)) //The +2 makes sense when you think about it, normally it would be +1, but since this part happens after we get the pointAtIndex, we're ... um ... look it just works ok, who am I? Mr Understands Everything of ExplainyTown Alabama? NO!
				{
					//Then we're at the end of the line, need to do something about that
					if(movementPath->IsALoopingPath())
					{
						//If the path loops, go back to the start
						currentMovePathNode = 0;
					}
					else if(!movementPath->IsALoopingPath())
					{
						//the path dosent loop, so we need to ping pong back
						currentMovePathNode = currentMovePathNode - 1;
						isGoingBackDownPath = true;
					}
				}
				else
				{
					currentMovePathNode++;
				}
			}
			else if(isGoingBackDownPath == true)
			{
				//We're going back down the line, so we need to cycle down to the next node
				if((currentMovePathNode) == 0)
				{
					if(!movementPath->IsALoopingPath())
					{
						//the path dosent loop, so we need to ping pong back
						currentMovePathNode = currentMovePathNode + 1;
						isGoingBackDownPath = false;
					}
					else
					{
						//If we're at the beggining of the line, we cant have a looping path or we wouldnt have reversed in the first place
						std::cout << "Somethings gone wrong with the moving platforms, trying to have a backwards looping path, weird" << std::endl;
					}
				}
				else
				{
					currentMovePathNode--;
				}
			}
		}
	}

}

bool SpecialPlatform::ShouldDestroy()
{
	return shouldDestroyThisObject;
}

void SpecialPlatform::SetIsAFallingPlatform(bool _falling)
{
	isAFallingPlatform = _falling;
}

void SpecialPlatform::SetIsAMovingPlatform(bool _moving)
{
	isAMovingPlatform = _moving;
}

void SpecialPlatform::SetFollowingPath(MovementPath *movementPath)
{
	this->movementPath = movementPath;
}

bool SpecialPlatform::IsAFallPlatform()
{
	return isAFallingPlatform;
}

bool SpecialPlatform::IsAMovingPlatform()
{
	return isAMovingPlatform;
}

MovementPath *SpecialPlatform::GetPlatformFollowPath()
{
	if(movementPath != NULL)
	{
		return movementPath;
	}
	else
	{
		std::cout << "Attempting to retreive a movement path from an object when it dosen't seem to have one, returning NULL" << std::endl;
		return NULL;
	}
}

void SpecialPlatform::SetMoveSpeed(float moveSpeed)
{
	this->movementSpeed = moveSpeed;
}

float SpecialPlatform::GetMoveSpeed()
{
	return movementSpeed;
}

sf::Vector2f SpecialPlatform::GetCurrentVelocity()
{
	return velocity;
}

sf::Vector2f SpecialPlatform::GetLastPosition()
{
	return lastPosition;
}


