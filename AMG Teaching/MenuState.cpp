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
	isOnLevelSelectStage = false;

	LoadMenuAudio(AUDIOCONFIG);
	
	if(!backgroundImage.loadFromFile(MENUBACKGROUND)){};
	backGroundSprite = std::unique_ptr<BigSprite>(new BigSprite(backgroundImage));
	backGroundSprite->move(Application::GetWindow().getSize().x/2 - backGroundSprite->GetSize().x/2,Application::GetWindow().getSize().y/2 - backGroundSprite->GetSize().y/2);
	

	float menuElementsBobAmount = 150.0f;
	float menuElementsTweenSpeed = 1.0f;
	
	//Load the buttons in the main menu system
	float titleDistanceFromTop = 125;
	mainMenuSystem.AddMenuButton(Application::GetWindow().getSize().x/2, titleDistanceFromTop, GAMELOGO, GAMELOGO, false, true, MenuButton::TweenInDirection::Top, menuElementsBobAmount, menuElementsTweenSpeed);

	float startButtonDistanceFromCenter = -50;
	mainMenuSystem.AddMenuButton(Application::GetWindow().getSize().x/2, (Application::GetWindow().getSize().y/2) + startButtonDistanceFromCenter, STARTBUTTON, STARTBUTTONSELECTED, true, true, MenuButton::TweenInDirection::Bottom, menuElementsBobAmount, menuElementsTweenSpeed, std::function<void()>(std::bind(&MenuState::GoToFirstLevelState,this)));

	float levelSelectButtonDistanceFromCenter = 100;
	mainMenuSystem.AddMenuButton(Application::GetWindow().getSize().x/2, (Application::GetWindow().getSize().y/2) + levelSelectButtonDistanceFromCenter, LEVELSELECTBUTTONUNSELECTED, LEVELSELECTBUTTONSELECTED, true, true, MenuButton::TweenInDirection::Bottom, menuElementsBobAmount, menuElementsTweenSpeed, std::function<void()>(std::bind(&MenuState::GoToLevelSelect,this)));

	float quitButtonDistanceFromCenter = 230;
	mainMenuSystem.AddMenuButton(Application::GetWindow().getSize().x/2, (Application::GetWindow().getSize().y/2) + quitButtonDistanceFromCenter, QUITBUTTON, QUITBUTTONSELECTED, true, true, MenuButton::TweenInDirection::Bottom, menuElementsBobAmount, menuElementsTweenSpeed, std::function<void()>(std::bind(&MenuState::QuitApplication,this)));

	float controlButtonDistanceFromBottom = 110;
	float controlButtonDistanceFromCenterX = -500;
	mainMenuSystem.AddMenuButton(Application::GetWindow().getSize().x/2 + controlButtonDistanceFromCenterX, Application::GetWindow().getSize().y - controlButtonDistanceFromBottom, CONTROLBUTTONNORMAL, CONTROLBUTTONSELECTED, true, true, MenuButton::TweenInDirection::Bottom, menuElementsBobAmount, menuElementsTweenSpeed, std::function<void()>(std::bind(&MenuState::GoToControlsState,this)));

	float volumeButtonDistanceFromBottom = 110;
	float volumeButtonDistanceFromCenterX = 500;
	if(interStateSingleton.GetIsVolumeOn() == false) //Not sure if supremely clever or dumb as fuck. If you come back into the main state and the volume is off, then switch around the on/off images so the icon appears correct. Seems to work
	{
		mainMenuSystem.AddMenuButtonToggleable(Application::GetWindow().getSize().x/2 + volumeButtonDistanceFromCenterX, Application::GetWindow().getSize().y - volumeButtonDistanceFromBottom, VOLUMEBUTTONOFFNORMAL, VOLUMEBUTTONOFFSELECTED, VOLUMEBUTTONONNORMAL, VOLUMEBUTTONONSELECTED, true, true, MenuButton::TweenInDirection::Bottom, menuElementsBobAmount, menuElementsTweenSpeed, std::function<void()>(std::bind(&MenuState::ToggleVolume,this)));
	}
	else
	{
		mainMenuSystem.AddMenuButtonToggleable(Application::GetWindow().getSize().x/2 + volumeButtonDistanceFromCenterX, Application::GetWindow().getSize().y - volumeButtonDistanceFromBottom, VOLUMEBUTTONONNORMAL, VOLUMEBUTTONONSELECTED, VOLUMEBUTTONOFFNORMAL, VOLUMEBUTTONOFFSELECTED, true, true, MenuButton::TweenInDirection::Bottom, menuElementsBobAmount, menuElementsTweenSpeed, std::function<void()>(std::bind(&MenuState::ToggleVolume,this)));
	}

	mainMenuSystem.MoveToFirstButton();
	//
	//Load the buttons in the level select system
	float scotlandLevelDistanceFromCenter = -180;
	levelSelectSystem.AddMenuButton(Application::GetWindow().getSize().x/2, (Application::GetWindow().getSize().y/2) + scotlandLevelDistanceFromCenter, SCOTLANDUNSELECTED, SCOTLANDSELECTED, true, true, MenuButton::TweenInDirection::Bottom, menuElementsBobAmount, menuElementsTweenSpeed, std::function<void()>(std::bind(&MenuState::GoToFirstLevelState,this)));

	float mexicoLevelDistanceFromCenter = 0;
	levelSelectSystem.AddMenuButton(Application::GetWindow().getSize().x/2, (Application::GetWindow().getSize().y/2) + mexicoLevelDistanceFromCenter, MEXICOUNSELECTED, MEXICOSELECTED, true, true, MenuButton::TweenInDirection::Bottom, menuElementsBobAmount, menuElementsTweenSpeed, std::function<void()>(std::bind(&MenuState::GoToSecondLevelState,this)));

	float japanDistanceFromCenter = 180;
	levelSelectSystem.AddMenuButton(Application::GetWindow().getSize().x/2, (Application::GetWindow().getSize().y/2) + japanDistanceFromCenter, JAPANUNSELECTED, JAPANSELECTED, true, true, MenuButton::TweenInDirection::Bottom, menuElementsBobAmount, menuElementsTweenSpeed, std::function<void()>(std::bind(&MenuState::GoToThirdLevelState,this)));

	float backToMenuButtonDistanceFromCenter = -500.0f;
	float backToMenuButtonDistanceFromBottom = 100.0f;
	levelSelectSystem.AddMenuButton(Application::GetWindow().getSize().x/2 + backToMenuButtonDistanceFromCenter, Application::GetWindow().getSize().y - backToMenuButtonDistanceFromBottom, BACKTOMENUNORMAL, BACKTOMENUSELECTED, true, true, MenuButton::TweenInDirection::Left, menuElementsBobAmount, menuElementsTweenSpeed, std::function<void()>(std::bind(&MenuState::ComeBackFromLevelSelect,this)));

	levelSelectSystem.MoveToFirstButton();
	return true;
}

void MenuState::LoadMenuAudio(std::string audioConfigFilePath)
{
	std::string menuMusicPathNodeName = "MenuMusicPath";
	std::string menuMusicVolumeNodeName = "MenuMusicVolume";

	std::string menuMusicPath = MENUBACKINGMUSIC;
	menuVolume = 100;

	pugi::xml_document configDoc;
	LoadXMLDoc(configDoc,audioConfigFilePath);

	//Work through all the variables we need to load and load em, checking for if they're there or not each time
	//TODO : This whole things needs refactoring into a generic loader function, this violates DRY like mad.
	pugi::xml_node rootNode = configDoc.child("AudioConfig");

	LoadTextValue(menuMusicPath,rootNode,menuMusicPathNodeName);
	LoadNumericalValue(menuVolume,rootNode,menuMusicVolumeNodeName);

	//Start the music, it's loaded in the singleton
	if(interStateSingleton.InterStateMusicIsPlaying() == false)
	{
		interStateSingleton.AdjustInterStateMusicVolume(menuVolume);
		interStateSingleton.LoadInterStateMusicFile(menuMusicPath);
		interStateSingleton.SetInterStateMusicLooping(true);
		interStateSingleton.PlayInterStateMusic();
	}
}

void MenuState::Update(sf::Event events, bool eventFired, double deltaTime)
{
	if(isOnLevelSelectStage)
	{
		levelSelectSystem.Update(events,eventFired,deltaTime);
	}
	else
	{
		mainMenuSystem.Update(events,eventFired,deltaTime);
	}
}

void MenuState::Draw(sf::RenderWindow &renderWindow)
{
	//Draw things here
	renderWindow.draw(*backGroundSprite);

	if(isOnLevelSelectStage)
	{
		levelSelectSystem.Render(renderWindow);
	}
	else
	{
		mainMenuSystem.Render(renderWindow);
	}

}


//These functions are passed to the buttons and executed upon clickety clackity click
void MenuState::ToggleVolume()
{
	if(interStateSingleton.GetIsVolumeOn())
	{
		interStateSingleton.SetIsVolumeOn(false);
		interStateSingleton.AdjustInterStateMusicVolume(0);
	}
	else
	{
		interStateSingleton.SetIsVolumeOn(true);
		interStateSingleton.AdjustInterStateMusicVolume(menuVolume);
	}

	mainMenuSystem.RecheckIfVolumeIsOn();
}

void MenuState::GoToFirstLevelState()
{
	interStateSingleton.StopInterStateMusic();
	SwitchState(State::PROLOGUE_STATE);
}

void MenuState::GoToSecondLevelState()
{
	interStateSingleton.StopInterStateMusic();
	SwitchState(State::LEVEL2_STATE);
}

void MenuState::GoToThirdLevelState()
{
	interStateSingleton.StopInterStateMusic();
	SwitchState(State::LEVEL3_STATE);
}

void MenuState::QuitApplication()
{
	Application::SetRunning(false);
}

void MenuState::GoToControlsState()
{
	SwitchState(State::CONTROL_STATE);
}

void MenuState::GoToLevelSelect()
{
	isOnLevelSelectStage = true;
	levelSelectSystem.ResetSystemTweens();
	levelSelectSystem.MoveToFirstButton();
}

void MenuState::ComeBackFromLevelSelect()
{
	isOnLevelSelectStage = false;
	mainMenuSystem.ResetSystemTweens();
	mainMenuSystem.MoveToFirstButton();
}



