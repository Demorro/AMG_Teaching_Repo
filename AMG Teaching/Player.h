
#pragma once
#include "SFML\Graphics.hpp"
#include "Assets.h"
#include "pugixml.hpp"
#include <iostream>
#include <sstream>
#include "DestructibleObject.h"
#include "AudioManager.h"
#include "SFML\Audio.hpp"

#define DEBUGPLAYER true

class Player
{
public:
	Player(std::string playerTexturePath, sf::Vector2f startPos, AudioManager &audioManager);
	~Player(void);

	//Runs the update logic, the rect vector is because this also runs the collision code
	void Update(sf::Event events, bool eventFired, double deltaTime, std::vector<sf::Rect<float>> &levelCollisionRects, std::vector<DestructibleObject> &destructibleObjects);
	void Render(sf::RenderWindow &window);

	void Move(float x, float y);

	sf::Vector2f GetPosition();
	void SetPosition(float xPos, float yPos);
	sf::FloatRect GetCollider();
	void SetPosition(sf::Vector2f position);

private:
	sf::Texture texture;
	sf::Sprite sprite;

	//Store a reference to the audio manager
	AudioManager *audioManager;

	bool Initialise(std::string playerTexturePath, sf::Vector2f startPos, AudioManager &audioManager);
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
	void HandleMovement(float deltaTime, std::vector<sf::Rect<float>> &levelCollisionRects);

	//Called in move, makes sure the attack collider is in the right place
	void HandleAttackColliderPositioning();
	
	//These functions here are just called in HandleMovement, i've just seperated them because handleMovement was becoming massive, also you can change the order easily
	//Does the left and right movement for the player, whether the player is on the ground or in the air
	void DoLeftAndRightMovement(float deltaTime);
	//Handles the jumping
	void DoJumping();
	//Deals with adding the drag, whether the player is on the ground or in the air, they both have different values
	void AddDrag(float deltaTime);
	//Pull the player towards the floor
	void AddGravity(float deltaTime);

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
	float attackRange;
	float attackDelay;
	
	//The rect used to check for collision when the player is attacking
	sf::Rect<float> attackCollider;
	//used to time the attackDelay
	sf::Clock attackTimer;

	sf::Sound jumpSound;
	sf::Sound attackSound;

	//This struct should store the complete state of the player, both in what it is currently doing and what it has been commanded to do last frame
	struct PlayerState
	{
		//These variables store the current absolute state of the player
		bool movingLeft;
		bool movingRight;
		bool facingLeft;
		bool facingRight;
		bool jumping;
		bool grounded;
		bool attacking;

		sf::Vector2f velocity;

		//These variables are updated from ReceiveControlInput() and are the last inputs received, should probably be set back to false every frame unless you're doing something weird.
		bool INPUT_MoveLeft;
		bool INPUT_MoveRight;
		bool INPUT_Jump;
		bool INPUT_Attack;

		PlayerState::PlayerState()
		{
			movingLeft = false;
			movingRight = false;
			facingLeft = false;
			//the guy starts facing right
			facingRight = true;
			jumping = false;
			grounded = false;
			attacking = false;
			INPUT_MoveLeft = false;
			INPUT_MoveRight = false;
			INPUT_Jump = false;
			INPUT_Attack = false;
			velocity = sf::Vector2f(0,0);
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
};

