#pragma once
#include "TweenableElement.h"
#include "Camera.h"

class TweeningText : public TweenableElement
{
public:
	TweeningText(sf::Font &font, int textSize, sf::Vector2f position, std::string initialString, bool shouldTweenIn = true, bool lockedToCamera = false, Camera *camera = nullptr);
	~TweeningText(void);

	void Update(sf::Event events, bool eventFired, double deltaTime);
	void Render(sf::RenderWindow& window);

	void SetText(std::string newText);

	sf::Vector2f GetPosition();
	sf::Rect<float> GetLocalBounds();

	void Move(sf::Vector2f movement);
private:
	sf::Text text;
	bool isLockedToCamera;
	//A reference to the camera if it is passed in
	Camera* gameCam;
};

