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

	//Read the player movement variables from the config files, or if it cant be found load defaults
	LoadConfigValues(PLAYERCONFIG);

	return true;
}

bool Player::LoadConfigValues(std::string configFilePath)
{
	//set the default move values, in case the loading dosent work these will be used
	groundAcceleration = 0.45f;
	airAcceleration = 0.3f;
	maximumHorizontalSpeed = 10;
	groundDrag = 0.65f;
	airDrag = 0.0f;
	personalGravity = 0.6f;
	terminalVelocity = 15;
	jumpStrength = 14;

	pugi::xml_document configDoc;

	//Load the config file into memory
	std::cout << "Loading Player Config File : " << configFilePath << std::endl;
	pugi::xml_parse_result result = configDoc.load_file(configFilePath.c_str());
	if (result)
	{
		std::cout << "XML Player Config File [" << configFilePath << "] was loaded without errors." << std::endl;
	}
	else
	{
		std::cout << "XML Player Config File [" << configFilePath << "] had some trouble loading and failed, using default player values" << std::endl;
		std::cout << "Error description: " << result.description() << "\n";

		return false;
	}

	//Work through all the variables we need to load and load em, checking for if they're there or not each time
	pugi::xml_node rootNode = configDoc.child("PlayerConfig");
	//Get MaximumHorizontalSpeed
	pugi::xml_node workingNode = rootNode.child("MaximumHorizontalSpeed");
	if(workingNode)
	{
		std::stringstream horizontalSpeedStream(workingNode.child_value());
		horizontalSpeedStream >> maximumHorizontalSpeed;
	}
	else
	{
		std::cout << "Couldn't find MaximumHorizontalSpeed in config file, using default" << std::endl;
	}

	//Get GroundAcceleration
	workingNode = rootNode.child("GroundAcceleration");
	if(workingNode)
	{
		std::stringstream groundAccelerationStream(workingNode.child_value());
		groundAccelerationStream >> groundAcceleration;
	}
	else
	{
		std::cout << "Couldn't find GroundAcceleration in config file, using default" << std::endl;
	}

	//Get AirAcceleration
	workingNode = rootNode.child("AirAcceleration");
	if(workingNode)
	{
		std::stringstream airAccelerationStream(workingNode.child_value());
		airAccelerationStream >> airAcceleration;
	}
	else
	{
		std::cout << "Couldn't find AirAcceleration in config file, using default" << std::endl;
	}

	//Get GroundDrag
	workingNode = rootNode.child("GroundDrag");
	if(workingNode)
	{
		std::stringstream groundDragStream(workingNode.child_value());
		groundDragStream >> groundDrag;
	}
	else
	{
		std::cout << "Couldn't find GroundDrag in config file, using default" << std::endl;
	}

	//Get AirDrag
	workingNode = rootNode.child("AirDrag");
	if(workingNode)
	{
		std::stringstream airDragStream(workingNode.child_value());
		airDragStream >> airDrag;
	}
	else
	{
		std::cout << "Couldn't find AirDrag in config file, using default" << std::endl;
	}

	//Get TerminalVelocity
	workingNode = rootNode.child("TerminalVelocity");
	if(workingNode)
	{
		std::stringstream terminalVelocityStream(workingNode.child_value());
		terminalVelocityStream >> terminalVelocity;
	}
	else
	{
		std::cout << "Couldn't find TerminalVelocity in config file, using default" << std::endl;
	}

	//Get PersonalGravity
	workingNode = rootNode.child("PersonalGravity");
	if(workingNode)
	{
		std::stringstream personalGravityStream(workingNode.child_value());
		personalGravityStream >> personalGravity;
	}
	else
	{
		std::cout << "Couldn't find PersonalGravity in config file, using default" << std::endl;
	}

	//Get JumpStrength
	workingNode = rootNode.child("JumpStrength");
	if(workingNode)
	{
		std::stringstream jumpStrengthStream(workingNode.child_value());
		jumpStrengthStream >> jumpStrength;
	}
	else
	{
		std::cout << "Couldn't find JumpStrength in config file, using default" << std::endl;
	}

	if(DEBUGPLAYER)
	{
		std::cout << "PLAYER MOVEMENT VALUES : " << std::endl;
		std::cout << "MaxHorizontalSpeed : " << maximumHorizontalSpeed << std::endl;
		std::cout << "GroundAcceleration : " << groundAcceleration << std::endl;
		std::cout << "AirAcceleration : " << airAcceleration << std::endl;
		std::cout << "GroundDrag : " << groundDrag << std::endl;
		std::cout << "AirDrag : " << airDrag << std::endl;
		std::cout << "TerminalVelocity : " << terminalVelocity << std::endl;
		std::cout << "PersonalGravity : " << personalGravity << std::endl;
		std::cout << "JumpStrength : " << jumpStrength << std::endl;
	}

	return true;
}

void Player::Update(sf::Event events, bool eventFired, double deltaTime, std::vector<sf::Rect<float>> &levelCollisionRects)
{
	//Receiving input is done seperate from the movement because ... well because I think it's cleaner, no other real reason.
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
	//update the player velocity to move left and right depending on player input
	DoLeftAndRightMovement(deltaTime);
	//Add the drag horizontally, different whether you are grounded or not
	AddDrag(deltaTime);
	//Move horizontal
	Move(playerState.velocity.x * deltaTime ,0);
	//Check to see that the new position is valid horizontally
	HandleHorizontalCollision(levelCollisionRects);
	//Deal wid jumping 
	DoJumping();
	//Add gravity
	AddGravity(deltaTime);
	//Move vertical
	Move(0, playerState.velocity.y * deltaTime);
	//Check to see if the new position is valid vertically
	HandleVerticalCollision(levelCollisionRects);
}
//All these functions are just used in HandleMovement, seperated because its cleaner and easier to make logic changes
void Player::DoLeftAndRightMovement(float deltaTime)
{
	//Left and right movement
	if((!playerState.INPUT_MoveLeft) || (!playerState.INPUT_MoveRight)) //Dont execute if both left and right are held
	{
		//Use ground acceleration value if we're on the floor
		if(playerState.grounded)
		{
			if(playerState.INPUT_MoveLeft)
			{
				//We dont want to accelerate over our max speed, so check
				if((playerState.velocity.x - (groundAcceleration * deltaTime)) > -maximumHorizontalSpeed)
				{
					playerState.velocity.x = playerState.velocity.x - (groundAcceleration * deltaTime);
				}
				else
				{
					playerState.velocity.x = -maximumHorizontalSpeed;
				}
				//flip the sprite to face left
				sprite.setScale(-1.0f,1.0f);
			}
			if(playerState.INPUT_MoveRight)
			{
				//We dont want to accelerate over our max speed, so check
				if((playerState.velocity.x + (groundAcceleration * deltaTime)) < maximumHorizontalSpeed)
				{
					//you multiply in deltatime here so acceleration is also constant
					playerState.velocity.x = playerState.velocity.x + (groundAcceleration * deltaTime);
				}
				else
				{
					playerState.velocity.x = maximumHorizontalSpeed;
				}
				//flip the sprite to face right
				sprite.setScale(1.0f,1.0f);
			}
		}
		else if(playerState.grounded == false)
		{
			if(playerState.INPUT_MoveLeft)
			{
				//We dont want to accelerate over our max speed, so check
				if((playerState.velocity.x - (airAcceleration * deltaTime)) > -maximumHorizontalSpeed)
				{
					playerState.velocity.x = playerState.velocity.x - (airAcceleration * deltaTime);
				}
				else
				{
					playerState.velocity.x = -maximumHorizontalSpeed;
				}
				//flip the sprite to face left
				sprite.setScale(-1.0f,1.0f);
			}
			if(playerState.INPUT_MoveRight)
			{
				//We dont want to accelerate over our max speed, so check
				if((playerState.velocity.x + (airAcceleration * deltaTime)) < maximumHorizontalSpeed)
				{
					//you multiply in deltatime here so acceleration is also constant
					playerState.velocity.x = playerState.velocity.x + (airAcceleration * deltaTime);
				}
				else
				{
					playerState.velocity.x = maximumHorizontalSpeed;
				}
				//flip the sprite to face right
				sprite.setScale(1.0f,1.0f);
			}
		}
	}
}
void Player::DoJumping()
{
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
}
void Player::AddDrag(float deltaTime)
{
	//Add drag
	if(playerState.grounded)
	{
		//Add ground drag
		if(playerState.velocity.x > 0)
		{
			//only apply drag if we're not actively going in this direction
			if(!playerState.INPUT_MoveRight)
			{
				if((playerState.velocity.x - (groundDrag * deltaTime)) > 0)
				{
					playerState.velocity.x = playerState.velocity.x - (groundDrag * deltaTime);
				}
				else
				{
					playerState.velocity.x = 0;
				}
			}
		}
		else if(playerState.velocity.x < 0)
		{
			//only apply drag if we're not actively going in this direction
			if(!playerState.INPUT_MoveLeft)
			{
				if((playerState.velocity.x + (groundDrag * deltaTime)) < 0)
				{
					playerState.velocity.x = playerState.velocity.x + (groundDrag * deltaTime);
				}
				else
				{
					playerState.velocity.x = 0;
				}
			}
		}
	}
	else
	{
		//Add air drag
		if(playerState.velocity.x > 0)
		{
			//only apply drag if we're not actively going in this direction
			if(!playerState.INPUT_MoveRight)
			{
				if((playerState.velocity.x - (airDrag * deltaTime)) > 0)
				{
					playerState.velocity.x = playerState.velocity.x - (airDrag * deltaTime);
				}
				else
				{
					playerState.velocity.x = 0;
				}
			}
		}
		else if(playerState.velocity.x < 0)
		{
			//only apply drag if we're not actively going in this direction
			if(!playerState.INPUT_MoveLeft)
			{
				if((playerState.velocity.x + (airDrag * deltaTime)) < 0)
				{
					playerState.velocity.x = playerState.velocity.x + (airDrag * deltaTime);
				}
				else
				{
					playerState.velocity.x = 0;
				}
			}
		}
	}
}
void Player::AddGravity(float deltaTime)
{
	if(playerState.velocity.y + (personalGravity * deltaTime) < terminalVelocity)
	{
		playerState.velocity.y = playerState.velocity.y + (personalGravity * deltaTime);
	}
	else
	{
		playerState.velocity.y = terminalVelocity;
	}

	//if you're falling, you're not grounded. You can run off a ledge, and without this grounded wont be updates
	if(playerState.velocity.y > 0)
	{
		playerState.grounded = false;
	}
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

//Getters and setters and wrappers around sf::sprite functionality
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