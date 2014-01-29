#pragma once
#include "SFML\Graphics.hpp"
#include "MotionSpline.h"
#include <memory>
#include <functional>
#include <Windows.h>
#include <sstream>
#include "TweenableElement.h"


class MenuButton : public TweenableElement
{
public:
	MenuButton(float xPos, float yPos, std::string buttonImageRestingPath, std::string buttonImageSelectingPath, bool isSelectable = true, bool shouldTweenIn = true, std::function<void()> onClickLogic = nullptr);
	MenuButton(float xPos, float yPos, std::string buttonImageRestingPath, std::string buttonImageSelectingPath, std::string buttonImageToggledRestingPath, std::string buttonImageToggledSelectedPath, bool isSelectable = true, bool shouldTweenIn = true, std::function<void()> onClickLogic = nullptr);
	~MenuButton(void);

	void Update(sf::Event events, bool eventFired, double deltaTime);
	void Render(sf::RenderWindow &window);

	void Move(sf::Vector2f movement);

	void SetSelected(bool isSelected);
	void Activate();
	bool IsAToggleButton();
	sf::Rect<float> GetButtonBounds();

	bool IsSelectable();
	sf::Vector2f GetPosition();

private:
	sf::Sprite buttonSprite;
	std::function<void()> onButtonActivationFunction;

	bool isSelectable;

	sf::Texture buttonImageResting;
	sf::Texture buttonImageSelected;
	bool isCurrentlySelected;
	//For toggle buttons
	bool isToggleButton;
	bool buttonIsToggled;
	sf::Texture buttonImageToggledResting;
	sf::Texture buttonImageToggledSelected;

	void LoadNonToggleableValues(float xPos, float yPos, std::string buttonImageRestingPath, std::string buttonImageSelectingPath, bool isSelectable);
};

