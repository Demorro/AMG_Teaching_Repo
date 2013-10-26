#include "Player.h"


Player::Player(std::string playerTexturePath, sf::Vector2f startPos)
{
	Initialise(playerTexturePath, startPos);
}


Player::~Player(void)
{
}

bool Player::Initialise(std::string playerTexturePath, sf::Vector2f startPos)
{
	if(!texture.loadFromFile(playerTexturePath))
	{
		return false;
	}
	sprite.setTexture(texture);
	sprite.setOrigin(sprite.getGlobalBounds().width/2,sprite.getGlobalBounds().height/2);
	sprite.setPosition(startPos);

	return true;
}

void Player::Update(sf::Event events, bool eventFired, double deltaTime)
{
}
void Player::Render(sf::RenderWindow &window)
{
	window.draw(sprite);
}

sf::Sprite &Player::GetSprite()
{
	return sprite;
}

void Player::Move(double x, double y)
{
	sprite.move(x,y);
}

sf::Vector2f Player::GetPosition()
{
	return sprite.getPosition();
}

void Player::SetPosition(sf::Vector2f position)
{
	sprite.setPosition(position);
}
