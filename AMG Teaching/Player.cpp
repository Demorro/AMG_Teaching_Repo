#include "Player.h"

Player::Player(std::string playerTexturePath, sf::Vector2f startPos, sf::IntRect startTextureRect, sf::IntRect boundsRect)
{
	Initialise(playerTexturePath, startPos, startTextureRect, boundsRect);
}

Player::~Player(void)
{
}

bool Player::Initialise(std::string playerTexturePath, sf::Vector2f startPos, sf::IntRect startTextureRect, sf::IntRect boundsRect)
{

	//Load the big ol' sprite texture, if this becomes too big we may need to split it up
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
	sprintKeys.push_back(sf::Keyboard::LShift);
	sprintKeys.push_back(sf::Keyboard::RShift);

	//Read the player movement variables from the config files, or if it cant be found load defaults
	LoadConfigValues(PLAYERCONFIG);
	LoadAudioConfigValues(AUDIOCONFIG);

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

	//Names of the animation, used for switching between them
	walkAnimName = "Walk";
	idleAnimName = "Idle";
	startJumpAnimName = "StartJump";
	jumpAnimName = "Jump";
	fallAnimName = "Fall";
	landFromNormalJumpAnimName = "Land";
	doubleJumpAnimName = "DoubleJump";
	doubleJumpToFallAnimName = "LandFromDoubleJump";
	kickAnimName = "Name";

	//Load the animations in, done manually for now
	LoadAnimations();

	//If the player is going down at this speed, the fall animation is triggered
	fallVelocityTillFallAnimation = 200.0f;

	handleToStandingPlatform = nullptr;

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
	sprintMultiplier = 2;
	walkFrameTime = 0.03f;
	sprintFrameTime = 0.015f;

	pugi::xml_document configDoc;

	LoadXMLDoc(configDoc,configFilePath);


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
	LoadNumericalValue(sprintMultiplier,rootNode,"SprintMultiplier");
	LoadNumericalValue(walkFrameTime,rootNode,"WalkFrameDelay");
	LoadNumericalValue(sprintFrameTime,rootNode,"SprintFrameDelay");

	if(DEBUGPLAYER)
	{
		std::cout << std::endl;
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
		std::cout << "SprintMultiplier : " << sprintMultiplier << std::endl;
		std::cout << "Walk Frame Delay : " << walkFrameTime << std::endl;
		std::cout << "Sprint Frame Delay : " << sprintFrameTime << std::endl;
		std::cout << std::endl;
	}

	return true;
}

bool Player::LoadAudioConfigValues(std::string audioConfigFilePath)
{
	std::string jumpSoundPathNodeName = "JumpSoundPath";
	std::string footStepSoundPathNodeName = "FootStepSoundPath";
	std::string kickSoundPathNodeName = "KickSoundPath";
	std::string landSoundPathNodeName = "LandSoundPath";
	std::string fallingFastSoundPathNodeName = "FallingFastSoundPath";
	std::string fartSoundPathNodeName = "FartSoundPath";
	std::string jumpVolumeNodeName = "JumpSoundVolume";
	std::string footStepVolumeNodeName = "FootStepSoundVolume";
	std::string kickVolumeNodeName = "KickSoundVolume";
	std::string landVolumeNodeName = "LandSoundVolume";
	std::string fallingFastVolumeNodeName = "FallingFastSoundVolume";
	std::string fartSoundVolumeNodeName = "FartSoundVolume";
	std::string fallingFastTriggerSpeedNodeName = "FallingFastTriggerSpeed";


	std::string jumpSoundPath = JUMPSOUND;
	std::string footStepSoundPath = FOOTSTEPSOUND;
	std::string kickSoundPath = KICKSOUND;
	std::string fallingFastSoundPath = FALLINGFASTSOUND;
	std::string landSoundPath = LANDSOUND;
	std::vector<std::string> fartSoundPaths;

	float jumpSoundVolume = 100;
	float footStepSoundVolume = 100;
	float kickSoundVolume = 100;
	float landSoundVolume = 100;
	float fallingFastSoundVolume = 100;
	float fartSoundVolume = 100;

	fallingFastTriggerSpeed = 1000;

	
	pugi::xml_document configDoc;
	LoadXMLDoc(configDoc,audioConfigFilePath);

	//Work through all the variables we need to load and load em, checking for if they're there or not each time
	//TODO : This whole things needs refactoring into a generic loader function, this violates DRY like mad.
	pugi::xml_node rootNode = configDoc.child("AudioConfig");

	//load in the fart sounds, do it like this cause you can have a variable amount of fart sounds
	for(pugi::xml_node node = rootNode.first_child(); node; node = node.next_sibling())
	{
		std::string nodeName = node.name();
		if(nodeName == fartSoundPathNodeName)
		{
			std::string value = node.child_value();
			fartSoundPaths.push_back(value);
		}
	}
	//Load fart volume
	LoadNumericalValue(fartSoundVolume,rootNode,fartSoundVolumeNodeName);

	//Load in Jump Sound Path
	LoadTextValue(jumpSoundPath,rootNode,jumpSoundPathNodeName);
	LoadNumericalValue(jumpSoundVolume,rootNode,jumpVolumeNodeName);
	//Load in FootStep Sound Path
	LoadTextValue(footStepSoundPath,rootNode,footStepSoundPathNodeName);
	LoadNumericalValue(footStepSoundVolume,rootNode,footStepVolumeNodeName);
	//Load in Kick Sound Path
	LoadTextValue(kickSoundPath,rootNode,kickSoundPathNodeName);
	LoadNumericalValue(kickSoundVolume,rootNode,kickVolumeNodeName);
	//Load in Fast Fall Sound
	LoadTextValue(fallingFastSoundPath,rootNode,fallingFastSoundPathNodeName);
	LoadNumericalValue(fallingFastSoundVolume,rootNode,fallingFastVolumeNodeName);
	LoadNumericalValue(fallingFastTriggerSpeed, rootNode,fallingFastTriggerSpeedNodeName);
	//Load in Land Sound Path
	LoadTextValue(landSoundPath,rootNode,landSoundPathNodeName);
	LoadNumericalValue(landSoundVolume,rootNode,landVolumeNodeName);

	
	//Load audio files needed for the player
	jumpSoundBuffer.loadFromFile(jumpSoundPath);
	attackSoundBuffer.loadFromFile(kickSoundPath);
	footStepSoundBuffer.loadFromFile(footStepSoundPath);
	fallingFastSoundBuffer.loadFromFile(fallingFastSoundPath);
	landSoundBuffer.loadFromFile(landSoundPath);
	//set the audio to the sf::sound instances
	jumpSound.setBuffer(jumpSoundBuffer);
	jumpSound.setVolume(jumpSoundVolume);
	attackSound.setBuffer(attackSoundBuffer);
	attackSound.setVolume(kickSoundVolume);
	footStepSound.setBuffer(footStepSoundBuffer);
	footStepSound.setVolume(footStepSoundVolume);
	fallingFastSound.setBuffer(fallingFastSoundBuffer);
	fallingFastSound.setVolume(fallingFastSoundVolume); //because falling fast is turned on and off by volume tween, rather than actual play
	fallingFastSound.setLoop(true);
	landSound.setBuffer(landSoundBuffer);
	landSound.setVolume(landSoundVolume);


	//load the fart sounds
	int noOfFartSounds = fartSoundPaths.size();
	fartSoundBuffers.resize(noOfFartSounds,sf::SoundBuffer());
	for(int i = 0; i < fartSoundBuffers.size(); i++)
	{
		fartSoundBuffers[i].loadFromFile(fartSoundPaths[i]);
	}
	fartSound.setBuffer(fartSoundBuffers[rand() % fartSoundBuffers.size()]);
	fartSound.setVolume(fartSoundVolume);
	
	return true;
}
void Player::Update(sf::Event events, bool eventFired, double deltaTime, std::vector<sf::Rect<float>> &staticLevelCollisionBounds, std::vector<SpecialPlatform> &movingPlatforms, std::vector<DestructibleObject> &destructibleObjects, bool shouldPlaySounds)
{
	sprite->UpdateAnimations();
	//Receiving input is done seperate from the movement because ... well because I think it's cleaner, no other real reason.
	if(playerState.isAcceptingInput) //If we toggle off input, don't want any of these actions to happen (of special note is reset inputs, because we can for instance, toggle left movement on and then turn off inputs and the player will walk himself :))
	{
		playerState.ResetInputs();
		ReceiveKeyboardInput(events,eventFired);
		ReceiveControllerInput(events,eventFired);
		DoAttacks(destructibleObjects,shouldPlaySounds);
	}
	DoPassiveSounds(shouldPlaySounds); // Do before handle movement cause it uses the last state set in there
	HandleMovement(events, eventFired, deltaTime, staticLevelCollisionBounds, movingPlatforms, shouldPlaySounds);
}

void Player::Respawn(sf::Vector2f spawnPosition)
{
	bool canPlayerDoubleJump = playerState.canDoubleJump;

	//Reset the players state to default.
	playerState.ResetEverythingButAnimation();
	lastState.ResetEverythingButAnimation();
	

	ToggleAbility(Player::Abilities::DoubleJump, canPlayerDoubleJump);

	SetPosition(spawnPosition);
}

void Player::ReceiveKeyboardInput(sf::Event events, bool eventFired)
{
//	playerState.ResetInputs();

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
	for(int i = 0; i < sprintKeys.size(); i++)
	{
		if(sf::Keyboard::isKeyPressed(sprintKeys[i]))
		{
			playerState.INPUT_IsSprinting = true;
		}
	}
	
}

void Player::ReceiveControllerInput(sf::Event events, bool eventfired)
{
//	playerState.ResetInputs();
	
	if(sf::Joystick::isConnected(0))
	{
		//Left
		if(sf::Joystick::getAxisPosition(0, sf::Joystick::X) < - ANALOGUESTICKMOVEBOUNDRY)
		{
			playerState.INPUT_MoveLeft = true;
		}
		
		//Right
		if(sf::Joystick::getAxisPosition(0, sf::Joystick::X) > ANALOGUESTICKMOVEBOUNDRY)
		{
			playerState.INPUT_MoveRight = true;
		}
	
		//Jump
		if(sf::Joystick::isButtonPressed(0,XBOXCONTROLLERBUTTONS::A))
		{
			playerState.INPUT_Jump = true;
		}

		//Attack
		if((sf::Joystick::isButtonPressed(0,XBOXCONTROLLERBUTTONS::X)) || (sf::Joystick::isButtonPressed(0,XBOXCONTROLLERBUTTONS::B)))
		{
			playerState.INPUT_Attack = true;
		}

		if(sf::Joystick::isButtonPressed(0,RB))
		{
			playerState.INPUT_IsSprinting = true;
		}

	}
}
void Player::HandleMovement(sf::Event events, bool eventFired, double deltaTime, std::vector<sf::Rect<float>> &staticLevelCollisionBounds, std::vector<SpecialPlatform> &movingPlatforms, bool shouldPlaySounds)
{

	AdjustPositionForMovingPlatforms(deltaTime,movingPlatforms);

	//Deal wid jumping 
	DoJumping(events, eventFired, shouldPlaySounds);
	//update the player velocity to move left and right depending on player inputs
	DoLeftAndRightMovement(deltaTime);
	//Add Gravity
	AddGravity(deltaTime);
	//Move according to velocity
	Move(playerState.velocity.x * deltaTime, playerState.velocity.y * deltaTime);

	//Check to see if the new position is valid vertically
	HandleCollision(staticLevelCollisionBounds,movingPlatforms);
	
	//Add the drag horizontally, different whether you are grounded or not
	AddDrag(deltaTime);
	

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
				if(playerState.INPUT_IsSprinting == false) //If we're not sprinting we dont need to multiply speed value caps
				{
					//We dont want to accelerate over our max speed, so check
					if((playerState.velocity.x - (groundAcceleration * deltaTime)) > -maximumHorizontalSpeed)
					{
						playerState.velocity.x = playerState.velocity.x - (groundAcceleration * deltaTime);
					}
					else
					{
						//this small segment means that when we go from sprinting on the ground to not sprinting, we slow down as according to drag, rather than just snapping
						playerState.velocity.x += groundDrag * deltaTime;
						playerState.velocity.x = clip(playerState.velocity.x,-maximumHorizontalSpeed * sprintMultiplier, -maximumHorizontalSpeed);
					}
				}
				else if(playerState.INPUT_IsSprinting) //We need to multiply speed value caps if we're sprinting.
				{
					if((playerState.velocity.x - (groundAcceleration * deltaTime)) > -(maximumHorizontalSpeed * sprintMultiplier) )
					{
						playerState.velocity.x = playerState.velocity.x - (groundAcceleration * deltaTime);
					}
					else
					{
						playerState.velocity.x = -(maximumHorizontalSpeed * sprintMultiplier);
					}
				}
				//flip the sprite to face left
				sprite->setScale(-loadedScaleX,loadedScaleY);
				playerState.facingLeft = true;
				playerState.facingRight = false;

			}
			if(playerState.INPUT_MoveRight)
			{
				if(playerState.INPUT_IsSprinting == false) //If we're not sprinting we dont nee to multiply speed value caps
				{
					//We dont want to accelerate over our max speed, so check
					if((playerState.velocity.x - (groundAcceleration * deltaTime)) < maximumHorizontalSpeed)
					{
						playerState.velocity.x = playerState.velocity.x + (groundAcceleration * deltaTime);
					}
					else
					{
						//this small segment means that when we go from sprinting on the ground to not sprinting, we slow down as according to drag, rather than just snapping
						playerState.velocity.x -= groundDrag * deltaTime;
						playerState.velocity.x = clip(playerState.velocity.x,maximumHorizontalSpeed, maximumHorizontalSpeed * sprintMultiplier);
					}
				}
				else if(playerState.INPUT_IsSprinting) //We need to multiply speed value caps if we're sprinting.
				{
					if((playerState.velocity.x - (groundAcceleration * deltaTime)) < (maximumHorizontalSpeed * sprintMultiplier) )
					{
						playerState.velocity.x = playerState.velocity.x + (groundAcceleration * deltaTime);
					}
					else
					{
						playerState.velocity.x = (maximumHorizontalSpeed * sprintMultiplier);
					}
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
				//The wasSprintingUponJump checks make it so that if the player released the sprint button in midair after doing a sprint jump, the player dosent just abruptly slow in midair, but the arc continues on as usual.
				if((playerState.INPUT_IsSprinting == false) && (playerState.wasSprintingUponJump == false))
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
				}
				else if((playerState.INPUT_IsSprinting) || (playerState.wasSprintingUponJump == false))
				{
					if((playerState.velocity.x - (airAcceleration * deltaTime)) > -(maximumHorizontalSpeed * sprintMultiplier))
					{
						playerState.velocity.x = playerState.velocity.x - (airAcceleration * deltaTime);
					}
					else
					{
						playerState.velocity.x = -(maximumHorizontalSpeed * sprintMultiplier);
					}
				}
				//flip the sprite to face left
				sprite->setScale(-loadedScaleX,loadedScaleY);
				playerState.facingLeft = true;
				playerState.facingRight = false;
			}
			if(playerState.INPUT_MoveRight)
			{
				if((playerState.INPUT_IsSprinting == false) && (playerState.wasSprintingUponJump == false))
				{
					//We dont want to accelerate over our max speed, so check
					if((playerState.velocity.x + (airAcceleration * deltaTime)) < maximumHorizontalSpeed)
					{
						//you multiply in deltatime here so acceleration is also constant
						playerState.velocity.x = playerState.velocity.x + (airAcceleration * deltaTime);
					}
					else
					{
						playerState.velocity.x = (maximumHorizontalSpeed);
					}
				}
				else if((playerState.INPUT_IsSprinting) || (playerState.wasSprintingUponJump == false))
				{
					if((playerState.velocity.x + (airAcceleration * deltaTime)) < (maximumHorizontalSpeed * sprintMultiplier))
					{
						//you multiply in deltatime here so acceleration is also constant
						playerState.velocity.x = playerState.velocity.x + (airAcceleration * deltaTime);
					}
					else
					{
						playerState.velocity.x = (maximumHorizontalSpeed * sprintMultiplier);
					}
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
void Player::DoJumping(sf::Event events, bool eventFired, bool shouldPlaySounds)
{
	//Jump
	if(playerState.INPUT_Jump)
	{
		
		//you can only jump if you're grounded, or if you're in the grace period after falling off a ledge
		if(playerState.grounded == true) 
		{
			if(shouldPlaySounds)
			{
				jumpSound.play();
			}
			playerState.velocity.y -= jumpStrength;
			playerState.grounded = false;
			playerState.firstJumping = true;

			//store whether the player was sprinting when he jumped, to deal with max velocity in the air.
			if(playerState.INPUT_IsSprinting)
			{
				playerState.wasSprintingUponJump = true;
			}
			else
			{
				playerState.wasSprintingUponJump = false;
			}

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
								if(shouldPlaySounds)
								{
									fartSound.setBuffer(fartSoundBuffers[rand() % fartSoundBuffers.size()]); //do a random fart noise
									fartSound.play();
								}
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

								//store whether the player was sprinting when he jumped, to deal with max velocity in the air.
								if(playerState.INPUT_IsSprinting)
								{
									playerState.wasSprintingUponJump = true;
								}
								else
								{
									playerState.wasSprintingUponJump = false;
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

	float restVelocity = 0.0f;
	
	if(playerState.isOnMovingPlatform)
	{
		restVelocity = platformMoveVector.x;
	}
	//Add drag
	if(playerState.grounded)
	{
		//Add ground drag
		if(playerState.velocity.x > restVelocity)
		{
			//only apply drag if we're not actively going in this direction
			if(!playerState.INPUT_MoveRight)
			{
				if((playerState.velocity.x - (groundDrag * deltaTime)) > restVelocity)
				{
					playerState.velocity.x = playerState.velocity.x - (groundDrag * deltaTime);
				}
				else
				{
					playerState.velocity.x = restVelocity;
				}
			}
		}
		else if(playerState.velocity.x < restVelocity)
		{
			//only apply drag if we're not actively going in this direction
			if(!playerState.INPUT_MoveLeft)
			{
				if((playerState.velocity.x + (groundDrag * deltaTime)) < restVelocity)
				{
					playerState.velocity.x = playerState.velocity.x + (groundDrag * deltaTime);
				}
				else
				{
					playerState.velocity.x = restVelocity;
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

bool Player::HandleCollision(std::vector<sf::Rect<float>> &staticLevelCollisionBounds, std::vector<SpecialPlatform> &movingPlatforms)
{

	bool hasCollided = false;

	//Check collision against static platforms
	for(int i = 0; i < staticLevelCollisionBounds.size(); i++)
	{
		if(GetCollider().intersects(staticLevelCollisionBounds[i]))
		{
			hasCollided = true;
			//Get the vector of intersection between the player collider and the object
			sf::Vector2f collisionDepth = GetIntersectionDepth(GetCollider(),staticLevelCollisionBounds[i]);

			//Push the player out in the shortest direction
			if(abs(collisionDepth.x) < abs(collisionDepth.y))
			{
				Move(collisionDepth.x,0);
				playerState.velocity.x = 0;

			}
			else
			{
				Move(0,collisionDepth.y);

				if(collisionDepth.y < 0)
				{
					playerState.grounded = true;
					playerState.firstJumping = false;
					playerState.doubleJumping = false;
				}
				else
				{
					playerState.grounded = false;
				}
				playerState.velocity.y = 0;
			}
		}
	}


	//Check collision against moving platforms
	for(int i = 0; i < movingPlatforms.size(); i++)
	{
		if(GetCollider().intersects(movingPlatforms[i].getGlobalBounds()))
		{
			hasCollided = true;
			//Get the vector of intersection between the player collider and the object
			sf::Vector2f collisionDepth = GetIntersectionDepth(GetCollider(),movingPlatforms[i].getGlobalBounds());

			//Push the player out in the shortest direction
			if(abs(collisionDepth.x) < abs(collisionDepth.y))
			{
				Move(collisionDepth.x,0);
				playerState.velocity.x = 0;
			}
			else
			{
				Move(0,collisionDepth.y);

				//The different velocities here are due to when you're on top of the platform you get special logic to keep you there, but if you didnt throw the velocity back at the player from the bottom, you would stick there too sometimes. I know it feels wrong, but it works.
				if(collisionDepth.y < 0)
				{
					//Top
					playerState.grounded = true;
					playerState.firstJumping = false;
					playerState.doubleJumping = false;
					playerState.velocity.y = 0;
				}
				else
				{
					//Bottom
					playerState.grounded = false;
					playerState.isOnMovingPlatform = false;
					playerState.velocity.y = movingPlatforms[i].GetCurrentVelocity().y;
				}
			}
		}
	}
	return hasCollided;
}

void Player::DoAttacks(std::vector<DestructibleObject> &destructibleObjects, bool shouldPlaySounds)
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
			if(shouldPlaySounds)
			{
				attackSound.play();
			}
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

//Play footstep sound when appropriate frame is displayed
void Player::DoPassiveSounds(bool shouldPlaySounds)
{
	//the footstep sound should play on these frames
	std::vector<int> footstepFrames;
	footstepFrames.push_back(4);
	footstepFrames.push_back(14);

	if(shouldPlaySounds == true)
	{
		if(sprite != nullptr)
		{
			if(sprite->IsPlaying())
			{
				//Footsteps
				if(sprite->GetCurrentAnimationName() == walkAnimName)
				{
					for(int i = 0; i < footstepFrames.size(); i++)
					{
						if(sprite->GetCurrentFrame() == footstepFrames[i])
						{
							if(playerState.grounded)
							{
								footStepSound.play();
							}
						}
					}
				}
				//Landing
				if(sprite->GetCurrentAnimationName() == landFromNormalJumpAnimName)
				{
					if(sprite->GetCurrentFrame() == 1)
					{
						if(playerState.grounded)
						{
							if(landSound.getStatus() != sf::Sound::Status::Playing)
							{
								landSound.play();
							}
						}
					}
				}
			}
		}
		//Fast fall sound
		//Start playing when the velocity is high, stop when it isnt
		if(fallingFastSound.getStatus() != sf::Sound::Status::Playing)
		{
			if(playerState.velocity.y > fallingFastTriggerSpeed)
			{
				fallingFastSound.play();
			}
		}
		else
		{
			if(playerState.velocity.y < fallingFastTriggerSpeed)
			{
				fallingFastSound.stop();
			}
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

//Called before platform step
void Player::DetermineIfPlayerIsOnMovingPlatform(std::vector<SpecialPlatform> &movingPlatforms)
{
	//reset this to false at the begging so we can check if we're still on a platform below.
	playerState.isOnMovingPlatform = false;


	for(int i = 0; i < movingPlatforms.size(); i++)
	{
		//Check to see if we're standing on a moving platform by getting a point 1 pixels below the bottom of the collider. Uses two so we can check both left and right
		int pixelsDown = 5;
		sf::Vector2f standPointLeft;
		sf::Vector2f standPointRight;
		standPointLeft.x = GetCollider().left;
		standPointRight.x = GetCollider().left + GetCollider().width;
		standPointLeft.y = GetCollider().top + GetCollider().height + pixelsDown;
		standPointRight.y = GetCollider().top + GetCollider().height + pixelsDown;
		//check to see if either of these points are in a platform
		if((movingPlatforms[i].getGlobalBounds().contains(standPointLeft)) || (movingPlatforms[i].getGlobalBounds().contains(standPointRight)))
		{

			//then do the standing on platform stuff
			handleToStandingPlatform = &movingPlatforms[i];		
			playerState.isOnMovingPlatform = true;
		}
	}
}

//Makes it so the player moves with moving objects rather than sliding off them
void Player::AdjustPositionForMovingPlatforms(float deltaTime, std::vector<SpecialPlatform> &movingLevelCollisionBounds)
{
	if(playerState.isOnMovingPlatform)
	{
		if(handleToStandingPlatform != nullptr)
		{
			platformMoveVector = handleToStandingPlatform->getPosition() - handleToStandingPlatform->GetLastPosition();
			if(VectorMagnitude(platformMoveVector)  < 1000)
			{
				Move(platformMoveVector.x, platformMoveVector.y);
			}
		}
	}
}

void Player::HandleAnimations()
{
	//This whole routine is a mess, not really sure how to do it better though. 

	//Attack
	if((playerState.attacking) && (playerState.INPUT_Attack))
	{
		sprite->SetCurrentAnimation(kickAnimName);
		sprite->SetRepeating(false);
		sprite->Play();
		playerState.animState = PlayerState::AnimationState::Attacking;
	}
	
	//Transition out of attack
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

	//Jump
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

	//doubleJump
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

	//GointoFallAnim
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

	//Land
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
	
	//Go back to idle from land
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

	if((playerState.INPUT_MoveLeft) || (playerState.INPUT_MoveRight))
	{
		if(playerState.grounded == true)
		{
			//Walk
			if(((!lastState.INPUT_MoveLeft) && (!lastState.INPUT_MoveRight)) || (playerState.animState == PlayerState::AnimationState::Idle))
			{
				playerState.animState = PlayerState::AnimationState::Walk;
				sprite->SetCurrentAnimation(walkAnimName);
				sprite->ChangeAnimSpeed(walkAnimName,walkFrameTime);
				sprite->SetRepeating(true);
				sprite->Play();
			}

			//Sprint
			if(playerState.INPUT_IsSprinting)
			{	
				sprite->ChangeAnimSpeed(walkAnimName,sprintFrameTime);
			}
			else if(!playerState.INPUT_IsSprinting)
			{
				sprite->ChangeAnimSpeed(walkAnimName,walkFrameTime);
			}
		}
	}
				
	//Back to idle from walk
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

	//A bug with landing where if you jump and land without going into fall, you dont transition out BAD BAD BAD BAD MAKES HIS ARMS FLAPS SAD
	if((lastState.grounded == false) && (playerState.grounded == true))
	{
		if((playerState.animState != PlayerState::AnimationState::Landing) || (playerState.animState != PlayerState::AnimationState::Idle))
		{
			playerState.animState = PlayerState::AnimationState::Landing;
		}
	}
}

//Loads up all the co-ordingates for each player animation. Bit of a hodge-podge, but it works. Would almost certainly be better defined externally, but I'm in a rush here! Geez!
void Player::LoadAnimations()
{
	//Load Walk
	sprite->LoadSingleAnimation(sf::Vector2i(0,0),125,19,126,156,0.03f,walkAnimName);
	//Load Idle
	sprite->LoadSingleAnimation(sf::Vector2i(0,166),125,20,126,156,0.06f,idleAnimName);
	//Load Jump
	sprite->LoadSingleAnimation(sf::Vector2i(125,505),125,4,120,156,0.03f,jumpAnimName);
	//Load Fall from Single Jump
	sprite->LoadSingleAnimation(sf::Vector2i(625,505),125,6,120,156,0.03f,fallAnimName);
	//Load Fall From Double Jump
	sprite->LoadSingleAnimation(sf::Vector2i(1250,686),125,8,120,156,0.03f,doubleJumpToFallAnimName);
	//Load Land From Normal Jump
	sprite->LoadSingleAnimation(sf::Vector2i(1250,506),125,6,120,156,0.03f,landFromNormalJumpAnimName);
	//Load Double Jump
	sprite->LoadSingleAnimation(sf::Vector2i(0,686),125,10,120,156,0.03f,doubleJumpAnimName);
	//Load Kick
	sprite->LoadSingleAnimation(sf::Vector2i(0,863),125,15,120,156,0.03f,kickAnimName);

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

sf::Vector2f Player::GetVelocity()
{
	return playerState.velocity;
}

void Player::SetVelocity(sf::Vector2f velocity)
{
	playerState.velocity = velocity;
}

sf::FloatRect Player::GetCollider()
{
	return collisionRect;
}

void Player::SetInputs(bool INPUT_MoveLeft, bool INPUT_MoveRight, bool INPUT_Jump, bool INPUT_Attack, bool INPUT_IsSprinting)
{
	playerState.INPUT_MoveLeft = INPUT_MoveLeft;
	playerState.INPUT_MoveRight = INPUT_MoveRight;
	playerState.INPUT_Jump = INPUT_Jump;
	playerState.INPUT_Attack = INPUT_Attack;
	playerState.INPUT_IsSprinting = INPUT_IsSprinting;
}

void Player::SetIsAcceptingInput(bool isAcceptingInput)
{
	this->playerState.isAcceptingInput = isAcceptingInput;
}

bool Player::GetIsAcceptingInput()
{
	return playerState.isAcceptingInput;
}