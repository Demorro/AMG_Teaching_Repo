#pragma once

#include <SFML\Graphics.hpp>
#include "MenuButton.h"
#include "XBoxButtons.h"
#include "Camera.h"
#include "InterStateSingleTon.h"
#include <memory>

class MenuSystem
{
public:
	MenuSystem(void);
	~MenuSystem(void);
	
	void Update(sf::Event events, bool eventFired, double deltaTime);

	void Render(sf::RenderWindow &renderWindow);
	void Render(sf::RenderWindow &renderWindow, Camera &camera); //The camera overload centers the sprites in the center of the screen

	void AddKeyboardSelectionButton(sf::Keyboard::Key selectionKey);
	void AddJoyStickSelectionButton(int gamePad, int buttonCode);
	void ClearKeyboardSelectionButtons();
	void ClearJoyStickSelectionButtons();

	void ResetSystemTweens();

	//An AddMenuButtonsfor each type of element
	//Nontoggleablebutton
	void AddMenuButton(float xPos, float yPos, std::string buttonImageRestingPath, std::string buttonImageSelectingPath, bool isSelectable = true, bool shouldTweenIn = false, MenuButton::TweenInDirection tweenDirection = MenuButton::TweenInDirection::Top,  float tweenBobAmount = 100.0f, float tweenSpeed = 1.0f, std::function<void()> onClickLogic = nullptr);
	//toggleable buttons
	void AddMenuButtonToggleable(float xPos, float yPos, std::string buttonImageRestingPath, std::string buttonImageSelectingPath, std::string buttonImageToggledRestingPath, std::string buttonImageToggledSelectedPath, bool isSelectable = true, bool shouldTweenIn = false, MenuButton::TweenInDirection tweenDirection = MenuButton::TweenInDirection::Top,  float tweenBobAmount = 100.0f, float tweenSpeed = 1.0f, std::function<void()> onClickLogic = nullptr);
	void ClearMenuElements();

	//Sets the selection to be the first button we got, so we can start selected.
	void MoveToFirstButton();

	void LoadMoveBetweenButtonSound(std::string soundPath);
	void LoadSelectButtonSound(std::string soundPath);

	//because we cache whether the volume is on or not, you need to sometimes recheck it when you turn it on/off so the menusounds start playing in that state
	void RecheckIfVolumeIsOn();

private:
	//buttons that can be used to hit a menu button
	std::vector<sf::Keyboard::Key> keyboardSelectionButtons;
	//The first int is the joy, 0 normally for player 1, while the second is the keycoke
	std::vector<std::pair<int,int>> joystickSelectionButtons;
	
	std::vector<std::unique_ptr<MenuButton>> menuElements;
	MenuButton* selectedButton;

	//Need a timer, else we'll move through all the buttons all at once when we hit left of right
	sf::Clock canMoveButtonTimer;
	float timeTillCanMoveButton;

	void HandleButtonMouseSelection(sf::Event events, bool eventFired, double deltaTime);
	void HandleButtonControllerSelection();
	void HandleButtonActivation(sf::Event events, bool eventFired);
	void MoveToButton(MenuButton* button);

	//Sounds
	sf::SoundBuffer moveBetweenButtonSoundBuffer;
	sf::SoundBuffer selectButtonSoundBuffer;
	sf::Sound moveBetweenButtonsSound;
	sf::Sound selectButtonSound;

	bool isSoundOn;
};

