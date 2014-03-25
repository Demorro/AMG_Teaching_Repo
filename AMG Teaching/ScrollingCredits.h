#pragma once
#include "Assets.h"
#include <SFML\Graphics.hpp>
#include "XMLParseUtilities.h"

class ScrollingCredits
{
public:
	ScrollingCredits(void);
	~ScrollingCredits(void);

	void Update(float deltaTime);
	void Render(sf::RenderWindow &window);

	bool ShouldSwitchOutOfCreditsState();

private:
	void LoadCreditsText(std::string creditsConfigPath);
	void LoadStringIntoRenderableStrings(std::string stringToLoad);
	void AllignRenderableCreditStrings();

	std::string creditString;
	std::vector<sf::Text> renderableStrings;
	sf::Font creditsFont;

	sf::Vector2f startPos;

	sf::Clock timeToStayInCreditsClock;

	float textSize;
	float scrollSpeed;
	float lineGap;
	float timeToStayInCredits;

	bool creditsTimeHasPassed;
};

