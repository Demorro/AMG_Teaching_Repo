#pragma once
#include "SFML\Graphics.hpp"

class Player
{
public:
	Player(std::string playerTexturePath, sf::Vector2f startPos);
	~Player(void);

	void Update(sf::Event events, bool eventFired, double deltaTime);
	void Render(sf::RenderWindow &window);
	void Move(double x, double y);
	sf::Vector2f GetPosition();
	sf::Sprite &GetSprite();
	void SetPosition(sf::Vector2f position);
private:
	sf::Texture texture;
	sf::Sprite sprite;
	bool Initialise(std::string playerTexturePath, sf::Vector2f startPos);
};

