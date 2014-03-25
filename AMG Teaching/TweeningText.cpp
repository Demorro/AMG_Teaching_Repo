#include "TweeningText.h"


TweeningText::TweeningText(sf::Font &font, int textSize, sf::Vector2f position, std::string initialString,  bool shouldTweenIn, bool lockedToCamera, Camera *camera) : TweenableElement(shouldTweenIn, position)
{
	text.setFont(font);
	text.setCharacterSize(textSize);
	text.setString(initialString);
	text.setOrigin(text.getGlobalBounds().width/2, text.getGlobalBounds().height/2);
	text.setPosition(position);

	this->shouldTweenIn = shouldTweenIn;
	this->isLockedToCamera = lockedToCamera;
	this->gameCam = camera;

	ResetTween();
}


TweeningText::~TweeningText(void)
{
}

void TweeningText::Update(sf::Event events, bool eventFired, double deltaTime)
{
	DoTweenLogic(deltaTime,text);
}

void TweeningText::Render(sf::RenderWindow& window)
{
	if(isLockedToCamera)
	{
		if(gameCam != nullptr)
		{
			text.move(gameCam->GetScreenSpaceOffsetVector());
			window.draw(text);
			text.move(-gameCam->GetScreenSpaceOffsetVector());
		}
	}
	else
	{
		window.draw(text);
	}
}

void TweeningText::SetText(std::string newText)
{
	text.setString(newText);
}

void TweeningText::SetTextColor(sf::Color textColor)
{
	text.setColor(textColor);
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