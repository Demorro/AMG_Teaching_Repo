#pragma once
#include "state.h"
#include "Assets.h"
#include "MotionSpline.h"
#include <memory>


class MenuState : public State
{
public:
	MenuState(void);
	~MenuState(void);

	/** 
	 * Loads MenuState content.
	 * Returns true if successful, otherwise returns false.
	 */
	bool Load();
	
	/** Main loop. */
	void Update(sf::Event events, bool eventFired, double deltaTime);

	/** Renders all of the applicable state content. */
	void Draw(sf::RenderWindow &renderWindow);

private:
	sf::RectangleShape fadeRectangle;
	bool shouldFadeIn;
	bool shouldFadeOut;

	sf::Texture backgroundImage;
	sf::Sprite backGroundSprite;

	bool shouldTweenInMenuElements;

	sf::Texture gameLogoTexture;
	sf::Sprite gameLogoSprite;
	std::unique_ptr<MotionSpline> initialLogoTween;
	float logoTweenProgressIterator;
	float logoTweenSpeed;

	sf::Texture startButtonTexture;
	sf::Texture startButtonSelectedTexture;
	bool startButtonIsSelected;
	sf::Sprite startButtonSprite;
	std::unique_ptr<MotionSpline> initialStartButtonTween;
	float startButtonTweenProgressIterator;
	float startButtonTweenSpeed;

};

