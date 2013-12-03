
#pragma once
#include "SFML\Graphics.hpp"
#include "Assets.h"
#include "pugixml.hpp"
#include <iostream>
#include <sstream>
#include "DestructibleObject.h"
#include "AudioManager.h"
#include "SFML\Audio.hpp"
#include <math.h>
#include "AnimatedSprite.h"

#define DEBUGPLAYER false

class Player
{
public:

	enum Abilities
	{
		DoubleJump
	};

	Player(std::string playerTexturePath, sf::Vector2f startPos, sf::IntRect startTextureRect, sf::IntRect boundsRect, AudioManager &audioManager);
	~Player(void);

	//Runs the update logic, the rect vector is because this also runs the collision code
	void Update(sf::Event events, bool eventFired, double deltaTime, std::vector<sf::Rect<float>> &levelCollisionRects, std::vector<DestructibleObject> &destructibleObjects);
	void Render(sf::RenderWindow &window);

	void Move(float x, float y);

	sf::Vector2f GetPosition();
	void SetPosition(float xPos, float yPos);
	sf::FloatRect GetCollider();
	void SetPosition(sf::Vector2f position);

	//Give or take an ability from the player, this function needs to be maintained when a new ability is added, along with the Abilites enum
	void ToggleAbility(Abilities ability, bool active);

private:
	//these values are stored here so we can flip the sprite and keep the same scale
	float loadedScaleX;
	float loadedScaleY;

	sf::Texture spriteSheet;
	std::unique_ptr<AnimatedSprite> sprite;


	std::string walkAnimName;
	std::string idleAnimName;
	std::string startJumpAnimName;
	std::string jumpAnimName;
	std::string fallAnimName;
	std::string landFromNormalJumpAnimName;
	std::string doubleJumpAnimName;
	std::string doubleJumpToFallAnimName;
	std::string kickAnimName;

	//Loads the player specific animations
	void LoadAnimations();
	void LoadSingleAnimation(sf::Vector2i startFrame, int xFrameOffset, int noOfFrames, int frameWidth, int frameHeight, float frameTime, std::string animationName);
	//Makes sure the correct animations are playing
	void HandleAnimations();

	//Store a reference to the audio manager
	AudioManager *audioManager;

	sf::Rect<float> collisionRect;

	bool Initialise(std::string playerTexturePath, sf::Vector2f startPos, sf::IntRect startTextureRect, sf::IntRect boundsRect, AudioManager &audioManager);
	//Loads the config values from the default config file if it can be found, else just plugs in defaults
	bool LoadConfigValues(std::string configFilePath);
	//A generic function that loads in a numerical value from the XML in the structure of the provided config file and puts it into the float thats passed in.
	void LoadNumericalValue(float &valueToLoadInto, pugi::xml_node &rootNode, std::string valueNodeName);

	//This function polls the input devices and updates the player state accordingly.
	void ReceiveControlInput(sf::Event events, bool eventFired);
	//All these vectors are set in Initialise, may seem a bit unnescesary, but doing it this way means we can easily change what keys control what, have as many keys as we like do one thing without loads of ifs, + allows easy support for in-game control remapping if neccesary
	std::vector<sf::Keyboard::Key> moveLeftKeys;
	std::vector<sf::Keyboard::Key> moveRightKeys;
	std::vector<sf::Keyboard::Key> jumpKeys;
	std::vector<sf::Keyboard::Key> attackKeys;

	//Reads the current state of input from the playerstate and deals with moving
	void HandleMovement(sf::Event events, bool eventFired, double deltaTime, std::vector<sf::Rect<float>> &levelCollisionRects);

	//Called in move, makes sure the attack collider is in the right place
	void HandleAttackColliderPositioning();
	
	//These functions here are just called in HandleMovement, i've just seperated them because handleMovement was becoming massive, also you can change the order easily
	//Does the left and right movement for the player, whether the player is on the ground or in the air
	void DoLeftAndRightMovement(double deltaTime);
	//Handles the jumping
	void DoJumping(sf::Event events, bool eventFired);
	//Deals with adding the drag, whether the player is on the ground or in the air, they both have different values
	void AddDrag(double deltaTime);
	//Pull the player towards the floor
	void AddGravity(double deltaTime);

	//Handle collision, done seperately because it's simpler than doing corner-exception cases
	void HandleHorizontalCollision(std::vector<sf::Rect<float>> &levelCollisionRects);
	void HandleVerticalCollision(std::vector<sf::Rect<float>> &levelCollisionRects);

	//Handles the attacking logic
	void DoAttacks(std::vector<DestructibleObject> &destructibleObjects);

	//These variables define how the player moves, loaded in in LoadConfigValues, which itself should be called in initialise
	float maximumHorizontalSpeed;
	float airAcceleration;
	float groundAcceleration;
	float airDrag;
	float groundDrag;
	float personalGravity;
	float terminalVelocity;
	float jumpStrength;
	float doubleJumpStrength;
	float doubleJumpVelocityChangeImpulse;
	float attackRange;
	float attackDelay;
	
	//The rect used to check for collision when the player is attacking
	sf::Rect<float> attackCollider;
	//used to time the attackDelay
	sf::Clock attackTimer;

	//used to ensure that the jumpkeys hasnt been down for a small amount of time so double jump dosent get triggered stupidly.
	sf::Clock doubleJumpKeyTimer;
	float doubleJumpKeyTime;

	sf::Sound jumpSound;
	sf::Sound attackSound;
	sf::Sound fartSound;

	//This struct should store the complete state of the player, both in what it is currently doing and what it has been commanded to do last frame
	struct PlayerState
	{
		//These variables store the current absolute state of the player
		bool movingLeft;
		bool movingRight;
		bool facingLeft;
		bool facingRight;
		bool firstJumping;
		bool doubleJumping;
		bool grounded;
		bool attacking;

		sf::Vector2f velocity;

		//these variables can be toggled to define what abilites the player has
		bool canDoubleJump;

		//These variables are updated from ReceiveControlInput() and are the last inputs received, should probably be set back to false every frame unless you're doing something weird.
		bool INPUT_MoveLeft;
		bool INPUT_MoveRight;
		bool INPUT_Jump;
		bool INPUT_Attack;

		enum AnimationState
		{
			Idle,
			Walk,
			StartJump,
			FirstJumping,
			DoubleJumping,
			Falling,
			Landing,
			Attacking,
		};

		AnimationState animState;

		PlayerState::PlayerState()
		{
			movingLeft = false;
			movingRight = false;
			facingLeft = false;
			//the guy starts facing right
			facingRight = true;
			firstJumping = false;
			doubleJumping = false;
			grounded = false;
			attacking = false;
			INPUT_MoveLeft = false;
			INPUT_MoveRight = false;
			INPUT_Jump = false;
			INPUT_Attack = false;
			canDoubleJump = false;
			velocity = sf::Vector2f(0,0);
			animState = AnimationState::Idle;
		}

		void PlayerState::ResetInputs()
		{
			INPUT_MoveLeft = false;
			INPUT_MoveRight = false;
			INPUT_Jump = false;
			INPUT_Attack = false;
		}
	};
	PlayerState playerState;
	//the state the player was in last frame
	PlayerState lastState;

	//The downwards speed at which the fall animation triggers
	float fallVelocityTillFallAnimation;
};

