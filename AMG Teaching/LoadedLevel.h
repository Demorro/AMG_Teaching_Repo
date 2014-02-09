#pragma once
#include <iostream>
#include <vector>
#include <map>
#include "SFML\Graphics.hpp"
#include "pugixml.hpp"
#include <sstream>
#include <memory>
#include "VectorMath.h"
#include "Assets.h"
#include "DestructibleObject.h"
#include "SFML\Audio.hpp"
#include "XMLParseUtilities.h"
#include "SpecialPlatform.h"
#include "Player.h"
#include "MovementPath.h"
#include "MovingCollider.h"
#include "AnimatedSprite.h"
#include "CheckPoint.h"

//you should do a check against this if you're drawing any debug sprites so it can be toggled easily
#define LEVEL_DEBUG false


class LoadedLevel
{
public:
	LoadedLevel();
	//Pass in the path to the desired levels XML file, should be in Assets/LoadedLevels
	LoadedLevel(std::string levelDataPath);
	~LoadedLevel(void);

	//Loads the level in from the xml file found at levelPath
	bool LoadLevel(std::string levelPath);

	//Runs any logic the level needs to do
	//Takes in the player so level objects can work out stuff, the cameraPos is for parralax
	void Update(double deltaTime, Player &player, sf::Vector2f &cameraVelocity);

	//Renders the layers that are behind the players
	void DrawLayersBehindPlayer(sf::RenderWindow &window);

	//Renders the layers that are in front of the players
	void DrawLayersInFrontOfPlayer(sf::RenderWindow &window);


	//Returns the collision rects so they can be iterated over to check for collision, only the still ones
	std::vector<sf::Rect<float>> GetStaticCollisionBounds();

	//Returns the collision bounds + velocity of moving objects so they can be iterated over, seperate cause the player has to know what is moving so the velocity can be added
	std::vector<MovingCollider> GetMovingCollisionBounds();

	//Returns all the destructible objects in the level
	std::vector<DestructibleObject> &GetDestructibleObjects();

	//Returns all the falling platforms in the level
	std::vector<SpecialPlatform> &GetSpecialPlatforms();

	//Returns the rects of the death zones
	std::vector<sf::Rect<float>> &GetDeathZones();

	//Returns the rects of the end zones
	std::vector<sf::Rect<float>> &GetEndZones();

	//Returns the animated sprite objects that are the checkpoints
	std::vector<CheckPoint> &GetCheckPoints();

	//Returns the movement paths
	std::map<std::string,MovementPath> &GetMovementPaths();

	//Returns the spawn position
	sf::Vector2f GetSpawnPosition();

private:
	//These functions handle loading the parts of the level that need something beyond a basic sprite and collision or a rect, just better encapsulated here.
	void LoadSpecialPlatform(pugi::xml_node &rootNode, sf::Sprite &baseSprite);
	void LoadMovementPath(pugi::xml_node &rootNode);

	void LoadLevelMetaData(pugi::xml_node &rootNode);
	sf::Vector2f spawnPosition;

	void LoadLevelConfigDoc(std::string configPath);
	
	float farFarParralaxSpeed;
	float farParralaxSpeed;
	float midParralaxSpeed;
	float closeParralaxSpeed;
	float foregroundParralaxSpeed;
	
	float defaultPlatformFallDelay;
	float defaultPlatformFallGravity;
	float defaultPlatformTerminalVelocity;

	float defaultPlatformMoveSpeed;
	float defaultPlatformMoveDistanceTillNextPathNode;


	//The xml file that we load in to define the level
	pugi::xml_document levelDoc;

	enum LevelLayers
	{
		BACKGROUNDCOLOUR,
		FARFARBACKGROUND,
		FARBACKGROUND,
		MIDBACKGROUND,
		NEARBACKGROUND,
		CHECKPOINTS,
		OBJECTS,
		FOREGROUND,
		COLLISION,
		DESTRUCTIBLES,
		SPECIALPLATFORMS,
		MOVEMENTPATHS,
		LEVELMETADATA,
	};


	//The background colour of the level
	sf::Color backgroundColor;

	//Loads a layer into the level, done like this so we can load layers in different orders if we want.
	void LoadLayer(LevelLayers layer);

	//The textures the level uses are stored here. Some fancy stuff is done so one texture is never duplicated to save on the memory footprint and increase performence.
	std::map<std::string, std::unique_ptr<sf::Texture>> loadedMapTextures;
	//The sounds the level uses are stored here, yadayadayada
	std::map<std::string, std::unique_ptr<sf::SoundBuffer>> loadedMapSounds;

	std::vector<sf::Sprite> farFarBackGroundSprites;
	std::vector<sf::Sprite> farBackGroundSprites;
	std::vector<sf::Sprite> midBackGroundSprites;
	std::vector<sf::Sprite> nearBackGroundSprites;
	std::vector<sf::Sprite> objectSprites;
	std::vector<sf::Sprite> foregroundSprites;
	std::vector<CheckPoint> checkPoints;
	std::vector<sf::Rect<float>> collisionBounds;
	std::vector<sf::Rect<float>> deathZones;
	std::vector<sf::Rect<float>> endZones;
	std::vector<SpecialPlatform> specialPlatforms;
	//This is a map as other objects need to reference these by name to allow loading in from the level editor
	std::map<std::string,MovementPath> movementPaths;
	//The first sprite is the normal, non destructed object, while the second is the destroyed sprite. These MUST be the same size.
	std::vector<DestructibleObject> destructibleObjects;

	void LoadCheckPoint(sf::Vector2f checkPointPosition);

	//the keys for the level sounds in the map;
	std::string fallingSoundKey;
	//Loads the sounds the level needs (such as the platform fall sounds) and puts them into the loadedMapSounds map
	void LoadLevelSounds();

	//loads in the ancillary assets for the destructibles and puts them into the relevent containers
	//Give em the original loaded sprite from the destructibles layer, loaded in using the normal sprite loading code, then the texture name and relative texture name already parsed using the regular sprite loading code.
	sf::Sprite LoadDestroyedDebrisImage(sf::Sprite &originalSprite, std::string originalTextureName, std::string originalRelativeTexPath);
	sf::Sound LoadDestroyedAudioFile(std::string originalTextureName, std::string originalRelativeTexPath);

	void HandleParralax(float deltaTime, sf::Vector2f &cameraVelocity);
};

