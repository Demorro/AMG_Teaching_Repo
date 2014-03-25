#pragma once
#include <SFML\Graphics.hpp>
#include <SFML\Audio.hpp>
#include "MovementPath.h"
#include "VectorMath.h"
#include "InterStateSingleTon.h"

class SpecialPlatform : public sf::Sprite
{
public:
	SpecialPlatform(sf::Sprite &platformSprite, float fallDelay, float fallGravity, float fallMaxSpeed, float movementSpeed, float distanceTillGoToNextPathNode, std::vector<sf::Rect<float>>* killPlatformZones, sf::SoundBuffer &fallingSound);
	~SpecialPlatform(void);

	//Sets if the patforms is falling, following a path, etc
	void SetIsAFallingPlatform(bool _falling);
	void SetIsAMovingPlatform(bool _moving);
	void SetFollowingPath(MovementPath *movementPath);

	bool IsAFallPlatform();
	bool IsAMovingPlatform();
	MovementPath *GetPlatformFollowPath();

	void SetFallDelay(float delay);
	float GetFallDelay();

	void SetMoveSpeed(float moveSpeed);
	float GetMoveSpeed();

	sf::Vector2f GetLastPosition();

	sf::Vector2f GetCurrentVelocity();
	
	//Run the update logic for falling and whatnot
	//Pass in a reference to the player to the platforms can handle themselves falling out of the sky. DATS GOOD OOP, I THINK. MAYBE ITS AWFUL? MAYBE MY CAPS LOCK KEY IS HELD DOWN? MAYBE IM DOING THE WORK OF THREE BLOODY PROGRAMMERS IN THE ONE DAY I CAN SPARE FROM MY OTHER COURSEWORK AND COMMITMENTS? WHO EVEN BLOODY KNOWS ANYMORE!?
	//We would like to apologise for the fault in the ending of the above comment, the programmer responsible has been sacked.
	//Again, we would like to apologise for the fault in the above comment apologising for the fault in the comment two line above, those responsible for sacking the programmer have been sacked
	//etc...
	void Update(float deltaTime, sf::Rect<float> &playerBounds, sf::Vector2f playerVelocity);

	//When someone stands on the platform, it needs to trigger the fall countdown
	void GotStoodOn();

	//returns true if this object needs to be destroyed, else false
	bool ShouldDestroy();

private:

	sf::Sound fallSound;

	bool isAFallingPlatform;
	bool isAMovingPlatform;
	//Store a reference to the movementPath that this platform may use
	MovementPath *movementPath;

	void DoFallingPlatormUpdateLogic(float deltaTime, sf::Rect<float> &playerBounds, sf::Vector2f playerVelocity);
	void DoMovingPlatformUpdateLogic(float deltaTime, sf::Rect<float> &playerBounds);


	bool hasBeenStoodOn;

	sf::Vector2f velocity;
	float fallGravity;
	float fallMaxSpeed;

	float fallDelay;
	//the clock the times how long until the fall
	sf::Clock fallClock;

	//Makes the platform fall
	void Fall();
	bool shouldBeFalling;

	//true if this object needs to be destroyed(because it has fallen into a death zone probably) else false
	bool shouldDestroyThisObject;

	//Keep a pointer to a vector of a bunch of rects that will cause this platform to be destroyed upon collision. Usually just one DEATHZONE, but can be more.
	std::vector<sf::Rect<float>> *killPlatformZones;

	//the position the platform was at last frame, used when moving the player to the right place on the platform
	sf::Vector2f lastPosition;
	//the speed the platform moves at if it is a moving platform
	float movementSpeed;
	//The min value of the distance check required to find out if we've arrived at a node on the movement path yet
	float distanceTillGoToNextPathNode;
	//Used for keeping track of what node we're moving to next
	int currentMovePathNode;
	//If the path isnt closed, ie in a state where we ping pong back and forth, we need to keep track if we're going forward or backwards down it
	bool isGoingBackDownPath;
};

