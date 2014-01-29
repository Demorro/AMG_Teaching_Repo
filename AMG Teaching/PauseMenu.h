#pragma once
#include <SFML\Graphics.hpp>
#include "MenuSystem.h"
#include "Camera.h"
#include <functional>
#include <memory>
#include "TweeningText.h"

class PauseMenu
{
public:
	PauseMenu(std::function<void()> ResumeFunction, std::function<void()> RestartFunction, std::function<void()> QuitFunction);
	~PauseMenu(void);

	void Update(sf::Event events, bool eventFired, double deltaTime);
	void Render(sf::RenderWindow &renderWindow, Camera &camera); //need to camera so we can always render in the center of the screen

	void ResetTweens();
	void MoveToFirstButton();
private:
	MenuSystem pauseMenuSystem;
};

