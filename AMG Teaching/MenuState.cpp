#include "MenuState.h"
#include "Application.h"

MenuState::MenuState(void) : State(State::MENU_STATE)
{
	selectedButton = nullptr;

	canMoveButtonTimer.restart();
	timeTillCanMoveButton = 0.18f;
}


MenuState::~MenuState(void)
{
}

bool MenuState::Load()
{
	selectionButtons.push_back(sf::Keyboard::Return);
	joystickSelectionButtons.push_back(std::pair<int,int>(0,XBOXCONTROLLERBUTTONS::A));


	if(!backgroundImage.loadFromFile(MENUBACKGROUND)){};
	backGroundSprite.setTexture(backgroundImage);

	float menuElementsBobAmount = 200.0f;
	float menuElementsTweenSpeed = 0.9f;

	float titleDistanceFromTop = 125;
	menuElements.push_back(std::unique_ptr<MenuButton>(new MenuButton(Application::GetWindow().getSize().x/2, titleDistanceFromTop, GAMELOGO, GAMELOGO, false, true, MenuButton::TweenInDirection::Top, menuElementsBobAmount, menuElementsTweenSpeed)));

	float startButtonDistanceFromBottom = 150;
	menuElements.push_back(std::unique_ptr<MenuButton>(new MenuButton(Application::GetWindow().getSize().x/2, Application::GetWindow().getSize().y - startButtonDistanceFromBottom, STARTBUTTON, STARTBUTTONSELECTED, true, true, MenuButton::TweenInDirection::Bottom, menuElementsBobAmount, menuElementsTweenSpeed, std::function<void()>(std::bind(&MenuState::GoToFirstLevelState,this)))));

	float quitButtonDistanceFromBottom = 60;
	menuElements.push_back(std::unique_ptr<MenuButton>(new MenuButton(Application::GetWindow().getSize().x/2, Application::GetWindow().getSize().y - quitButtonDistanceFromBottom, QUITBUTTON, QUITBUTTONSELECTED, true, true, MenuButton::TweenInDirection::Bottom, menuElementsBobAmount, menuElementsTweenSpeed, std::function<void()>(std::bind(&MenuState::QuitApplication,this)))));

	float controlButtonDistanceFromBottom = 110;
	float controlButtonDistanceFromCenterX = -500;
	menuElements.push_back(std::unique_ptr<MenuButton>(new MenuButton(Application::GetWindow().getSize().x/2 + controlButtonDistanceFromCenterX, Application::GetWindow().getSize().y - controlButtonDistanceFromBottom, CONTROLBUTTONNORMAL, CONTROLBUTTONSELECTED, true, true, MenuButton::TweenInDirection::Bottom, menuElementsBobAmount, menuElementsTweenSpeed, std::function<void()>(std::bind(&MenuState::GoToControlsState,this)))));

	float volumeButtonDistanceFromBottom = 110;
	float volumeButtonDistanceFromCenterX = 500;
	menuElements.push_back(std::unique_ptr<MenuButton>(new MenuButton(Application::GetWindow().getSize().x/2 + volumeButtonDistanceFromCenterX, Application::GetWindow().getSize().y - volumeButtonDistanceFromBottom, VOLUMEBUTTONONNORMAL, VOLUMEBUTTONONSELECTED, VOLUMEBUTTONOFFNORMAL, VOLUMEBUTTONOFFSELECTED, true, true, MenuButton::TweenInDirection::Bottom, menuElementsBobAmount, menuElementsTweenSpeed, std::function<void()>(std::bind(&MenuState::ToggleVolume,this)))));

	for(int i = 0; i < menuElements.size(); i++)
	{
		if(menuElements[i]->IsSelectable())
		{
			MoveToButton(menuElements[i].get());
			break;
		}
	}
	return true;
}

void MenuState::Update(sf::Event events, bool eventFired, double deltaTime)
{
	for(int i = 0; i < menuElements.size(); i++)
	{
		menuElements[i]->Update(events,eventFired,deltaTime);
	}

	HandleButtonMouseSelection(events, eventFired, deltaTime);
	HandleButtonControllerSelection();
	HandleButtonActivation(events, eventFired);
}

void MenuState::HandleButtonMouseSelection(sf::Event events, bool eventFired, double deltaTime)
{
	for(int i = 0; i < menuElements.size(); i++)
	{

		//dont bother if the element isn't selectable
		if(menuElements[i]->IsSelectable())
		{
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
				if(selectedButton != menuElements[i].get())
				{
					//Not strictly neccesary, but MoveToButton will be where we place sound playing and stuff, so this will make it consistent.
					MoveToButton(menuElements[i].get());
				}
			}
			//highlight selected button
			if(selectedButton != nullptr)
			{
				selectedButton->SetSelected(true);
			}	
		}
	}
}

void MenuState::HandleButtonControllerSelection()
{
	if(canMoveButtonTimer.getElapsedTime().asSeconds() > timeTillCanMoveButton) //Dont even bother if the time hasn't passed
	{
		enum MenuMovements
		{
			Up,
			Down,
			Left,
			Right,
			None
		};
		MenuMovements thisMove = MenuMovements::None;

		//Get the current movement if we want to move, favour left and right over up and down
		if(sf::Joystick::getAxisPosition(0, sf::Joystick::X) > ANALOGUESTICKMOVEBOUNDRY)
		{
			//Move right
			thisMove = MenuMovements::Right;
		}
		else if(sf::Joystick::getAxisPosition(0, sf::Joystick::X) < -ANALOGUESTICKMOVEBOUNDRY)
		{
			//Move left
			thisMove = MenuMovements::Left;
		}
		else if(sf::Joystick::getAxisPosition(0, sf::Joystick::Y) > ANALOGUESTICKMOVEBOUNDRY)
		{
			//Move Up
			thisMove = MenuMovements::Down;
		}
		else if(sf::Joystick::getAxisPosition(0, sf::Joystick::Y) < -ANALOGUESTICKMOVEBOUNDRY)
		{
			//Move Down
			thisMove = MenuMovements::Up;
		}


		if((thisMove != MenuMovements::None) && (selectedButton != nullptr))
		{
			/*Algorithm(lol, hardly) for finding what button to jump too :
			find the distance of all the buttons from the current one
			check if the least distant is in the direction pushed on the controller
			if yes, go there, if not check the next distant
			*/

			//Vector of candidate buttons, the pair is a pointer to the button and the distance from the selected buttons
			std::vector<std::pair<MenuButton*, float>> candidateMoveButtons;
			candidateMoveButtons.resize(menuElements.size(), std::pair<MenuButton*,float>(nullptr,0));
			for(int i = 0; i < menuElements.size(); i++)
			{
				//get button and distance
				if(selectedButton != nullptr)
				{
					candidateMoveButtons[i] = std::pair<MenuButton*,float>(menuElements[i].get(),VectorDistance(selectedButton->GetPosition(), menuElements[i]->GetPosition()));
				}
			}

			//sort the vector to have the shortest distance first, using the second element of the pair to sort, yay lamda functions
			sort(candidateMoveButtons.begin(), candidateMoveButtons.end(),
			[](const std::pair<MenuButton*, float>& lhs, const std::pair<MenuButton*, float>& rhs)
			{
					return lhs.second < rhs.second; 
			});

			//check it's in the right direction
			MenuButton* buttonToMoveTo = nullptr;
			for(int i = 0; i < candidateMoveButtons.size(); i++)
			{
				if(candidateMoveButtons[i].first != nullptr)
				{
					//Dont bother if the button isnt selectable
					if(candidateMoveButtons[i].first->IsSelectable())
					{
						//Check if we actually want to move to this button
						if(thisMove == MenuMovements::Right)
						{
							if(candidateMoveButtons[i].first->GetPosition().x > selectedButton->GetPosition().x)
							{
								//Move
								MoveToButton(candidateMoveButtons[i].first);
								return; //gotta break out or we'll move to the further out buttons as well
							}
						}
						else if(thisMove == MenuMovements::Left)
						{
							if(candidateMoveButtons[i].first->GetPosition().x < selectedButton->GetPosition().x)
							{
								//Move
								MoveToButton(candidateMoveButtons[i].first);
								return;
							}
						}
						else if(thisMove == MenuMovements::Up)
						{
							if(candidateMoveButtons[i].first->GetPosition().y < selectedButton->GetPosition().y)
							{
								//Move
								MoveToButton(candidateMoveButtons[i].first);
								return;
							}
						}
						else if(thisMove == MenuMovements::Down)
						{
							if(candidateMoveButtons[i].first->GetPosition().y > selectedButton->GetPosition().y)
							{
								//Move
								MoveToButton(candidateMoveButtons[i].first);
								return;
							}
						}
					}
				}
			}
		}
	}
}

void MenuState::HandleButtonActivation(sf::Event events, bool eventFired)
{
	//Check for activation of selected button with the mouse or keyboard
	if(selectedButton != nullptr)
	{
		if(eventFired)
		{
			if((events.type == sf::Event::MouseButtonPressed) || (events.type == sf::Event::KeyReleased))
			{
				//check for the keyboard selection(and mouse click)
				for(int i = 0; i < selectionButtons.size(); i++)
				{
					if((events.mouseButton.button == sf::Mouse::Left) || (events.key.code == selectionButtons[i]))
					{
						//Activate the button, it will toggle if it is a toggle button
						selectedButton->Activate();
					}
				}
			}
			else if(events.type == sf::Event::JoystickButtonPressed)
			{
				//check for the joystick selections
				for(int i = 0; i < joystickSelectionButtons.size(); i++)
				{
					if((events.joystickButton.joystickId == joystickSelectionButtons[i].first) && (events.joystickButton.button == joystickSelectionButtons[i].second))
					{
						//Activate the button, it will toggle if it is a toggle button
						selectedButton->Activate();
					}
				}
			}
		}
	}
}

void MenuState::MoveToButton(MenuButton* button)
{
	if(button != nullptr)
	{
		selectedButton = button;
		canMoveButtonTimer.restart();
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
	SwitchState(State::LEVEL1_STATE);
}

void MenuState::QuitApplication()
{
	Application::SetRunning(false);
}

void MenuState::GoToControlsState()
{
	SwitchState(State::CONTROL_STATE);
}




