#include "FallingPlatform.h"


FallingPlatform::FallingPlatform(sf::Sprite &platformSprite, float fallDelay, float fallGravity, float fallMaxSpeed, std::vector<sf::Rect<float>>* killPlatformZones,  AudioManager *audioManager) : sf::Sprite(platformSprite)
{
	this->audioManager = audioManager;
	if(audioManager != NULL)
	{
		fallSound.setBuffer(audioManager->GetSoundFile(AudioManager::PlatformFall));
	}
	

	hasBeenStoodOn = false;
	shouldBeFalling = false;
	shouldDestroyThisObject = false;

	this->fallDelay = fallDelay;
	this->fallGravity = fallGravity;
	this->fallMaxSpeed = fallMaxSpeed;
	this->killPlatformZones = killPlatformZones;

	velocity = sf::Vector2f(0,0);
}


FallingPlatform::~FallingPlatform(void)
{
	killPlatformZones = NULL;
}

void FallingPlatform::SetFallDelay(float delay)
{
	fallDelay = delay;
}

float FallingPlatform::GetFallDelay()
{
	return fallDelay;
}

//Call when something stands on the platform
void FallingPlatform::GotStoodOn()
{
	hasBeenStoodOn = true;
	fallClock.restart();

}

void FallingPlatform::Fall()
{
	shouldBeFalling = true;
	fallSound.play();
}

void FallingPlatform::Update(float deltaTime, Player &player)
{
	//Check if the thing needs to fall
	//If we dont add the velocity the player will have reset itself to not be colliding, so it wont trigger
	sf::FloatRect playerPlusVelocityCollider = player.GetCollider();
	playerPlusVelocityCollider.left += (player.GetVelocity().x * deltaTime);
	playerPlusVelocityCollider.top += (player.GetVelocity().y * deltaTime);

	if(playerPlusVelocityCollider.intersects(getGlobalBounds()))
	{
		//if we're standing on the platform from the top, ie are we above it
		if((player.GetCollider().top + player.GetCollider().height) < getGlobalBounds().top)
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

		move(velocity * deltaTime);
	}
}


bool FallingPlatform::ShouldDestroy()
{
	return shouldDestroyThisObject;
}


