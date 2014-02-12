#pragma once
#include <SFML\Graphics.hpp>


//Base class for non player characters, such as the background characters

class NPC : public sf::Sprite
{
public:
	NPC(void);
	~NPC(void);

	void Update(double deltaTime);
	void DoExpressionWobble(float _timeToWobbleInSeconds, float _wobbleSpeed, float _wobbleIntensity);

	bool IsWobbling();
private:

	//Lol Wobbles
	bool isWobbling;
	float wobbieIntensity;
	sf::Clock wobbleTimer;
	//These two store the wobble stuff passed in
	float timeToWobble;
	float wobbleSpeed;
	float allowedAngleVariance;

	bool RotateTowardsTargetAngle(float targetAngle, float deltaTime, float rotationSpeed);
	////

};

