#include "MenuState.h"
#include "Application.h"

MenuState::MenuState(void) : State(State::MENU_STATE)
{
}


MenuState::~MenuState(void)
{
}

bool MenuState::Load()
{
	//load things here, remember to do load checks if possible
	fadeRectangle.setFillColor(sf::Color::Black);
	sf::Color startColor = fadeRectangle.getFillColor();
	startColor.a = 0.0f;
	fadeRectangle.setFillColor(startColor);
	fadeRectangle.setSize(sf::Vector2f(Application::GetWindow().getSize()));
	fadeRectangle.setPosition(0,0);



	if(!backgroundImage.loadFromFile(MENUBACKGROUND))
	{
		//error
	}
	backGroundSprite.setTexture(backgroundImage);

	if(!gameLogoTexture.loadFromFile(GAMELOGO))
	{
		//error
	}
	gameLogoSprite.setTexture(gameLogoTexture);

	if(!startButtonTexture.loadFromFile(STARTBUTTON))
	{
		//error
	}
	if(!startButtonSelectedTexture.loadFromFile(STARTBUTTONSELECTED))
	{
		//error
	}
	startButtonSprite.setTexture(startButtonTexture);

	//Logo
	float logoYOffsetFromTop = 100;
	gameLogoSprite.setOrigin((gameLogoSprite.getGlobalBounds().left + (gameLogoSprite.getGlobalBounds().width/2)),(gameLogoSprite.getGlobalBounds().top + (gameLogoSprite.getGlobalBounds().height/2)));
	gameLogoSprite.setPosition(Application::GetWindow().getSize().x/2, logoYOffsetFromTop);
	float logoTweenLength = 250;
	float logoBobDistance = 250;
	sf::Vector2f logoStartPos = gameLogoSprite.getPosition();
	logoStartPos.y -= logoTweenLength;
	sf::Vector2f logoBobPos = gameLogoSprite.getPosition();
	logoBobPos.y += logoBobDistance;
	initialLogoTween = std::unique_ptr<MotionSpline>(new MotionSpline(logoStartPos,logoBobPos,gameLogoSprite.getPosition(),gameLogoSprite.getPosition()));
	logoTweenProgressIterator = 0.0f;
	logoTweenSpeed = 0.95f;
	gameLogoSprite.setPosition(logoStartPos);
	
	//Start Button
	float startButtonYOffsetFromBottom = 100;
	startButtonSprite.setOrigin((startButtonSprite.getGlobalBounds().left + (startButtonSprite.getGlobalBounds().width/2)),(startButtonSprite.getGlobalBounds().top + (startButtonSprite.getGlobalBounds().height/2)));
	startButtonSprite.setPosition(Application::GetWindow().getSize().x/2, Application::GetWindow().getSize().y - startButtonYOffsetFromBottom);
	float startButtonTweenLength = 250;
	float startButtonBobDistance = 200;
	sf::Vector2f startButtonStartPos = startButtonSprite.getPosition();
	startButtonStartPos.y += startButtonTweenLength;
	sf::Vector2f startButtonBobPos = startButtonSprite.getPosition();
	startButtonBobPos.y -= startButtonBobDistance;
	initialStartButtonTween = std::unique_ptr<MotionSpline>(new MotionSpline(startButtonStartPos,startButtonBobPos,startButtonSprite.getPosition(),startButtonSprite.getPosition()));
	startButtonTweenProgressIterator = 0.0f;
	startButtonTweenSpeed = 0.95f;
	startButtonIsSelected = false;
	startButtonSprite.setPosition(startButtonStartPos);

	shouldTweenInMenuElements = true;
	return true;
}

void MenuState::Update(sf::Event events, bool eventFired, double deltaTime)
{

	//Update things here, remember about deltatime for framerate independent movement	
	if(shouldTweenInMenuElements)
	{
		if(logoTweenProgressIterator < 1.0f)
		{
			gameLogoSprite.setPosition(initialLogoTween->GetPointOnSpline(logoTweenProgressIterator));
			logoTweenProgressIterator += deltaTime * logoTweenSpeed;
		}

		if(startButtonTweenProgressIterator < 1.0f)
		{
			startButtonSprite.setPosition(initialStartButtonTween->GetPointOnSpline(startButtonTweenProgressIterator));
			startButtonTweenProgressIterator += deltaTime * startButtonTweenSpeed;
		}
	}

	if(startButtonIsSelected == false)
	{
		if(startButtonSprite.getGlobalBounds().contains(sf::Vector2f(sf::Mouse::getPosition(Application::GetWindow()))))
		{
			startButtonSprite.setTexture(startButtonSelectedTexture);
			startButtonIsSelected = true;
		}
	}
	else if (startButtonIsSelected)
	{
		if(!startButtonSprite.getGlobalBounds().contains(sf::Vector2f(sf::Mouse::getPosition(Application::GetWindow()))))
		{
			startButtonSprite.setTexture(startButtonTexture);
			startButtonIsSelected = false;
		}
		else
		{
			//Start Game
			if(sf::Mouse::isButtonPressed(sf::Mouse::Left))
			{
				SwitchState(StateID::LEVEL1_STATE);
			}
		}
	}

}

void MenuState::Draw(sf::RenderWindow &renderWindow)
{
	

	//Draw things here
	renderWindow.draw(backGroundSprite);
	renderWindow.draw(gameLogoSprite);
	renderWindow.draw(startButtonSprite);

	//initialLogoTween->Render(renderWindow);

	//The rectangle for fading in/out
	renderWindow.draw(fadeRectangle);
}



