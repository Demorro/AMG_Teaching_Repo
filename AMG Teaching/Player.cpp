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
	//Set the origin to the center of the sprite
	sprite.setOrigin(sprite.getGlobalBounds().width/2,sprite.getGlobalBounds().height/2);
	sprite.setPosition(startPos);

	//load the control vectors up
	moveLeftKeys.push_back(sf::Keyboard::A);
	moveLeftKeys.push_back(sf::Keyboard::Left);
	moveRightKeys.push_back(sf::Keyboard::D);
	moveRightKeys.push_back(sf::Keyboard::Right);
	jumpKeys.push_back(sf::Keyboard::Space);
	jumpKeys.push_back(sf::Keyboard::Up);
	jumpKeys.push_back(sf::Keyboard::W);

	groundAcceleration = 400;
	maxGroundMovementSpeed = 550;
	personalGravity = 30;
	terminalVelocity = 600;
	jumpStrength = 900;

	return true;
}

void Player::Update(sf::Event events, bool eventFired, double deltaTime, std::vector<sf::Rect<float>> &levelCollisionRects)
{
	ReceiveControlInput(events,eventFired);
	HandleMovement(deltaTime, levelCollisionRects);
}

void Player::ReceiveControlInput(sf::Event events, bool eventFired)
{
	playerState.ResetInputs();

	//loop through all the keyboard inputs, check, and update the state accordingly
	//Left
	for(int i = 0; i < moveLeftKeys.size(); i++)
	{
		if(sf::Keyboard::isKeyPressed(moveLeftKeys[i]))
		{
			playerState.INPUT_MoveLeft = true;
		}
	}
	//Right
	for(int i = 0; i < moveRightKeys.size(); i++)
	{
		if(sf::Keyboard::isKeyPressed(moveRightKeys[i]))
		{
			playerState.INPUT_MoveRight = true;
		}
	}
	//Jump
	for(int i = 0; i < jumpKeys.size(); i++)
	{
		if(sf::Keyboard::isKeyPressed(jumpKeys[i]))
		{
			playerState.INPUT_Jump = true;
		}
	}
}

void Player::HandleMovement(float deltaTime, std::vector<sf::Rect<float>> &levelCollisionRects)
{
	playerState.velocity.x = 0;
	//Update the velocity
	//Left and right movement
	if(playerState.INPUT_MoveLeft)
	{
		playerState.velocity.x = -maxGroundMovementSpeed;
		//flip the sprite to face left
		sprite.setScale(-1.0f,1.0f);
	}
	if(playerState.INPUT_MoveRight)
	{
		playerState.velocity.x = maxGroundMovementSpeed;
		//flip the sprite to face right
		sprite.setScale(1.0f,1.0f);
	}
	//Move horizontal
	Move(playerState.velocity.x * deltaTime,0);

	HandleHorizontalCollision(levelCollisionRects);

	//Jump
	if(playerState.INPUT_Jump)
	{
		//you can only jump if you're grounded, or if you're in the grace period after falling off a ledge
		if(playerState.grounded == true) 
		{
			playerState.velocity.y -= jumpStrength;
			playerState.grounded = false;
		}
	}


	//Add gravity
	if(playerState.velocity.y + personalGravity < terminalVelocity)
	{
		playerState.velocity.y += personalGravity;
	}
	else
	{
		playerState.velocity.y = terminalVelocity;
	}

	//if you're falling, you're not grounded
	if(playerState.velocity.y > 0)
	{

		playerState.grounded = false;
	}

	//Move vertical
	Move(0, playerState.velocity.y * deltaTime);

	HandleVerticalCollision(levelCollisionRects);

}

void Player::HandleHorizontalCollision(std::vector<sf::Rect<float>> &levelCollisionRects)
{
	for(int i = 0; i < levelCollisionRects.size(); i++)
	{
		if(GetCollider().intersects(levelCollisionRects[i]))
		{
			//we're going right, snap to lefts of object
			if(playerState.velocity.x > 0)
			{
				SetPosition(levelCollisionRects[i].left - (GetCollider().width/2),GetPosition().y);
				playerState.velocity.x = 0;
			}
			//we're going left, snap to right of object
			else if(playerState.velocity.x < 0)
			{
				SetPosition((levelCollisionRects[i].left + levelCollisionRects[i].width) + (GetCollider().width/2),GetPosition().y);
				playerState.velocity.x = 0;
			}
		}
	}
}

void Player::HandleVerticalCollision(std::vector<sf::Rect<float>> &levelCollisionRects)
{
	for(int i = 0; i < levelCollisionRects.size(); i++)
	{
		if(GetCollider().intersects(levelCollisionRects[i]))
		{
			//we're going down, snap to top of object
			if(playerState.velocity.y > 0)
			{
				SetPosition(GetPosition().x, levelCollisionRects[i].top - (GetCollider().height/2));
				playerState.grounded = true;
				playerState.velocity.y = 0;
			}
			//we're going up, snap to bottom of object
			else if(playerState.velocity.y < 0)
			{
				SetPosition(GetPosition().x, (levelCollisionRects[i].top + levelCollisionRects[i].height) + (GetCollider().height/2));
				playerState.grounded = false;
				playerState.velocity.y = 0;
			}
			//wtf mate? just snap to top
			else
			{
				SetPosition(GetPosition().x, levelCollisionRects[i].top - (GetCollider().height/2));
				playerState.grounded = true;
				playerState.velocity.y = 0;
			}
		}
	}
}

void Player::Render(sf::RenderWindow &window)
{
	window.draw(sprite);
}

void Player::Move(float x, float y)
{
	sf::Vector2f movementChange(x,y);
	sprite.move(movementChange);
}

void Player::SetPosition(float xPos, float yPos)
{
	sprite.setPosition(xPos,yPos);
}

sf::Vector2f Player::GetPosition()
{
	return sprite.getPosition();
}

void Player::SetPosition(sf::Vector2f position)
{
	sprite.setPosition(position);
}

sf::FloatRect Player::GetCollider()
{
	return sprite.getGlobalBounds();
}