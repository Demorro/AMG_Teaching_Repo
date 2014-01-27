#include "MenuSystem.h"
#include "Application.h"

MenuSystem::MenuSystem(void)
{
	selectedButton = nullptr;

	canMoveButtonTimer.restart();
	timeTillCanMoveButton = 0.18f;

	//default selection buttons
	keyboardSelectionButtons.push_back(sf::Keyboard::Return);
	joystickSelectionButtons.push_back(std::pair<int,int>(0,XBOXCONTROLLERBUTTONS::A));

	RecheckIfVolumeIsOn();

	//Load default sounds
	LoadMoveBetweenButtonSound(MOVEBETWEENBUTTONSOUND);
	LoadSelectButtonSound(SELECTBUTTONSOUND);
}


MenuSystem::~MenuSystem(void)
{
}

void MenuSystem::Update(sf::Event events, bool eventFired, double deltaTime)
{
	for(int i = 0; i < menuElements.size(); i++)
	{
		menuElements[i]->Update(events,eventFired,deltaTime);
	}

	HandleButtonMouseSelection(events, eventFired, deltaTime);
	HandleButtonControllerSelection();
	HandleButtonActivation(events, eventFired);
}

void MenuSystem::Render(sf::RenderWindow &renderWindow)
{
	for(int i = 0; i < menuElements.size(); i++)
	{
		menuElements[i]->Render(renderWindow);
	}
}

void MenuSystem::Render(sf::RenderWindow &renderWindow, Camera &camera)
{
	for(int i = 0; i < menuElements.size(); i++)
	{
		sf::Vector2f screenCorrectionMoveVector = camera.GetPosition();
		screenCorrectionMoveVector.x -= Application::GetWindow().getSize().x/2;
		screenCorrectionMoveVector.y -= Application::GetWindow().getSize().y/2;
		menuElements[i]->Move(screenCorrectionMoveVector);
		menuElements[i]->Render(renderWindow);
		menuElements[i]->Move(-screenCorrectionMoveVector);
	}
}

void MenuSystem::ResetSystemTweens()
{
	for(int i = 0; i < menuElements.size(); i++)
	{
		menuElements[i]->ResetTween();
	}
}

void MenuSystem::AddKeyboardSelectionButton(sf::Keyboard::Key selectionKey)
{
	keyboardSelectionButtons.push_back(selectionKey);
}

void MenuSystem::AddJoyStickSelectionButton(int gamePad, int buttonCode)
{
	joystickSelectionButtons.push_back(std::pair<int,int>(gamePad,buttonCode));
}

void MenuSystem::ClearKeyboardSelectionButtons()
{
	keyboardSelectionButtons.clear();
}

void MenuSystem::ClearJoyStickSelectionButtons()
{
	joystickSelectionButtons.clear();
}

//non toggleable button
void MenuSystem::AddMenuElement(float xPos, float yPos, std::string buttonImageRestingPath, std::string buttonImageSelectingPath, bool isSelectable, bool shouldTweenIn, MenuButton::TweenInDirection tweenDirection, float tweenBobAmount, float tweenSpeed, std::function<void()> onClickLogic)
{
	menuElements.push_back(std::unique_ptr<MenuButton>(new MenuButton(xPos, yPos, buttonImageRestingPath, buttonImageSelectingPath, isSelectable, shouldTweenIn, tweenDirection, tweenBobAmount, tweenSpeed, onClickLogic)));
}
//toggleable button
void MenuSystem::AddMenuElement(float xPos, float yPos, std::string buttonImageRestingPath, std::string buttonImageSelectingPath, std::string buttonImageToggledRestingPath, std::string buttonImageToggledSelectedPath, bool isSelectable, bool shouldTweenIn, MenuButton::TweenInDirection tweenDirection,  float tweenBobAmount, float tweenSpeed, std::function<void()> onClickLogic)
{
	menuElements.push_back(std::unique_ptr<MenuButton>(new MenuButton(xPos, yPos, buttonImageRestingPath, buttonImageSelectingPath, buttonImageToggledRestingPath, buttonImageToggledSelectedPath, isSelectable, shouldTweenIn, tweenDirection, tweenBobAmount, tweenSpeed, onClickLogic)));
}

void MenuSystem::ClearMenuElements()
{
	menuElements.clear();
}


void MenuSystem::HandleButtonMouseSelection(sf::Event events, bool eventFired, double deltaTime)
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

void MenuSystem::HandleButtonControllerSelection()
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
			candidateMoveButtons.resize(menuElements.size());
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
								if(isSoundOn)
								{
									moveBetweenButtonsSound.play();
								}
								return; //gotta break out or we'll move to the further out buttons as well
							}
						}
						else if(thisMove == MenuMovements::Left)
						{
							if(candidateMoveButtons[i].first->GetPosition().x < selectedButton->GetPosition().x)
							{
								//Move
								MoveToButton(candidateMoveButtons[i].first);
								if(isSoundOn)
								{
									moveBetweenButtonsSound.play();
								}
								return;
							}
						}
						else if(thisMove == MenuMovements::Up)
						{
							if(candidateMoveButtons[i].first->GetPosition().y < selectedButton->GetPosition().y)
							{
								//Move
								MoveToButton(candidateMoveButtons[i].first);
								if(isSoundOn)
								{
									moveBetweenButtonsSound.play();
								}
								return;
							}
						}
						else if(thisMove == MenuMovements::Down)
						{
							if(candidateMoveButtons[i].first->GetPosition().y > selectedButton->GetPosition().y)
							{
								//Move
								MoveToButton(candidateMoveButtons[i].first);
								if(isSoundOn)
								{
									moveBetweenButtonsSound.play();
								}
								return;
							}
						}
					}
				}
			}
		}
	}
}

void MenuSystem::HandleButtonActivation(sf::Event events, bool eventFired)
{
	//Check for activation of selected button with the mouse or keyboard
	if(selectedButton != nullptr)
	{
		if(eventFired)
		{
			if((events.type == sf::Event::MouseButtonPressed) || (events.type == sf::Event::KeyReleased))
			{
				//check for the keyboard selection(and mouse click)
				for(int i = 0; i < keyboardSelectionButtons.size(); i++)
				{
					if((events.mouseButton.button == sf::Mouse::Left) || (events.key.code == keyboardSelectionButtons[i]))
					{
						if(isSoundOn)
						{
							selectButtonSound.play();
						}
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
						if(isSoundOn)
						{
							selectButtonSound.play();
						}
						//Activate the button, it will toggle if it is a toggle button
						selectedButton->Activate();
					}
				}
			}
		}
	}
}

void MenuSystem::MoveToButton(MenuButton* button)
{
	if(button != nullptr)
	{
		selectedButton = button;
		canMoveButtonTimer.restart();
	}
}

void MenuSystem::MoveToFirstButton()
{
	for(int i = 0; i < menuElements.size(); i++)
	{
		if(menuElements[i]->IsSelectable())
		{
			MoveToButton(menuElements[i].get());
			break;
		}
	}
}

void MenuSystem::LoadMoveBetweenButtonSound(std::string soundPath)
{
	moveBetweenButtonSoundBuffer.loadFromFile(soundPath);
	moveBetweenButtonsSound.setBuffer(moveBetweenButtonSoundBuffer);
}

void MenuSystem::LoadSelectButtonSound(std::string soundPath)
{
	selectButtonSoundBuffer.loadFromFile(soundPath);
	selectButtonSound.setBuffer(selectButtonSoundBuffer);
}

void MenuSystem::RecheckIfVolumeIsOn()
{
	isSoundOn = interStateSingleton.GetIsVolumeOn();
}