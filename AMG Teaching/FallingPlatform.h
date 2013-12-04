#pragma once
#include <SFML\Graphics.hpp>
#include "Player.h"

class FallingPlatform : public sf::Sprite
{
public:
	FallingPlatform(sf::Sprite &platformSprite, float fallDelay, float fallGravity, float fallMaxSpeed, std::vector<sf::Rect<float>>* killPlatformZones, AudioManager *audioManager);
	~FallingPlatform(void);

	void SetFallDelay(float delay);
	float GetFallDelay();
	
	//Run the update logic for falling and whatnot
	//Pass in a reference to the player to the platforms can handle themselves falling out of the sky. DATS GOOD OOP, I THINK. MAYBE ITS AWFUL? MAYBE MY CAPS LOCK KEY IS HELD DOWN? MAYBE IM DOING THE WORK OF THREE BLOODY PROGRAMMERS IN THE ONE DAY I CAN SPARE FROM MY OTHER COURSEWORK AND COMMITMENTS? WHO BLOODY KNOWS ANYMORE!?
	//We would like to apologise for the fault in the ending of the above comment, the programmer responsible has been sacked.
	//Again, we would like to apologise for the fault in the above comment apologising for the fault in the comment two line above, those responsible for sacking the programmer have been sacked
	//etc...
	void Update(float deltaTime, Player &player);

	//When someone stands on the platform, it needs to trigger the fall countdown
	void GotStoodOn();

	//returns true if this object needs to be destroyed, else false
	bool ShouldDestroy();

private:
	//Store a reference to the audio manager
	AudioManager* audioManager;
	sf::Sound fallSound;

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
};

