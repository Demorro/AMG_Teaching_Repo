#include "ControlState.h"
#include "Application.h"

ControlState::ControlState(void) : State(State::CONTROL_STATE)
{
}


ControlState::~ControlState(void)
{
}

bool ControlState::Load()
{
	//load things here, remember to do load checks if possible
	if(!controllerTexture.loadFromFile(CONTROLSIMAGE))
	{
	}
	controllerSprite.setTexture(controllerTexture);
	controllerSprite.setOrigin((controllerSprite.getGlobalBounds().left + (controllerSprite.getGlobalBounds().width/2)),(controllerSprite.getGlobalBounds().top + (controllerSprite.getGlobalBounds().height/2)));
	controllerSprite.setPosition(Application::GetWindow().getSize().x/2, Application::GetWindow().getSize().y/2);
	
	float backToMenuButtonDistanceFromCenter = -500.0f;
	float backToMenuButtonDistanceFromBottom = 100.0f;
	float menuElementsBobAmount = 150.0f;
	float menuElementsTweenSpeed = 1.4f;

	controlScreenMenuSystem.AddMenuElement(Application::GetWindow().getSize().x/2 + backToMenuButtonDistanceFromCenter, Application::GetWindow().getSize().y - backToMenuButtonDistanceFromBottom, BACKTOMENUNORMAL, BACKTOMENUSELECTED, true, true, MenuButton::TweenInDirection::Left, menuElementsBobAmount, menuElementsTweenSpeed, std::function<void()>(std::bind(&ControlState::GoBackToMenuState,this)));
	controlScreenMenuSystem.MoveToFirstButton();
	return true;
}

void ControlState::Update(sf::Event events, bool eventFired, double deltaTime)
{
	controlScreenMenuSystem.Update(events,eventFired,deltaTime);
}

void ControlState::Draw(sf::RenderWindow &renderWindow)
{
	controlScreenMenuSystem.Render(renderWindow);
	renderWindow.draw(controllerSprite);
}

void ControlState::GoBackToMenuState()
{
	SwitchState(State::MENU_STATE);
}
