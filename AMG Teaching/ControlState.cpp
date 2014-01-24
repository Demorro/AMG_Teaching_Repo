#include "ControlState.h"
#include "Application.h"

ControlState::ControlState(void) : State(State::CONTROL_STATE)
{
	selectedButton = nullptr;
}


ControlState::~ControlState(void)
{
}

bool ControlState::Load()
{
	selectionButtons.push_back(sf::Keyboard::Return);

	//load things here, remember to do load checks if possible
	if(!controllerTexture.loadFromFile(CONTROLSIMAGE))
	{
	}
	controllerSprite.setTexture(controllerTexture);
	controllerSprite.setOrigin((controllerSprite.getGlobalBounds().left + (controllerSprite.getGlobalBounds().width/2)),(controllerSprite.getGlobalBounds().top + (controllerSprite.getGlobalBounds().height/2)));
	controllerSprite.setPosition(Application::GetWindow().getSize().x/2, Application::GetWindow().getSize().y/2);

	float menuElementsBobAmount = 200.0f;
	float menuElementsTweenSpeed = 0.9f;

	float backToMenuButtonDistanceFromBottom = 110;
	float backToMenuButtonDistanceFromCenter = -500;
	menuElements.push_back(std::unique_ptr<MenuButton>(new MenuButton(Application::GetWindow().getSize().x/2 + backToMenuButtonDistanceFromCenter, Application::GetWindow().getSize().y - backToMenuButtonDistanceFromBottom, BACKTOMENUNORMAL, BACKTOMENUSELECTED, true, MenuButton::TweenInDirection::Bottom, menuElementsBobAmount, menuElementsTweenSpeed, std::function<void()>(std::bind(&ControlState::GoBackToMenuState,this)))));
	return true;
}

void ControlState::Update(sf::Event events, bool eventFired, double deltaTime)
{
	for(int i = 0; i < menuElements.size(); i++)
	{
		menuElements[i]->Update(events,eventFired,deltaTime);

		//Mouse selection
		//Reset all selections
		for(int j = 0; j < menuElements.size(); j++)
		{
			menuElements[j]->SetSelected(false);
		}
		//get a selected button
		if(menuElements[i]->GetButtonBounds().contains(sf::Vector2f(sf::Mouse::getPosition(Application::GetWindow()))))
		{
			selectedButton = menuElements[i].get();
		}
		//highlight selected button
		if(selectedButton != nullptr)
		{
			selectedButton->SetSelected(true);
		}		
	}

	//Check for activation of selected button
	if(selectedButton != nullptr)
	{
		if(eventFired)
		{
			if((events.type == sf::Event::MouseButtonPressed) || (events.type == sf::Event::KeyReleased))
			{
				for(int i = 0; i < selectionButtons.size(); i++)
				{
					if((events.mouseButton.button == sf::Mouse::Left) || (events.key.code == selectionButtons[i]))
					{
						//Activate the button, it will toggle if it is a toggle button
						selectedButton->Activate();
					}
				}
			}
		}
	}
}

void ControlState::Draw(sf::RenderWindow &renderWindow)
{
	renderWindow.draw(controllerSprite);
	for(int i = 0; i < menuElements.size(); i++)
	{
		menuElements[i]->Render(renderWindow);
	}
}

void ControlState::GoBackToMenuState()
{
	SwitchState(State::MENU_STATE);
}
