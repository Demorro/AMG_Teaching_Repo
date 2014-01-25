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

	if(!backgroundImage.loadFromFile(MENUBACKGROUND)){};
	backGroundSprite.setTexture(backgroundImage);

	float menuElementsBobAmount = 200.0f;
	float menuElementsTweenSpeed = 0.9f;

	float titleDistanceFromTop = 125;
	mainMenuSystem.AddMenuElement(Application::GetWindow().getSize().x/2, titleDistanceFromTop, GAMELOGO, GAMELOGO, false, true, MenuButton::TweenInDirection::Top, menuElementsBobAmount, menuElementsTweenSpeed);

	float startButtonDistanceFromBottom = 150;
	mainMenuSystem.AddMenuElement(Application::GetWindow().getSize().x/2, Application::GetWindow().getSize().y - startButtonDistanceFromBottom, STARTBUTTON, STARTBUTTONSELECTED, true, true, MenuButton::TweenInDirection::Bottom, menuElementsBobAmount, menuElementsTweenSpeed, std::function<void()>(std::bind(&MenuState::GoToFirstLevelState,this)));

	float quitButtonDistanceFromBottom = 60;
	mainMenuSystem.AddMenuElement(Application::GetWindow().getSize().x/2, Application::GetWindow().getSize().y - quitButtonDistanceFromBottom, QUITBUTTON, QUITBUTTONSELECTED, true, true, MenuButton::TweenInDirection::Bottom, menuElementsBobAmount, menuElementsTweenSpeed, std::function<void()>(std::bind(&MenuState::QuitApplication,this)));

	float controlButtonDistanceFromBottom = 110;
	float controlButtonDistanceFromCenterX = -500;
	mainMenuSystem.AddMenuElement(Application::GetWindow().getSize().x/2 + controlButtonDistanceFromCenterX, Application::GetWindow().getSize().y - controlButtonDistanceFromBottom, CONTROLBUTTONNORMAL, CONTROLBUTTONSELECTED, true, true, MenuButton::TweenInDirection::Bottom, menuElementsBobAmount, menuElementsTweenSpeed, std::function<void()>(std::bind(&MenuState::GoToControlsState,this)));

	float volumeButtonDistanceFromBottom = 110;
	float volumeButtonDistanceFromCenterX = 500;
	mainMenuSystem.AddMenuElement(Application::GetWindow().getSize().x/2 + volumeButtonDistanceFromCenterX, Application::GetWindow().getSize().y - volumeButtonDistanceFromBottom, VOLUMEBUTTONONNORMAL, VOLUMEBUTTONONSELECTED, VOLUMEBUTTONOFFNORMAL, VOLUMEBUTTONOFFSELECTED, true, true, MenuButton::TweenInDirection::Bottom, menuElementsBobAmount, menuElementsTweenSpeed, std::function<void()>(std::bind(&MenuState::ToggleVolume,this)));

	mainMenuSystem.MoveToFirstButton();

	return true;
}

void MenuState::Update(sf::Event events, bool eventFired, double deltaTime)
{
	mainMenuSystem.Update(events,eventFired,deltaTime);
}

void MenuState::Draw(sf::RenderWindow &renderWindow)
{
	//Draw things here
	renderWindow.draw(backGroundSprite);

	mainMenuSystem.Render(renderWindow);

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




