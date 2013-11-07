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
	attackKeys.push_back(sf::Keyboard::K);

	//Read the player movement variables from the config files, or if it cant be found load defaults
	LoadConfigValues(PLAYERCONFIG);

	//Initialise the attack rect collider
	attackCollider = sprite.getGlobalBounds();
	attackCollider.width += attackRange;
	//start the timer just to make sure it's running
	attackTimer.restart();

	return true;
}

bool Player::LoadConfigValues(std::string configFilePath)
{
	//set the default move values, in case the loading dosent work these will be used
	groundAcceleration = 800;
	airAcceleration = 500;
	maximumHorizontalSpeed = 500;
	groundDrag = 1800;
	airDrag = 0.0f;
	personalGravity = 2000;
	terminalVelocity = 5000;
	jumpStrength = 800;
	attackRange = 30;
	attackDelay = 1;

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
	//TODO : This whole things needs refactoring into a generic loader function, this violates DRY like mad.
	pugi::xml_node rootNode = configDoc.child("PlayerConfig");

	//Load in all the numerical config values
	LoadNumericalValue(maximumHorizontalSpeed,rootNode,"MaximumHorizontalSpeed");
	LoadNumericalValue(groundAcceleration,rootNode,"GroundAcceleration");
	LoadNumericalValue(airAcceleration,rootNode,"AirAcceleration");
	LoadNumericalValue(groundDrag,rootNode,"GroundDrag");
	LoadNumericalValue(airDrag,rootNode,"AirDrag");
	LoadNumericalValue(terminalVelocity,rootNode,"TerminalVelocity");
	LoadNumericalValue(personalGravity,rootNode,"PersonalGravity");
	LoadNumericalValue(jumpStrength,rootNode,"JumpStrength");
	LoadNumericalValue(attackRange,rootNode,"AttackRange");
	LoadNumericalValue(attackDelay,rootNode,"AttackDelay");


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
		std::cout << "AttackRange : " << attackRange << std::endl;
		std::cout << "AttackDelay : " << attackDelay << std::endl;
	}

	return true;
}

//For example, if you wanted to load a value "CheesesEaten" from the player config file into float cheeses, you would call LoadNumericalValue(cheeses,rootNode,"CheesesEaten");
void Player::LoadNumericalValue(float &valueToLoadInto, pugi::xml_node &rootNode, std::string valueNodeName)
{
	pugi::xml_node workingNode = rootNode.child(valueNodeName.c_str());
	if(workingNode)
	{
		std::stringstream valueStream(workingNode.child_value());
		valueStream >> valueToLoadInto;
	}
	else
	{
		std::cout << "Couldn't find " << valueNodeName << " in config file, using default" << std::endl;
	}
}

void Player::Update(sf::Event events, bool eventFired, double deltaTime, std::vector<sf::Rect<float>> &levelCollisionRects, std::vector<DestructibleObject> &destructibleObjects)
{
	//Receiving input is done seperate from the movement because ... well because I think it's cleaner, no other real reason.
	ReceiveControlInput(events,eventFired);
	HandleMovement(deltaTime, levelCollisionRects);
	DoAttacks(destructibleObjects);
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
	//Attack
	for(int i = 0; i < attackKeys.size(); i++)
	{
		if(sf::Keyboard::isKeyPressed(attackKeys[i]))
		{
			playerState.INPUT_Attack = true;
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
	//reset the moving states, just so we can keep track
	playerState.movingLeft = false;
	playerState.movingRight = false;

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
				playerState.facingLeft = true;
				playerState.facingRight = false;
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
				playerState.facingLeft = false;
				playerState.facingRight = true;
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
				playerState.facingLeft = true;
				playerState.facingRight = false;
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
				playerState.facingLeft = false;
				playerState.facingRight = true;
			}
		}
	}

	//update the state, just to keep track
	if(playerState.velocity.x > 0)
	{
		playerState.movingRight = true;
	}
	else if(playerState.velocity.x < 0)
	{
		playerState.movingLeft = true;
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

void Player::DoAttacks(std::vector<DestructibleObject> &destructibleObjects)
{
	playerState.attacking = false;
	if(playerState.INPUT_Attack)
	{
		//if the correct amount of time has passed since the last attack
		if(attackTimer.getElapsedTime().asSeconds() > attackDelay)
		{
			//We can attack, so do it!
			playerState.attacking = true;
			//check against all the objects to see if the attack collider colliders
			for(int i = 0; i < destructibleObjects.size(); i++)
			{
				if(attackCollider.intersects(destructibleObjects[i].GetCollisionRect()))
				{
					destructibleObjects[i].Destroy();
				}
			}
			//Reset the timer so we can't attack again immediately
			attackTimer.restart();
		}
	}
}

void Player::Render(sf::RenderWindow &window)
{
	window.draw(sprite);

	if(DEBUGPLAYER)
	{
		//Badly written debug code to show the collision rects of the player and any other related debug information, toggle in header
		sf::RectangleShape debugAttackCollider = sf::RectangleShape(sf::Vector2f(attackCollider.width,attackCollider.height));
		debugAttackCollider.setPosition(attackCollider.left,attackCollider.top);
		debugAttackCollider.setFillColor(sf::Color::Transparent);
		debugAttackCollider.setOutlineColor(sf::Color::Red);
		debugAttackCollider.setOutlineThickness(1.0f);
		window.draw(debugAttackCollider);

		sf::RectangleShape debugPlayerCollider = sf::RectangleShape(sf::Vector2f(GetCollider().width,GetCollider().height));
		debugPlayerCollider.setPosition(GetCollider().left,GetCollider().top);
		debugPlayerCollider.setFillColor(sf::Color::Transparent);
		debugPlayerCollider.setOutlineColor(sf::Color::Green);
		debugPlayerCollider.setOutlineThickness(1.0f);
		window.draw(debugPlayerCollider);
	}
}




void Player::Move(float x, float y)
{
	sf::Vector2f movementChange(x,y);
	sprite.move(movementChange);

	HandleAttackColliderPositioning();
}

//Called in move, makes sure the attack collider is in the right place
void Player::HandleAttackColliderPositioning()
{
	//move the attackCollider along with the player, otherwise we wont be able to check attack hitboxes
	attackCollider.top = GetCollider().top;
	//make sure the attackCollider is flipped the correct way
	if(playerState.facingRight)
	{
		attackCollider.left = GetCollider().left;
	}
	else if(playerState.facingLeft)
	{
		attackCollider.left = GetCollider().left - attackRange;
	}
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