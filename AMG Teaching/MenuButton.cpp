#include "MenuButton.h"
#include "Application.h"


//A non toggleable button
MenuButton::MenuButton(float xPos, float yPos, std::string buttonImageRestingPath, std::string buttonImageSelectingPath, bool isSelectable, bool shouldTweenIn, TweenInDirection tweenDirection, float tweenBobAmount, float tweenSpeed, std::function<void()> onClickLogic)
{
	onButtonActivationFunction = onClickLogic;
	LoadNonToggleableValues(xPos,yPos,buttonImageRestingPath,buttonImageSelectingPath, isSelectable, shouldTweenIn,tweenDirection, tweenBobAmount, tweenSpeed);
}

//A toggleable button
MenuButton::MenuButton(float xPos, float yPos, std::string buttonImageRestingPath, std::string buttonImageSelectingPath, std::string buttonImageToggledRestingPath, std::string buttonImageToggledSelectedPath, bool isSelectable, bool shouldTweenIn, TweenInDirection tweenDirection, float tweenBobAmount, float tweenSpeed, std::function<void()> onClickLogic)
{
	onButtonActivationFunction = onClickLogic;
	LoadNonToggleableValues(xPos,yPos,buttonImageRestingPath,buttonImageSelectingPath,isSelectable,shouldTweenIn,tweenDirection, tweenBobAmount, tweenSpeed);

	//Do the extra toggleable bits
	isToggleButton = true; //this must come after LoadNonToggleables because isToggleButton is set to false by default there.
	//Load textures
	if(!buttonImageToggledResting.loadFromFile(buttonImageToggledRestingPath)){};
	if(!buttonImageToggledSelected.loadFromFile(buttonImageToggledSelectedPath)){};

	buttonIsToggled = false;
}

void MenuButton::LoadNonToggleableValues(float xPos, float yPos, std::string buttonImageRestingPath, std::string buttonImageSelectingPath, bool isSelectable, bool shouldTweenIn, TweenInDirection tweenDirection, float tweenBobAmount, float tweenSpeed)
{
	this->isSelectable = isSelectable;

	//Load textures
	if(!buttonImageResting.loadFromFile(buttonImageRestingPath)){};
	if(!buttonImageSelected.loadFromFile(buttonImageSelectingPath)){};


	buttonSprite.setTexture(buttonImageResting);
	buttonSprite.setOrigin(buttonSprite.getGlobalBounds().left + buttonSprite.getGlobalBounds().width/2, buttonSprite.getGlobalBounds().top + buttonSprite.getGlobalBounds().height/2);
	buttonSprite.setPosition(xPos, yPos);

	//If there's a tween
	if(shouldTweenIn)
	{
		tweenInSpeed = tweenSpeed;
		tweenProgressIterator = 0.0f;

		sf::Vector2f tweenStartPosition; //This position is just off the screen in the tween direction, so you cant see the button
		sf::Vector2f tweenBobPosition; //The final position + the bob offset
		sf::Vector2f endTweenPosition = buttonSprite.getPosition();
		if(tweenDirection == Top)
		{
			tweenStartPosition = buttonSprite.getPosition();
			tweenStartPosition.y = 0 - buttonSprite.getLocalBounds().height;
			tweenBobPosition = buttonSprite.getPosition();
			tweenBobPosition.y = tweenBobPosition.y + tweenBobAmount;
		}
		else if(tweenDirection == Bottom)
		{
			tweenStartPosition = buttonSprite.getPosition();
			tweenStartPosition.y = Application::GetWindow().getSize().y + buttonSprite.getLocalBounds().height;
			tweenBobPosition = buttonSprite.getPosition();
			tweenBobPosition.y = tweenBobPosition.y - tweenBobAmount;
		}
		else if(tweenDirection == Left)
		{
			tweenStartPosition = buttonSprite.getPosition();
			tweenStartPosition.x = 0 - buttonSprite.getLocalBounds().width;
			tweenBobPosition = buttonSprite.getPosition();
			tweenBobPosition.x = tweenBobPosition.x + tweenBobAmount;
		}
		else if(tweenDirection == Right)
		{
			tweenStartPosition = buttonSprite.getPosition();
			tweenStartPosition.x = Application::GetWindow().getSize().x + buttonSprite.getLocalBounds().width;
			tweenBobPosition = buttonSprite.getPosition();
			tweenBobPosition.x = tweenBobPosition.x - tweenBobAmount;
		}
		tweenInSpline = std::unique_ptr<MotionSpline>(new MotionSpline(tweenStartPosition,tweenBobPosition,endTweenPosition,endTweenPosition));
	}

	buttonIsToggled = false;
	isToggleButton = false;

}

MenuButton::~MenuButton(void)
{
}

void MenuButton::Update(sf::Event events, bool eventFired, double deltaTime)
{
	DoTweenLogic(deltaTime);
}

void MenuButton::DoTweenLogic(double deltaTime)
{
	if(shouldTweenIn)
	{
		if(tweenProgressIterator < 1.0f)
		{
			if(tweenInSpline != nullptr)
			{
				buttonSprite.setPosition(tweenInSpline->GetPointOnSpline(tweenProgressIterator));
				tweenProgressIterator += deltaTime * tweenInSpeed;
			}
			else
			{
				std::cout << "Cannot tween button, no spline" << std::endl;
			}
		}
	}
}

void MenuButton::Render(sf::RenderWindow &window)
{
	window.draw(buttonSprite);
}

void MenuButton::ResetTween()
{
	tweenProgressIterator = 0.0f;
}

void MenuButton::SetSelected(bool isSelected)
{
	isCurrentlySelected = isSelected;

	if(isCurrentlySelected)
	{
		if(buttonIsToggled)
		{
			buttonSprite.setTexture(buttonImageToggledSelected,true);
		}
		else
		{
			buttonSprite.setTexture(buttonImageSelected,true);
		}
	}
	else
	{
		if(buttonIsToggled)
		{
			buttonSprite.setTexture(buttonImageToggledResting,true);
		}
		else
		{
			buttonSprite.setTexture(buttonImageResting,true);
		}
	}
}

bool MenuButton::IsAToggleButton()
{
	return isToggleButton;
}

void MenuButton::Activate()
{
	if(isToggleButton)
	{
		buttonIsToggled = !buttonIsToggled;
	}

	if(onButtonActivationFunction != nullptr)
	{
		onButtonActivationFunction();
	}
}

sf::Rect<float> MenuButton::GetButtonBounds()
{
	return buttonSprite.getGlobalBounds();
}

bool MenuButton::IsSelectable()
{
	return isSelectable;
}

sf::Vector2f MenuButton::GetPosition()
{
	return buttonSprite.getPosition();
}

void MenuButton::Move(sf::Vector2f movement)
{
	buttonSprite.move(movement);
}