#include "Player.h"


Player::Player(std::string playerTexturePath, sf::Vector2f startPos, sf::IntRect startTextureRect, sf::IntRect boundsRect, AudioManager &audioManager)
{
	Initialise(playerTexturePath, startPos, startTextureRect, boundsRect, audioManager);
}

Player::~Player(void)
{
}

bool Player::Initialise(std::string playerTexturePath, sf::Vector2f startPos, sf::IntRect startTextureRect, sf::IntRect boundsRect, AudioManager &audioManager)
{
	//Store a reference to the audiomanager
	this->audioManager = &audioManager;
	//Load audio files needed for the player
	this->audioManager->LoadSoundFile(JUMPSOUND,AudioManager::Jump);
	this->audioManager->LoadSoundFile(KICKSOUND,AudioManager::Kick);
	this->audioManager->LoadSoundFile(FARTSOUND,AudioManager::Fart);
	//set the audio to the sf::sound instances
	jumpSound.setBuffer(audioManager.GetSoundFile(AudioManager::Jump));
	attackSound.setBuffer(audioManager.GetSoundFile(AudioManager::Kick));
	fartSound.setBuffer(audioManager.GetSoundFile(AudioManager::Fart));

	if(!spriteSheet.loadFromFile(PLAYERTEXTURE))
	{
		std::cout << "Failed to load player texture" << std::endl;
	}

	sprite = std::unique_ptr<AnimatedSprite>(new AnimatedSprite(startTextureRect));

	sprite->setTexture(spriteSheet);
	//Set the origin to the center of the sprite
	sprite->setOrigin(sprite->getGlobalBounds().width/2,sprite->getGlobalBounds().height/2);
	sprite->setPosition(startPos);

	//the collider used
	collisionRect = sf::Rect<float>(boundsRect);
	collisionRect.left = startPos.x - collisionRect.width/2;
	collisionRect.top = startPos.y - collisionRect.height/2;

	//load the control vectors up
	moveLeftKeys.push_back(sf::Keyboard::A);
	moveLeftKeys.push_back(sf::Keyboard::Left);
	moveRightKeys.push_back(sf::Keyboard::D);
	moveRightKeys.push_back(sf::Keyboard::Right);
	jumpKeys.push_back(sf::Keyboard::Space);
	jumpKeys.push_back(sf::Keyboard::Up);
	jumpKeys.push_back(sf::Keyboard::W);
	attackKeys.push_back(sf::Keyboard::X);

	//Read the player movement variables from the config files, or if it cant be found load defaults
	LoadConfigValues(PLAYERCONFIG);

	doubleJumpKeyTimer.restart();
	//you need to have 65ms without having a jump key down to double jump again. This isnt a balance thing, it's more of a "Make sure the game dosent jump twice at once" thing
	doubleJumpKeyTime = 65;

	//The player can double jump in this level
	ToggleAbility(DoubleJump,true);

	//Initialise the attack rect collider
	attackCollider = GetCollider();
	attackCollider.width += attackRange;
	//start the timer just to make sure it's running
	attackTimer.restart();

	walkAnimName = "Walk";
	idleAnimName = "Idle";
	startJumpAnimName = "StartJump";
	jumpAnimName = "Jump";
	fallAnimName = "Fall";
	landFromNormalJumpAnimName = "Land";
	doubleJumpAnimName = "DoubleJump";
	doubleJumpToFallAnimName = "LandFromDoubleJump";
	kickAnimName = "Name";

	LoadAnimations();

	fallVelocityTillFallAnimation = 200.0f;

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
	doubleJumpStrength = 800;
	doubleJumpVelocityChangeImpulse = 500;
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

	//Set the player size and store the loaded scale
	LoadNumericalValue(loadedScaleX,rootNode,"XScale");
	LoadNumericalValue(loadedScaleY,rootNode,"YScale");
	sprite->setScale(loadedScaleX,loadedScaleY);


	//Load in the values that define movement and physics
	LoadNumericalValue(maximumHorizontalSpeed,rootNode,"MaximumHorizontalSpeed");
	LoadNumericalValue(groundAcceleration,rootNode,"GroundAcceleration");
	LoadNumericalValue(airAcceleration,rootNode,"AirAcceleration");
	LoadNumericalValue(groundDrag,rootNode,"GroundDrag");
	LoadNumericalValue(airDrag,rootNode,"AirDrag");
	LoadNumericalValue(terminalVelocity,rootNode,"TerminalVelocity");
	LoadNumericalValue(personalGravity,rootNode,"PersonalGravity");
	LoadNumericalValue(jumpStrength,rootNode,"JumpStrength");
	LoadNumericalValue(doubleJumpStrength,rootNode,"DoubleJumpStrength");
	LoadNumericalValue(doubleJumpVelocityChangeImpulse,rootNode,"DoubleJumpVelocityChangeImpulse");
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
		std::cout << "DoubleJumpStrength : " << doubleJumpStrength << std::endl;
		std::cout << "DoubleJumpVelocityChangeImpulse : " << doubleJumpVelocityChangeImpulse << std::endl;
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
	sprite->UpdateAnimations();
	//Receiving input is done seperate from the movement because ... well because I think it's cleaner, no other real reason.
	ReceiveControlInput(events,eventFired);
	DoAttacks(destructibleObjects);
	HandleMovement(events, eventFired, deltaTime, levelCollisionRects);
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

void Player::HandleMovement(sf::Event events, bool eventFired, double deltaTime, std::vector<sf::Rect<float>> &levelCollisionRects)
{

	//Deal wid jumping 
	DoJumping(events, eventFired);
	//Add gravity
	AddGravity(deltaTime);
	//Move vertical
	Move(0, playerState.velocity.y * deltaTime);
	//Check to see if the new position is valid vertically
	HandleVerticalCollision(levelCollisionRects);


	//update the player velocity to move left and right depending on player input
	DoLeftAndRightMovement(deltaTime);
	//Add the drag horizontally, different whether you are grounded or not
	AddDrag(deltaTime);
	//Move horizontal
	Move(playerState.velocity.x * deltaTime ,0);
	//Check to see that the new position is valid horizontally
	HandleHorizontalCollision(levelCollisionRects);

	//Make sure the right animation is playing
	HandleAnimations();

	lastState = playerState;

}
//All these functions are just used in HandleMovement, seperated because its cleaner and easier to make logic changes
void Player::DoLeftAndRightMovement(double deltaTime)
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
				sprite->setScale(-loadedScaleX,loadedScaleY);
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
				sprite->setScale(loadedScaleX,loadedScaleY);
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
				sprite->setScale(-loadedScaleX,loadedScaleY);
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
				sprite->setScale(loadedScaleX,loadedScaleY);
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
void Player::DoJumping(sf::Event events, bool eventFired)
{
	//Jump
	if(playerState.INPUT_Jump)
	{
		//you can only jump if you're grounded, or if you're in the grace period after falling off a ledge
		if(playerState.grounded == true) 
		{
			jumpSound.play();
			playerState.velocity.y -= jumpStrength;
			playerState.grounded = false;
			playerState.firstJumping = true;
		}
		else
		{
			//Dont double jump if the player dosent have the ability
			if(playerState.canDoubleJump)
			{
				//If we're not grounded, we could be trying to double jump
				if(playerState.firstJumping == true)
				{
					if(!playerState.doubleJumping)
					{
						if(!playerState.grounded)
						{
							//check to make sure that the player really can double jump, and the button hasnt just fired twice
							if(doubleJumpKeyTimer.getElapsedTime().asMilliseconds() > doubleJumpKeyTime)
							{
								fartSound.play();
								playerState.velocity.y = 0;
								playerState.velocity.y -= jumpStrength;

								//This chunk of code allows us to change velocity when we double jump
								if(playerState.INPUT_MoveLeft)
								{
									playerState.velocity.x -= doubleJumpVelocityChangeImpulse;
								}
								else if(playerState.INPUT_MoveRight)
								{
									playerState.velocity.x += doubleJumpVelocityChangeImpulse;
								}

								playerState.firstJumping = false;
								playerState.doubleJumping = true;
							}
						}
					}
				}
			}
		}
	}

	//if a jump key is pressed you need to restart this timer to ensure we dont double jump twice at once
	if(playerState.INPUT_Jump)
	{
		doubleJumpKeyTimer.restart();
	}

}
void Player::AddDrag(double deltaTime)
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
void Player::AddGravity(double deltaTime)
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

void Player::HandleAnimations()
{
	std::cout << playerState.velocity.y << std::endl;
	if((playerState.attacking) && (playerState.INPUT_Attack))
	{
		sprite->SetCurrentAnimation(kickAnimName);
		sprite->SetRepeating(false);
		sprite->Play();
		playerState.animState = PlayerState::AnimationState::Attacking;
	}
	if(playerState.animState == PlayerState::AnimationState::Attacking)
	{
		if(sprite->IsPlaying())
		{
			return;
		}
		else
		{
			if(playerState.grounded == true)
			{
				playerState.animState = PlayerState::AnimationState::Idle;
			}
			else
			{
				sprite->SetCurrentAnimation(jumpAnimName);
				sprite->SetRepeating(false);
				sprite->Play();
				playerState.animState = PlayerState::AnimationState::FirstJumping;
			}
		}
	}

	if((lastState.grounded == true) || (playerState.grounded == true))
	{
		if(playerState.INPUT_Jump)
		{
			sprite->SetCurrentAnimation(jumpAnimName);
			sprite->SetRepeating(false);
			sprite->Play();
			playerState.animState = PlayerState::AnimationState::FirstJumping;
		}
	}
	if((playerState.doubleJumping == true) && (lastState.doubleJumping == false))
	{
		if(playerState.INPUT_Jump)
		{
			sprite->SetCurrentAnimation(doubleJumpAnimName);
			sprite->SetRepeating(false);
			sprite->Play();
			playerState.animState = PlayerState::AnimationState::DoubleJumping;
		}
	}


	if(playerState.animState == PlayerState::AnimationState::FirstJumping)
	{
		if(playerState.velocity.y > fallVelocityTillFallAnimation)
		{
			sprite->SetCurrentAnimation(fallAnimName);
			sprite->SetRepeating(false);
			sprite->Play();
			playerState.animState = PlayerState::AnimationState::Falling;
		}
	}
	else if (playerState.animState == PlayerState::AnimationState::DoubleJumping)
	{
		if(playerState.velocity.y > fallVelocityTillFallAnimation)
		{
			sprite->SetCurrentAnimation(doubleJumpToFallAnimName);
			sprite->SetRepeating(false);
			sprite->Play();
			playerState.animState = PlayerState::AnimationState::Falling;
		}
	}

	if(playerState.animState == PlayerState::AnimationState::Falling)
	{
		if(playerState.grounded == true)
		{
			sprite->SetCurrentAnimation(landFromNormalJumpAnimName);
			sprite->SetRepeating(false);
			sprite->Play();
			playerState.animState = PlayerState::AnimationState::Landing;
		}
	}
	if(playerState.animState == PlayerState::AnimationState::Landing)
	{
		//If we've finished the land
		if(sprite->IsPlaying() == false)
		{
			playerState.animState = PlayerState::AnimationState::Idle;
			sprite->SetCurrentAnimation(idleAnimName);
			sprite->SetRepeating(true);
			sprite->Play();
		}
	}


	if(playerState.INPUT_MoveLeft)
	{
		if(playerState.grounded == true)
		{
			if((!lastState.INPUT_MoveLeft) || (playerState.animState == PlayerState::AnimationState::Idle))
			{
				playerState.animState = PlayerState::AnimationState::Walk;
				sprite->SetCurrentAnimation(walkAnimName);
				sprite->SetRepeating(true);
				sprite->Play();
			}
		}
	}
	else if(playerState.INPUT_MoveRight)
	{
		if(playerState.grounded == true)
		{
			if((!lastState.INPUT_MoveRight) || (playerState.animState == PlayerState::AnimationState::Idle))
			{
				playerState.animState = PlayerState::AnimationState::Walk;
				sprite->SetCurrentAnimation(walkAnimName);
				sprite->SetRepeating(true);
				sprite->Play();
			}
		}
	}
	else
	{
		if(playerState.animState == PlayerState::AnimationState::Walk)
		{
			playerState.animState = PlayerState::AnimationState::Idle;
			sprite->SetCurrentAnimation(idleAnimName);
			sprite->SetRepeating(true);
			sprite->Play();
		}
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
				playerState.firstJumping = false;
				playerState.doubleJumping = false;
				playerState.velocity.y = 0;
			}
			//we're going up, snap to bottom of object
			else if(playerState.velocity.y < 0)
			{
				SetPosition(GetPosition().x, (levelCollisionRects[i].top + levelCollisionRects[i].height) + (GetCollider().height/2));
				playerState.grounded = false;
				playerState.velocity.y = 0;
			}
			//wtf mate? do nothing
			else
			{

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
			//Play the attack sound
			attackSound.play();
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

void Player::ToggleAbility(Abilities ability, bool active)
{
	switch(ability)
	{
	case Abilities::DoubleJump :
		playerState.canDoubleJump = active;
		break;
	}
}

void Player::Render(sf::RenderWindow &window)
{
	window.draw(*sprite);

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
	sprite->move(movementChange);
	collisionRect.left = collisionRect.left + x;
	collisionRect.top = collisionRect.top + y;
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

void Player::LoadAnimations()
{
	//walk
	std::vector<sf::IntRect> walkAnim;
	int xOffset = 125;
	int framesToAdd = 19;
	sf::Rect<int> frameSize;
	frameSize.width = 126;
	frameSize.height = 156;
	for(int i = 0; i < framesToAdd; i++)
	{
		walkAnim.push_back(sf::IntRect(i * xOffset,0,frameSize.width,frameSize.height));
	}
	sprite->AddAnimation(walkAnimName,walkAnim,0.03f);

	//idle
	std::vector<sf::IntRect> idleAnim;
	sf::Vector2i startPos = sf::Vector2i(0,166);
	xOffset = 125;
	framesToAdd = 20;
	frameSize.width = 126;
	frameSize.height = 156;
	for(int i = 0; i < framesToAdd; i++)
	{
		idleAnim.push_back(sf::IntRect((i * xOffset) + startPos.x,0 + startPos.y,frameSize.width,frameSize.height));
	}
	sprite->AddAnimation(idleAnimName,idleAnim,0.06f);

	
	//Jump
	std::vector<sf::IntRect> jumpAnim;
	xOffset = 125;
	framesToAdd = 4;
	frameSize.width = 120;
	frameSize.height = 156;
	startPos = sf::Vector2i(xOffset,505);
	for(int i = 0; i < framesToAdd; i++)
	{
		jumpAnim.push_back(sf::IntRect((i * xOffset) + startPos.x,0 + startPos.y,frameSize.width,frameSize.height));
	}
	sprite->AddAnimation(jumpAnimName,jumpAnim,0.03f);

	//Fall
	std::vector<sf::IntRect> fallAnim;
	xOffset = 125;
	framesToAdd = 7;
	frameSize.width = 120;
	frameSize.height = 156;
	startPos = sf::Vector2i(xOffset * 4,505);
	for(int i = 0; i < framesToAdd; i++)
	{
		fallAnim.push_back(sf::IntRect((i * xOffset) + startPos.x,0 + startPos.y,frameSize.width,frameSize.height));
	}
	sprite->AddAnimation(fallAnimName,fallAnim,0.03f);

	//Land
	std::vector<sf::IntRect> landAnim;
	xOffset = 125;
	framesToAdd = 6;
	frameSize.width = 120;
	frameSize.height = 156;
	startPos = sf::Vector2i(xOffset * 10,505);
	for(int i = 0; i < framesToAdd; i++)
	{
		landAnim.push_back(sf::IntRect((i * xOffset) + startPos.x,0 + startPos.y,frameSize.width,frameSize.height));
	}
	sprite->AddAnimation(landFromNormalJumpAnimName,landAnim,0.03f);

	//DoubleJump
	std::vector<sf::IntRect> doubleJumpAnim;
	xOffset = 125;
	framesToAdd = 10;
	frameSize.width = 120;
	frameSize.height = 156;
	startPos = sf::Vector2i(0,686);
	for(int i = 0; i < framesToAdd; i++)
	{
		doubleJumpAnim.push_back(sf::IntRect((i * xOffset) + startPos.x,0 + startPos.y,frameSize.width,frameSize.height));
	}
	sprite->AddAnimation(doubleJumpAnimName,doubleJumpAnim,0.03f);

	//LandFromDoubleJump
	std::vector<sf::IntRect> doubleJumpToFallAnim;
	xOffset = 125;
	framesToAdd = 8;
	frameSize.width = 120;
	frameSize.height = 156;
	startPos = sf::Vector2i(xOffset * 10,686);
	for(int i = 0; i < framesToAdd; i++)
	{
		doubleJumpToFallAnim.push_back(sf::IntRect((i * xOffset) + startPos.x,0 + startPos.y,frameSize.width,frameSize.height));
	}
	sprite->AddAnimation(doubleJumpToFallAnimName,doubleJumpToFallAnim,0.03f);

	//Kick
	std::vector<sf::IntRect> kickAnim;
	xOffset = 125;
	framesToAdd = 15;
	frameSize.width = 120;
	frameSize.height = 156;
	startPos = sf::Vector2i(0,863);
	for(int i = 0; i < framesToAdd; i++)
	{
		kickAnim.push_back(sf::IntRect((i * xOffset) + startPos.x,0 + startPos.y,frameSize.width,frameSize.height));
	}
	sprite->AddAnimation(kickAnimName,kickAnim,0.03f);

	sprite->SetCurrentAnimation(idleAnimName);
	sprite->SetRepeating(true);
	sprite->Play();
}

void Player::SetPosition(float xPos, float yPos)
{
	sprite->setPosition(xPos,yPos);
	collisionRect.left = xPos - collisionRect.width/2;
	collisionRect.top = yPos - collisionRect.height/2;
}

sf::Vector2f Player::GetPosition()
{
	return sprite->getPosition();
}

void Player::SetPosition(sf::Vector2f position)
{
	sprite->setPosition(position);
	collisionRect.left = position.x - collisionRect.width/2;
	collisionRect.top = position.y - collisionRect.height/2;
}

sf::FloatRect Player::GetCollider()
{
	return collisionRect;
}