#include "MenuButton.h"
#include "Application.h"


//A non toggleable button
MenuButton::MenuButton(float xPos, float yPos, std::string buttonImageRestingPath, std::string buttonImageSelectingPath, bool isSelectable, bool shouldTweenIn, std::function<void()> onClickLogic) : TweenableElement(shouldTweenIn, sf::Vector2f(xPos,yPos))
{
	onButtonActivationFunction = onClickLogic;
	LoadNonToggleableValues(xPos,yPos,buttonImageRestingPath,buttonImageSelectingPath, isSelectable);
}

//A toggleable button
MenuButton::MenuButton(float xPos, float yPos, std::string buttonImageRestingPath, std::string buttonImageSelectingPath, std::string buttonImageToggledRestingPath, std::string buttonImageToggledSelectedPath, bool isSelectable, bool shouldTweenIn, std::function<void()> onClickLogic) : TweenableElement(shouldTweenIn, sf::Vector2f(xPos,yPos))
{
	onButtonActivationFunction = onClickLogic;
	LoadNonToggleableValues(xPos,yPos,buttonImageRestingPath,buttonImageSelectingPath,isSelectable);

	//Do the extra toggleable bits
	isToggleButton = true; //this must come after LoadNonToggleables because isToggleButton is set to false by default there.
	//Load textures
	if(!buttonImageToggledResting.loadFromFile(buttonImageToggledRestingPath)){};
	if(!buttonImageToggledSelected.loadFromFile(buttonImageToggledSelectedPath)){};

	buttonIsToggled = false;
}

void MenuButton::LoadNonToggleableValues(float xPos, float yPos, std::string buttonImageRestingPath, std::string buttonImageSelectingPath, bool isSelectable)
{
	this->isSelectable = isSelectable;

	//Load textures
	if(!buttonImageResting.loadFromFile(buttonImageRestingPath)){};
	if(!buttonImageSelected.loadFromFile(buttonImageSelectingPath)){};


	buttonSprite.setTexture(buttonImageResting);
	buttonSprite.setOrigin(buttonSprite.getGlobalBounds().left + buttonSprite.getGlobalBounds().width/2, buttonSprite.getGlobalBounds().top + buttonSprite.getGlobalBounds().height/2);
	buttonSprite.setPosition(xPos, yPos);

	buttonIsToggled = false;
	isToggleButton = false;
}

MenuButton::~MenuButton(void)
{
}


void MenuButton::Update(sf::Event events, bool eventFired, double deltaTime)
{
	DoTweenLogic(deltaTime,buttonSprite);
}


void MenuButton::Render(sf::RenderWindow &window)
{
	window.draw(buttonSprite);
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