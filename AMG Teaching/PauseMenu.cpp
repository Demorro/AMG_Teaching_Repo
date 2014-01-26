#include "PauseMenu.h"
#include "Application.h"

PauseMenu::PauseMenu(std::function<void()> ResumeFunction, std::function<void()> RestartFunction, std::function<void()> QuitFunction)
{
	float menuElementsBobAmount = 200.0f;
	float menuElementsTweenSpeed = 1.5f;

	pauseMenuSystem.AddMenuElement(Application::GetWindow().getSize().x/2, Application::GetWindow().getSize().y/2, PAUSEMENUFRAME, PAUSEMENUFRAME, false, true, MenuButton::TweenInDirection::Top, menuElementsBobAmount, menuElementsTweenSpeed);

	float resumeButtonYFromCenter = -120;
	pauseMenuSystem.AddMenuElement(Application::GetWindow().getSize().x/2, Application::GetWindow().getSize().y/2 + resumeButtonYFromCenter, PAUSERESUMEBUTTON, PAUSERESUMEBUTTONSELECTED, true, true, MenuButton::TweenInDirection::Left, menuElementsBobAmount, menuElementsTweenSpeed, ResumeFunction);

	float restartButtonYFromCenter = 0;
	pauseMenuSystem.AddMenuElement(Application::GetWindow().getSize().x/2, Application::GetWindow().getSize().y/2 + restartButtonYFromCenter, PAUSERESTARTBUTTON, PAUSERESTARTBUTTONSELECTED, true, true, MenuButton::TweenInDirection::Right, menuElementsBobAmount, menuElementsTweenSpeed, RestartFunction);
	
	float quitButtonYFromCenter = 120;
	pauseMenuSystem.AddMenuElement(Application::GetWindow().getSize().x/2, Application::GetWindow().getSize().y/2 + quitButtonYFromCenter, PAUSEQUITBUTTON, PAUSEQUITBUTTONSELECTED, true, true, MenuButton::TweenInDirection::Left, menuElementsBobAmount, menuElementsTweenSpeed, QuitFunction);

	pauseMenuSystem.MoveToFirstButton();
}


PauseMenu::~PauseMenu(void)
{
}

void PauseMenu::Update(sf::Event events, bool eventFired, double deltaTime)
{
	pauseMenuSystem.Update(events,eventFired,deltaTime);
}

void PauseMenu::Render(sf::RenderWindow &renderWindow,  Camera &camera)
{
	pauseMenuSystem.Render(renderWindow,camera);
}

void PauseMenu::ResetTweens()
{
	pauseMenuSystem.ResetSystemTweens();
}

void PauseMenu::MoveToFirstButton()
{
	pauseMenuSystem.MoveToFirstButton();
}
