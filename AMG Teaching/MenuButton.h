#pragma once
#include "SFML\Graphics.hpp"
#include "MotionSpline.h"
#include <memory>
#include <functional>
#include <Windows.h>
#include <sstream>


class MenuButton
{
public:

	enum TweenInDirection
	{
		Top,
		Bottom,
		Left,
		Right
	};

	MenuButton(float xPos, float yPos, std::string buttonImageRestingPath, std::string buttonImageSelectingPath, bool isSelectable = true, bool shouldTweenIn = false, TweenInDirection tweenDirection = TweenInDirection::Top,  float tweenBobAmount = 100.0f, float tweenSpeed = 1.0f, std::function<void()> onClickLogic = nullptr);
	MenuButton(float xPos, float yPos, std::string buttonImageRestingPath, std::string buttonImageSelectingPath, std::string buttonImageToggledRestingPath, std::string buttonImageToggledSelectedPath, bool isSelectable = true, bool shouldTweenIn = false, TweenInDirection tweenDirection = TweenInDirection::Top,  float tweenBobAmount = 100.0f, float tweenSpeed = 1.0f, std::function<void()> onClickLogic = nullptr);
	~MenuButton(void);

	void Update(sf::Event events, bool eventFired, double deltaTime);
	void Render(sf::RenderWindow &window);

	void Move(sf::Vector2f movement);

	void ResetTween();
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

	//For tweening in
	bool shouldTweenIn;
	std::unique_ptr<MotionSpline> tweenInSpline;
	TweenInDirection tweenInDirection; 
	float tweenInSpeed;
	float tweenProgressIterator; //Iterate from 0 to 1 for the tween

	void DoTweenLogic(double deltaTime);
	void LoadNonToggleableValues(float xPos, float yPos, std::string buttonImageRestingPath, std::string buttonImageSelectingPath, bool isSelectable, bool shouldTweenIn, TweenInDirection tweenDirection, float tweenBobAmount, float tweenSpeed);
};

