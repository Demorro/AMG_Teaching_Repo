#include "MenuState.h"
#include "Application.h"

MenuState::MenuState(void) : State(State::MENU_STATE)
{
	selectedButton = nullptr;
}


MenuState::~MenuState(void)
{
}

bool MenuState::Load()
{
	selectionButtons.push_back(sf::Keyboard::Return);


	if(!backgroundImage.loadFromFile(MENUBACKGROUND)){};
	backGroundSprite.setTexture(backgroundImage);

	float menuElementsBobAmount = 200.0f;
	float menuElementsTweenSpeed = 0.9f;

	float titleDistanceFromTop = 125;
	menuElements.push_back(std::unique_ptr<MenuButton>(new MenuButton(Application::GetWindow().getSize().x/2, titleDistanceFromTop, GAMELOGO, GAMELOGO, true, MenuButton::TweenInDirection::Top, menuElementsBobAmount, menuElementsTweenSpeed)));

	float startButtonDistanceFromBottom = 110;
	menuElements.push_back(std::unique_ptr<MenuButton>(new MenuButton(Application::GetWindow().getSize().x/2, Application::GetWindow().getSize().y - startButtonDistanceFromBottom, STARTBUTTON, STARTBUTTONSELECTED, true, MenuButton::TweenInDirection::Bottom, menuElementsBobAmount, menuElementsTweenSpeed, std::function<void()>(std::bind(&MenuState::GoToFirstLevelState,this)))));

	float controlButtonDistanceFromBottom = 110;
	float controlButtonDistanceFromCenterX = -500;
	menuElements.push_back(std::unique_ptr<MenuButton>(new MenuButton(Application::GetWindow().getSize().x/2 + controlButtonDistanceFromCenterX, Application::GetWindow().getSize().y - controlButtonDistanceFromBottom, CONTROLBUTTONNORMAL, CONTROLBUTTONSELECTED, true, MenuButton::TweenInDirection::Bottom, menuElementsBobAmount, menuElementsTweenSpeed, std::function<void()>(std::bind(&MenuState::GoToControlsState,this)))));

	float volumeButtonDistanceFromBottom = 110;
	float volumeButtonDistanceFromCenterX = 500;
	menuElements.push_back(std::unique_ptr<MenuButton>(new MenuButton(Application::GetWindow().getSize().x/2 + volumeButtonDistanceFromCenterX, Application::GetWindow().getSize().y - volumeButtonDistanceFromBottom, VOLUMEBUTTONONNORMAL, VOLUMEBUTTONONSELECTED, VOLUMEBUTTONOFFNORMAL, VOLUMEBUTTONOFFSELECTED, true, MenuButton::TweenInDirection::Bottom, menuElementsBobAmount, menuElementsTweenSpeed, std::function<void()>(std::bind(&MenuState::ToggleVolume,this)))));

	return true;
}

void MenuState::Update(sf::Event events, bool eventFired, double deltaTime)
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

void MenuState::Draw(sf::RenderWindow &renderWindow)
{
	//Draw things here
	renderWindow.draw(backGroundSprite);

	for(int i = 0; i < menuElements.size(); i++)
	{
		menuElements[i]->Render(renderWindow);
	}
}


//These functions are passed to the buttons and executed upon clickety clackity click
void MenuState::ToggleVolume()
{
	if(interStateSingleton.GetIsVolumeOn())
	{
		interStateSingleton.SetIsVolumeOn(false);
	}
	else
	{
		interStateSingleton.SetIsVolumeOn(true);
	}
}

void MenuState::GoToFirstLevelState()
{
	std::cout << "ATADETG" << std::endl;
	SwitchState(State::LEVEL1_STATE);
}

void MenuState::GoToControlsState()
{
	SwitchState(State::CONTROL_STATE);
}




