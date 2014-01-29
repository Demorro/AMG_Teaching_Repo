#include "TweeningText.h"


TweeningText::TweeningText(sf::Font &font, int textSize, sf::Vector2f position, std::string initialString,  bool shouldTweenIn, bool lockedToCamera, Camera *camera) : TweenableElement(shouldTweenIn, position)
{
	text.setFont(font);
	text.setCharacterSize(textSize);
	text.setPosition(position);
	text.setString(initialString);


	this->shouldTweenIn = shouldTweenIn;
	this->isLockedToCamera = lockedToCamera;
	this->gameCam = camera;

}


TweeningText::~TweeningText(void)
{
}

void TweeningText::Update(sf::Event events, bool eventFired, double deltaTime)
{
	if(isLockedToCamera)
	{
		if(gameCam != nullptr)
		{
			DoTweenLogic(deltaTime,text);
		}
	}
	else
	{
		DoTweenLogic(deltaTime,text);
	}
}

void TweeningText::Render(sf::RenderWindow& window)
{
	window.draw(text);
}

void TweeningText::SetText(std::string newText)
{
	text.setString(newText);
}

sf::Vector2f TweeningText::GetPosition()
{
	return text.getPosition();
}

sf::Rect<float> TweeningText::GetLocalBounds()
{
	return text.getLocalBounds();
}

void TweeningText::Move(sf::Vector2f movement)
{
	text.move(movement);
}