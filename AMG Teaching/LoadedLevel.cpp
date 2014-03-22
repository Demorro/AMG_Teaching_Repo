#include "LoadedLevel.h"


LoadedLevel::LoadedLevel()
{
	backgroundColor = sf::Color::Black;
	spawnPosition = sf::Vector2f(0,0);
}

LoadedLevel::LoadedLevel(std::string levelDataPath)
{
	LoadLevelSounds();
	LoadLevel(levelDataPath);
}

LoadedLevel::~LoadedLevel(void)
{
}

void LoadedLevel::LoadLevelConfigDoc(std::string configPath)
{
	farFarParralaxSpeed = -0.9f;
	farParralaxSpeed = -0.6f;
	midParralaxSpeed = -0.4f;
	closeParralaxSpeed = 0.0f;
	foregroundParralaxSpeed = 0.0f;
	
	defaultPlatformFallDelay = 1.0f;
	defaultPlatformFallGravity = 50.0f;
	defaultPlatformTerminalVelocity = 1200.0f;

	defaultPlatformMoveSpeed = 50.0f;
	defaultPlatformMoveDistanceTillNextPathNode = 5.0f;

	pugi::xml_document levelConfigDoc;

	LoadXMLDoc(levelConfigDoc,configPath);
	pugi::xml_node configRoot = levelConfigDoc.child("LevelConfig");

	LoadNumericalValue(farFarParralaxSpeed,configRoot,"FarFarBackgroundParralax");
	LoadNumericalValue(farParralaxSpeed,configRoot,"FarBackgroundParralax");
	LoadNumericalValue(midParralaxSpeed,configRoot,"MidBackgroundParralax");
	LoadNumericalValue(closeParralaxSpeed,configRoot,"CloseBackgroundParralax");
	LoadNumericalValue(foregroundParralaxSpeed,configRoot,"ForegroundParralax");

	LoadNumericalValue(defaultPlatformFallDelay,configRoot,"DefaultPlatformFallDelay");
	LoadNumericalValue(defaultPlatformFallGravity,configRoot,"DefaultPlatformFallGravity");
	LoadNumericalValue(defaultPlatformTerminalVelocity,configRoot,"DefaultPlatformTerminalVelocity");

	LoadNumericalValue(defaultPlatformMoveSpeed,configRoot,"DefaultPlatformMoveSpeed");
	LoadNumericalValue(defaultPlatformMoveDistanceTillNextPathNode,configRoot,"DefaultDistanceUntillNextPathNode");
	
	if(LEVEL_DEBUG)
	{
		std::cout << std::endl;
		std::cout << "LEVEL CONFIG VALUES : " << std::endl;
		std::cout << "FarFarBackgroundParralax : " << farFarParralaxSpeed << std::endl;
		std::cout << "FarBackgroundParralax : " << farParralaxSpeed << std::endl;
		std::cout << "MidBackgroundParralax : " << midParralaxSpeed << std::endl;
		std::cout << "CloseBackgroundParralax : " << closeParralaxSpeed << std::endl;
		std::cout << "ForegroundParralax : " << foregroundParralaxSpeed << std::endl;
		std::cout << "DefaultPlatformFallDelay : " << defaultPlatformFallDelay << std::endl;
		std::cout << "DefaultPlatformFallGravity : " << defaultPlatformFallGravity << std::endl;
		std::cout << "DefaultPlatformTerminalVelocity : " << defaultPlatformTerminalVelocity << std::endl;
		std::cout << "DefaultPlatformMoveSpeed : " << defaultPlatformMoveSpeed << std::endl;
		std::cout << "DefaultDistanceUntillNextPathNode : " << defaultPlatformMoveDistanceTillNextPathNode << std::endl;
		std::cout << std::endl;
	}
}

void LoadedLevel::LoadLevelSounds()
{
	fallingSoundKey = "PlatformFallSound";

	//Load the falling platform sounds;
	sf::SoundBuffer fallingSoundBuffer;
	fallingSoundBuffer.loadFromFile(PLATFORMFALLSOUND);

	loadedMapSounds[fallingSoundKey] = std::unique_ptr<sf::SoundBuffer>(new sf::SoundBuffer(fallingSoundBuffer));
}

void LoadedLevel::Update(double deltaTime, Player &player, sf::Vector2f &cameraVelocity)
{
	//run the updates for the destructibles, but only if neccesary (ie the destroy animations are playing,)
	for(size_t i = 0; i < destructibleObjects.size(); i++)
	{
		destructibleObjects[i].Update(deltaTime);
	}


	for(size_t i = 0; i < specialPlatforms.size(); i++)
	{
		specialPlatforms[i].Update(deltaTime,player.GetCollider(),player.GetVelocity());

		//Check to see if we need to destroy any of the platforms
		if(specialPlatforms[i].ShouldDestroy())
		{
			specialPlatforms.erase(specialPlatforms.begin() + i);
		}
	}

	//run the update on the checkpoints
	for(int i = 0; i < checkPoints.size(); i++)
	{
		checkPoints[i].UpdateAnimations();
	}

	//run the update on the dialogue characters
	for(int i = 0; i < dialogueCharacters.size(); i++)
	{
		dialogueCharacters[i].Update(deltaTime,player);
	}

	HandleParralax(deltaTime,cameraVelocity);
}

void LoadedLevel::HandleParralax(float deltaTime, sf::Vector2f &cameraVelocity)
{
	for(int i = 0; i < farFarBackGroundSprites.size(); i++)
	{
		farFarBackGroundSprites[i].move(-cameraVelocity * farFarParralaxSpeed);
	}
	for(int i = 0; i < farBackGroundSprites.size(); i++)
	{
		farBackGroundSprites[i].move(-cameraVelocity * farParralaxSpeed);
	}
	for(int i = 0; i < midBackGroundSprites.size(); i++)
	{
		midBackGroundSprites[i].move(-cameraVelocity * midParralaxSpeed);
	}
	for(int i = 0; i < nearBackGroundSprites.size(); i++)
	{
		nearBackGroundSprites[i].move(-cameraVelocity * closeParralaxSpeed);
	}
	for(int i = 0; i < foregroundSprites.size(); i++)
	{
		foregroundSprites[i].move(-cameraVelocity * foregroundParralaxSpeed);
	}
}

bool LoadedLevel::LoadLevel(std::string levelPath)
{

	LoadLevelConfigDoc(LEVELDEFAULTSCONFIG);

	LoadXMLDoc(levelDoc,levelPath);

	//Movement Paths need to be loaded before the special platforms(or anything else that might make use of them)
	LoadLayer(MOVEMENTPATHS);
	LoadLayer(BACKGROUNDCOLOUR);
	LoadLayer(FARFARBACKGROUND);
	LoadLayer(FARBACKGROUND);
	LoadLayer(MIDBACKGROUND);
	LoadLayer(NEARBACKGROUND);
	LoadLayer(CHECKPOINTS);
	LoadLayer(DIALOGUECHARACTERS);
	LoadLayer(OBJECTS);
	LoadLayer(FOREGROUND);
	LoadLayer(COLLISION);
	LoadLayer(DESTRUCTIBLES);
	LoadLayer(SPECIALPLATFORMS);
	LoadLayer(LEVELMETADATA);

	return true;
}

void LoadedLevel::LoadLayer(LevelLayers layer)
{
	pugi::xml_node startNode;

	//nodeName is required because .value() returns a const char*, whereas we want to compare it to an std::string. Putting into nodeName does an implicit cast, which is cool
	std::string nodeName;
	//Scan through the document and fisnd the layer node that is the one we are trying to load
	for(pugi::xml_node beginNode = levelDoc.child("Level").child("Layers").first_child(); beginNode; beginNode = beginNode.next_sibling())
	{
		if(layer == BACKGROUNDCOLOUR)
		{
			nodeName = beginNode.attribute("Name").value();
			if(nodeName == "BackgroundColour")
			{
				startNode = beginNode;
			}
		}
		else if(layer == FARFARBACKGROUND)
		{
			nodeName = beginNode.attribute("Name").value();
			if(nodeName == "FarFarBackGround")
			{
				startNode = beginNode;
			}
		}
		else if(layer == FARBACKGROUND)
		{
			nodeName = beginNode.attribute("Name").value();
			if(nodeName == "FarBackGround")
			{
				startNode = beginNode;
			}
		}
		else if(layer == MIDBACKGROUND)
		{
			nodeName = beginNode.attribute("Name").value();
			if(nodeName == "MidBackGround")
			{
				startNode = beginNode;
			}
		}
		else if(layer == NEARBACKGROUND)
		{
			nodeName = beginNode.attribute("Name").value();
			if(nodeName == "NearBackGround")
			{
				startNode = beginNode;
			}
		}
		else if(layer == CHECKPOINTS)
		{
			nodeName = beginNode.attribute("Name").value();
			if(nodeName == "CheckPoints")
			{
				startNode = beginNode;
			}
		}
		else if(layer == DIALOGUECHARACTERS)
		{
			nodeName = beginNode.attribute("Name").value();
			if(nodeName == "DialogueCharacters")
			{
				startNode = beginNode;
			}
		}
		else if(layer == OBJECTS)
		{
			nodeName = beginNode.attribute("Name").value();
			if(nodeName == "Objects")
			{
				startNode = beginNode;
			}
		}
		else if(layer == FOREGROUND)
		{
			nodeName = beginNode.attribute("Name").value();
			if(nodeName == "Foreground")
			{
				startNode = beginNode;
			}
		}
		else if(layer == COLLISION)
		{
			nodeName = beginNode.attribute("Name").value();
			if(nodeName == "Collision")
			{
				startNode = beginNode;
			}
		}
		else if(layer == DESTRUCTIBLES)
		{
			nodeName = beginNode.attribute("Name").value();
			if(nodeName == "Destructibles")
			{
				startNode = beginNode;
			}
		}
		else if(layer == SPECIALPLATFORMS)
		{
			nodeName = beginNode.attribute("Name").value();
			if(nodeName == "SpecialPlatforms")
			{
				startNode = beginNode;
			}
		}
		else if(layer == MOVEMENTPATHS)
		{
			nodeName = beginNode.attribute("Name").value();
			if(nodeName == "MovementPaths")
			{
				startNode = beginNode;
			}
		}
		else if(layer == LEVELMETADATA)
		{
			nodeName = beginNode.attribute("Name").value();
			if(nodeName == "LevelMetaData")
			{
				startNode = beginNode;
			}
		}
		else
		{
			std::cout << "Error : Level::LoadLayer()" << std::endl;
			return;
		}
	}


	//Load in the background colour
	if(layer == BACKGROUNDCOLOUR)
	{
		for(pugi::xml_node traversalNode = startNode.first_child().first_child(); traversalNode; traversalNode = traversalNode.next_sibling())
		{
			//Get the RGBA values for the one entry
			int red;
			std::stringstream redStream(traversalNode.child("FillColor").child_value("R"));
			redStream >> red;

			int green;
			std::stringstream greenStream(traversalNode.child("FillColor").child_value("G"));
			greenStream >> green;

			int blue;
			std::stringstream blueStream(traversalNode.child("FillColor").child_value("B"));
			blueStream >> blue;

			int alpha;
			std::stringstream alphaStream(traversalNode.child("FillColor").child_value("A"));
			alphaStream >> alpha;

			backgroundColor.r = red;
			backgroundColor.b = blue;
			backgroundColor.g = green;
			backgroundColor.a = alpha;
		}
	}
	//The collision data, movement path data, levelmetadata and background colour needs to be loaded in differently to the normal sprite based texture data.
	else if((layer != COLLISION) && (layer != BACKGROUNDCOLOUR) && (layer != MOVEMENTPATHS) && (layer != LEVELMETADATA))
	{
		//Now we have the root node of the layer, start loading in dat data! Yeah Baby Yeah!
		for(pugi::xml_node traversalNode = startNode.first_child().first_child(); traversalNode; traversalNode = traversalNode.next_sibling())
		{
			//Get the X and Y positions of this object, and then convert the const chars* read into ints so we can use them
			int xPos;
			std::stringstream xStream(traversalNode.child("Position").child_value("X"));
			xStream >> xPos;
			int yPos;
			std::stringstream yStream(traversalNode.child("Position").child_value("Y"));
			yStream >> yPos;

			//Do the same for the rotation and scale
			float rotation;
			std::stringstream rotStream(traversalNode.child_value("Rotation"));
			rotStream >> rotation;
			//The rotation needs to be converted from radians to degrees
			rotation = RadToDeg(rotation);
			
			float xScale;
			std::stringstream xScaleStream(traversalNode.child("Scale").child_value("X"));
			xScaleStream >> xScale;
			float yScale;
			std::stringstream yScaleStream(traversalNode.child("Scale").child_value("Y"));
			yScaleStream >> yScale;

			//create a sprite for this object
			sf::Sprite objectSprite;

			//Load in the texture
			std::string relativeTexPath = traversalNode.child_value("texture_filename");
			std::string texName = traversalNode.child_value("asset_name");

			std::unique_ptr<sf::Texture> tex(new sf::Texture());
			//This if statement checks if the key is already in the map. If the key is already in the map loading another texture will be wasteful, so we just use the entry already stored
			if(loadedMapTextures.find(texName) != loadedMapTextures.end())
			{
				objectSprite.setTexture(*loadedMapTextures[texName]);
			}
			else
			{
				std::cout << "Loading Texture : " << relativeTexPath << std::endl;
				tex->loadFromFile("..\\" + relativeTexPath);
				loadedMapTextures[texName] = std::move(tex);
				objectSprite.setTexture(*loadedMapTextures[texName]);
			}

			objectSprite.setOrigin(objectSprite.getGlobalBounds().left + (objectSprite.getLocalBounds().width/2) , objectSprite.getGlobalBounds().top + (objectSprite.getLocalBounds().height/2));

			//load up the sprite with the extracted date
			objectSprite.setPosition(float(xPos),float(yPos));
			objectSprite.setRotation(rotation);
			objectSprite.setScale(xScale,yScale);

			//And now actually place the sprites into the correct storage for rendering
			if(layer == FARFARBACKGROUND)
			{
				//BigSprite bigSprite(objectSprite.getPosition(), *loadedMapTextures[texName]);
				farFarBackGroundSprites.push_back(objectSprite);
			}
			else if(layer == FARBACKGROUND)
			{
				farBackGroundSprites.push_back(objectSprite);
			}
			else if(layer == MIDBACKGROUND)
			{
				midBackGroundSprites.push_back(objectSprite);
			}
			else if(layer == NEARBACKGROUND)
			{
				nearBackGroundSprites.push_back(objectSprite);
			}
			else if(layer == CHECKPOINTS)
			{
				LoadCheckPoint(objectSprite.getPosition());
			}
			else if(layer == DIALOGUECHARACTERS)
			{
				LoadDialogueCharacter(objectSprite,relativeTexPath,*loadedMapTextures[texName]);
			}
			else if(layer == OBJECTS)
			{
				objectSprites.push_back(objectSprite);
			}
			else if(layer == FOREGROUND)
			{
				foregroundSprites.push_back(objectSprite);
			}
			else if(layer == DESTRUCTIBLES)
			{
				//Add a new destructible sprite to the destructibles container, at the moment it takes 3 sprites, so load em all up and pack em in!
				destructibleObjects.push_back(DestructibleObject(objectSprite,LoadDestroyedDebrisImage(objectSprite,texName,relativeTexPath),LoadDestroyedAudioFile(texName,relativeTexPath)));
			}
			else if(layer == SPECIALPLATFORMS)
			{
				LoadSpecialPlatform(traversalNode,objectSprite);
				//specialPlatforms.push_back(SpecialPlatform(objectSprite, defaultPlatformFallDelay, defaultPlatformFallGravity, defaultPlatformTerminalVelocity, &GetDeathZones(), audioManager));
			}
		}
	}
	//The collision layer needs to be loaded into rects
	else if(layer == COLLISION)
	{
		for(pugi::xml_node traversalNode = startNode.first_child().first_child(); traversalNode; traversalNode = traversalNode.next_sibling())
		{
			//Get the X and Y positions of this object, and then convert the const chars* read into ints so we can use them
			int xPos;
			std::stringstream xStream(traversalNode.child("Position").child_value("X"));
			xStream >> xPos;
			int yPos;
			std::stringstream yStream(traversalNode.child("Position").child_value("Y"));
			yStream >> yPos;

			//Get the dimensions
			int width;
			std::stringstream widthStream(traversalNode.child_value("Width"));
			widthStream >> width;
			int height;
			std::stringstream heightStream(traversalNode.child_value("Height"));
			heightStream >> height;

			//Create a rect with the parsed properties
			sf::Rect<float> loadedRect(xPos,yPos,width,height);
			//And finally push it onto the collision bounds storage vector

			if(layer == COLLISION)
			{
				collisionBounds.push_back(loadedRect);
			}
		}
	}
	//movement paths have their own special structure
	else if(layer == MOVEMENTPATHS)
	{
		for(pugi::xml_node traversalNode = startNode.first_child().first_child(); traversalNode; traversalNode = traversalNode.next_sibling())
		{
			LoadMovementPath(traversalNode);
		}
	}
	else if(layer == LEVELMETADATA)
	{
		//Load the death zone
		for(pugi::xml_node traversalNode = startNode.first_child().first_child(); traversalNode; traversalNode = traversalNode.next_sibling())
		{
			LoadLevelMetaData(traversalNode);
		}
	}
}

void LoadedLevel::LoadCheckPoint(sf::Vector2f checkPointPosition)
{
	std::cout << "Loading Checkpoints" << std::endl;

	//check if the checkpoint spritesheet is already in the map
	if (loadedMapTextures.find(CHECKPOINTSHEET) == loadedMapTextures.end() ) 
	{
		//load the checkpoint spritesheet
		std::unique_ptr<sf::Texture> checkPointTexture(new sf::Texture());
		checkPointTexture->loadFromFile(CHECKPOINTSHEET);
		loadedMapTextures[CHECKPOINTSHEET] = std::move(checkPointTexture);
	} 
	else 
	{
		//Checkpoint texture is already in the map
	}
	

	float startX = 0;
	float startY = 0;
	float subRectWidth = 120;
	float subRectHeight = 260;
	CheckPoint checkPoint(sf::IntRect(startX,startY,subRectWidth,subRectHeight));

	checkPoint.setTexture(*loadedMapTextures[CHECKPOINTSHEET]);
	checkPoint.setOrigin(checkPoint.getGlobalBounds().left + checkPoint.getGlobalBounds().width/2, checkPoint.getGlobalBounds().top + checkPoint.getGlobalBounds().height/2); //set the origin to the center
	checkPoint.setPosition(checkPointPosition);
	
	checkPoints.push_back(checkPoint);
}

void LoadedLevel::LoadDialogueCharacter(sf::Sprite &baseSprite, std::string originalRelativeTexPath, sf::Texture &charTexture)
{
	//Load the speech bubble that the character needs if we havn't already got it.
	if (loadedMapTextures.find(SPEECHBUBBLE) == loadedMapTextures.end() ) 
	{
		//load the checkpoint spritesheet
		std::unique_ptr<sf::Texture> speechBubbleTexture(new sf::Texture());
		speechBubbleTexture->loadFromFile(SPEECHBUBBLE);
		loadedMapTextures[SPEECHBUBBLE] = std::move(speechBubbleTexture);
	}
	else 
	{
		//Checkpoint texture is already in the map
	}

	//Get the full path the the xml file containing the speech text and audio paths
	std::string speechConfigFileName = "\\CharacterSpeechConfig.xml";
	std::string speechConfigPath = originalRelativeTexPath.substr(0,originalRelativeTexPath.find_last_of("\\"));
	speechConfigPath = "..\\" + speechConfigPath + speechConfigFileName;

	//load it in
	pugi::xml_document speechConfigDoc;
	LoadXMLDoc(speechConfigDoc,speechConfigPath);

	std::string speechNodeName = "Speech";
	std::string speechBubbleXOriginName = "SpeechBubbleOriginFromTopLeftX";
	std::string speechBubbleYOriginName = "SpeechBubbleOriginFromTopLeftY";
	std::string shouldLoopAttributeName = "PhrasesShouldLoop";
	std::string speechVolumeName = "SpeechVolume";
	std::string shouldDisplayBubbleNodeName = "ShouldDisplayBubble";
	std::string shouldWobbleNodeName = "ShouldWobble";

	bool shouldLoop = false;

	std::vector<std::string> speechText;
	std::vector<std::string> speechAudioPath;

	float speechBubbleXOriginPos = 0;
	float speechBubbleYOriginPos = 0;

	float speechVolume = 100;

	int noOfSpeechNodes = 0;

	bool charShouldHaveSpeechBubble = false;
	bool charShouldWobble = false;

	shouldLoop = speechConfigDoc.child("CharacterSpeechConfig").attribute(shouldLoopAttributeName.c_str()).as_bool();

	//loop through the nodes, loading up the speech vectors and getting the position of the speech bubble.
	for(pugi::xml_node node = speechConfigDoc.child("CharacterSpeechConfig").first_child(); node; node = node.next_sibling())
	{
		std::string nodeName = node.name();

		if(nodeName == speechNodeName)
		{
			noOfSpeechNodes++;
			std::string text = node.child_value("SpeechText");
			std::string audioFilePath = node.child_value("AudioFilePath");
			//process the audio file path so it points to the correct reletive file path from root folder.
			audioFilePath = "..\\" + originalRelativeTexPath.substr(0,originalRelativeTexPath.find_last_of("\\")) + "\\" + audioFilePath;

			//load the vectors
			speechText.push_back(text);
			speechAudioPath.push_back(audioFilePath);
		}
		else if(nodeName == speechBubbleXOriginName)
		{
			LoadNumericalValue(speechBubbleXOriginPos,speechConfigDoc.child("CharacterSpeechConfig"),speechBubbleXOriginName); //load the x origin position of speech bubble 
		}
		else if(nodeName == speechBubbleYOriginName)
		{
			LoadNumericalValue(speechBubbleYOriginPos,speechConfigDoc.child("CharacterSpeechConfig"),speechBubbleYOriginName); //load the y origin position of speech bubble
		}
		else if(nodeName == speechVolumeName)
		{
			LoadNumericalValue(speechVolume,speechConfigDoc.child("CharacterSpeechConfig"),speechVolumeName); //load the y origin position of speech bubble
		}
		else if(nodeName == shouldDisplayBubbleNodeName)
		{
			LoadBooleanValue(charShouldHaveSpeechBubble,speechConfigDoc.child("CharacterSpeechConfig"),shouldDisplayBubbleNodeName);
		}
		else if(nodeName == shouldWobbleNodeName)
		{
			LoadBooleanValue(charShouldWobble,speechConfigDoc.child("CharacterSpeechConfig"),shouldWobbleNodeName);
		}
	}

	//create a vector of pairs from the speechtext and audio path vectors, to be passed into the dialogue character constructor. first is text, second is audio path
	std::vector<std::pair<std::string,std::string>> textAndAudioPaths;
	for(int i = 0; i < noOfSpeechNodes; i++)
	{
		std::string textToPush = "";
		std::string pathToPush = "";

		if(speechText.size() > i)
		{
			textToPush = speechText[i];
		}
		if(speechAudioPath.size() > i)
		{
			pathToPush = speechAudioPath[i];
		}

		textAndAudioPaths.push_back(std::pair<std::string,std::string>(textToPush,pathToPush));
	}
	
	dialogueCharacters.push_back(DialogueCharacter(baseSprite.getPosition(),charTexture,textAndAudioPaths,sf::Vector2f(speechBubbleXOriginPos,speechBubbleYOriginPos),*loadedMapTextures[SPEECHBUBBLE],shouldLoop,speechVolume,charShouldHaveSpeechBubble,charShouldWobble));
	dialogueCharacters.back().Load();
}

void LoadedLevel::LoadLevelMetaData(pugi::xml_node &rootNode)
{
	//Load in the death zone
	std::string deathZoneObjectName = "DeathZone";
	std::string playerSpawnPosName = "PlayerStartPosition";
	std::string endZoneName = "EndZone";

	//A DeathZone is a rect with the start of the string being "DeathZone", so "DeathZone1" "DeathZoneBlocks" or "DeathZoneHoopyFrood" will all work
	std::string rectNameForDeathZone = rootNode.attribute("Name").as_string();
	if(rectNameForDeathZone.substr(0,deathZoneObjectName.size()) == deathZoneObjectName)
	{
		//Get the X and Y positions of this object, and then convert the const chars* read into ints so we can use them
		int xPos;
		std::stringstream xStream(rootNode.child("Position").child_value("X"));
		xStream >> xPos;
		int yPos;
		std::stringstream yStream(rootNode.child("Position").child_value("Y"));
		yStream >> yPos;

		//Get the dimensions
		int width;
		std::stringstream widthStream(rootNode.child_value("Width"));
		widthStream >> width;
		int height;
		std::stringstream heightStream(rootNode.child_value("Height"));
		heightStream >> height;

		//Create a rect with the parsed properties
		sf::Rect<float> loadedRect(xPos,yPos,width,height);
		deathZones.push_back(loadedRect);

		std::cout << "Loading DeathZone" << std::endl;
	}
	//Load in the spawn position
	if(rootNode.attribute("Name").value() == playerSpawnPosName)
	{
		//Get the X and Y positions of this object, and then convert the const chars* read into ints so we can use them
		int xPos;
		std::stringstream xStream(rootNode.child("Position").child_value("X"));
		xStream >> xPos;
		int yPos;
		std::stringstream yStream(rootNode.child("Position").child_value("Y"));
		yStream >> yPos;

		spawnPosition = sf::Vector2f(xPos,yPos);

		std::cout << "Loading PlayerStartPos" << std::endl;
	}
	//Load in the ending rectangles(they trigger the end of the level when the player touches em)
	if(rootNode.attribute("Name").value() == endZoneName)
	{
		//Get the X and Y positions of this object, and then convert the const chars* read into ints so we can use them
		int xPos;
		std::stringstream xStream(rootNode.child("Position").child_value("X"));
		xStream >> xPos;
		int yPos;
		std::stringstream yStream(rootNode.child("Position").child_value("Y"));
		yStream >> yPos;

		//Get the dimensions
		int width;
		std::stringstream widthStream(rootNode.child_value("Width"));
		widthStream >> width;
		int height;
		std::stringstream heightStream(rootNode.child_value("Height"));
		heightStream >> height;

		//Create a rect with the parsed properties
		sf::Rect<float> loadedRect(xPos,yPos,width,height);
		endZones.push_back(loadedRect);

		std::cout << "Loading EndZone" << std::endl;
	}
}

void LoadedLevel::LoadSpecialPlatform(pugi::xml_node &rootNode, sf::Sprite &baseSprite)
{
	SpecialPlatform platform(baseSprite,defaultPlatformFallDelay, defaultPlatformFallGravity, defaultPlatformTerminalVelocity, defaultPlatformMoveSpeed, defaultPlatformMoveDistanceTillNextPathNode, &GetDeathZones(),*loadedMapSounds[fallingSoundKey]);

	bool isAFallingPlatform;
	isAFallingPlatform = rootNode.child("CustomProperties").find_child_by_attribute("Property","Name","ShouldFall").child("boolean").text().as_bool();
	platform.SetIsAFallingPlatform(isAFallingPlatform);

	bool isAMovingPlatform;
	isAMovingPlatform = rootNode.child("CustomProperties").find_child_by_attribute("Property","Name","ShouldFollowPath").child("boolean").text().as_bool();
	platform.SetIsAMovingPlatform(isAMovingPlatform);

	std::string movementPathName;
	movementPathName = rootNode.child("CustomProperties").find_child_by_attribute("Property","Name","PathToFollow").text().as_string();


	//Check to see if the path name we've just read in actually exists
	if(isAMovingPlatform)
	{
		if (movementPaths.find(movementPathName) == movementPaths.end() ) 
		{
			// not found
			std::cout << "Error : FollowPath : " << movementPathName << "is assigned to a platform, but it dosent seem to exist. Solution : Reset the Matrix, Consult the Architect" << std::endl;
			//since theres no path, this can't be a moving platform
			platform.SetIsAMovingPlatform(false);
		} 
		else
		{
			// found
			platform.SetFollowingPath(&movementPaths[movementPathName]);
		}

		float platformMoveSpeed;
		platformMoveSpeed = rootNode.child("CustomProperties").find_child_by_attribute("Property","Name","Speed").child("string").text().as_float();
		//If its zero we're happy using the default
		if(platformMoveSpeed != 0)
		{
			platform.SetMoveSpeed(platformMoveSpeed);
		}
	}
	specialPlatforms.push_back(platform);
}

void LoadedLevel::ReloadFallingPlatforms()
{
	specialPlatforms.clear();
	LoadLayer(SPECIALPLATFORMS);
}

void LoadedLevel::LoadMovementPath(pugi::xml_node &rootNode)
{
	std::vector<sf::Vector2f>  movementPathPoints;

	std::string pathName = rootNode.attribute("Name").as_string();

	//Each movement path is just a collection of points, here we load in the absolute world points and pack it into a MovementPath object
	for(pugi::xml_node worldPointNode = rootNode.child("WorldPoints").first_child(); worldPointNode; worldPointNode = worldPointNode.next_sibling())
	{
		float pathPointXPos =  worldPointNode.child("X").text().as_float();
		float pathPointYPos =  worldPointNode.child("Y").text().as_float();

		movementPathPoints.push_back(sf::Vector2f(pathPointXPos,pathPointYPos));
	}

	bool pathLoops = rootNode.child("IsPolygon").text().as_bool();
	movementPaths[pathName] = (MovementPath(movementPathPoints,pathLoops));
	
}

sf::Sprite LoadedLevel::LoadDestroyedDebrisImage(sf::Sprite &originalSprite, std::string originalTextureName, std::string originalRelativeTexPath)
{
	//if we are here, we have just loaded in a non destructible sprite as a normal sprite
	//Create a sprite for the destroyed version of this object, the same as objectSprite for now
	sf::Sprite destroyedObjectSprite = originalSprite;

	//Load the destroyed object image
	std::unique_ptr<sf::Texture> destroyedTex(new sf::Texture());

	//Find the image in the same directory that is marked to be the destructible debtris, meaning it will be called DESTRUCTIBLEDEBRISNAME.
	//This means we need to change textureName.png or whatever to textureNameDebris.png, or something like that, so we have a unique marker in the map
	originalTextureName = originalTextureName + DESTRUCTIBLEDEBRISNAME;
	//The relative texture path is the same as the orginal image, except the actual image name is replaced by the DESTRUCTIBLEDEBRISNAME, normally just Debris.png
	originalRelativeTexPath = originalRelativeTexPath.substr(0,originalRelativeTexPath.find_last_of("\\"));
	//at this point the relative tex path is just to the folder that should contain all the destructible object assets for this particular object
	originalRelativeTexPath = originalRelativeTexPath + "\\" + DESTRUCTIBLEDEBRISNAME;

	//This if statement checks if the key is already in the map. If the key is already in the map loading another texture will be wasteful, so we just use the entry already stored
	if(loadedMapTextures.find(originalTextureName) != loadedMapTextures.end())
	{
		destroyedObjectSprite.setTexture(*loadedMapTextures[originalTextureName]);
	}
	else
	{
		std::cout << "Loading Texture : " << originalRelativeTexPath << std::endl;
		if(destroyedTex->loadFromFile("..\\" + originalRelativeTexPath))
		{
			loadedMapTextures[originalTextureName] = std::move(destroyedTex);
			destroyedObjectSprite.setTexture(*loadedMapTextures[originalTextureName]);	
		}
		else
		{
			std::cout << "Cannot find destroyed version of texture, should be at : " << originalRelativeTexPath << std::endl;
			std::cout << "FIX THIS IMMEDIATELY, returning out, undefined behaviour could result" << std::endl;
			return sf::Sprite();
		}			
	}
	
	//return the destroyed sprite all set up
	return destroyedObjectSprite;
}

sf::Sound LoadedLevel::LoadDestroyedAudioFile(std::string originalTextureName, std::string originalRelativeTexPath)
{
	//if we are here, we need to load in the audio for the destroyed image
	sf::Sound objectDestructionSound;

	//Load the destroyed object image
	std::unique_ptr<sf::SoundBuffer> destructionSound(new sf::SoundBuffer());

	//Find the sound in the same directory that is marked to be the destructible soun, meaning it will be called DESTRUCTIBLESOUNDNAME.
	originalTextureName = originalTextureName + DESTRUCTIBLESOUNDNAME;
	//The relative texture path is the same as the orginal image, except the actual image name is replaced by the DESTRUCTIBLEDEBRISNAME, normally just Debris.png
	originalRelativeTexPath = originalRelativeTexPath.substr(0,originalRelativeTexPath.find_last_of("\\"));
	//at this point the relative tex path is just to the folder that should contain all the destructible object assets for this particular object
	originalRelativeTexPath = originalRelativeTexPath + "\\" + DESTRUCTIBLESOUNDNAME;

	//This if statement checks if the key is already in the map. If the key is already in the map loading another texture will be wasteful, so we just use the entry already stored
	if(loadedMapSounds.find(originalTextureName) != loadedMapSounds.end())
	{
		objectDestructionSound.setBuffer(*loadedMapSounds[originalTextureName]);
	}
	else
	{
		std::cout << "Loading Sound : " << originalRelativeTexPath << std::endl;
		if(destructionSound->loadFromFile("..\\" + originalRelativeTexPath))
		{
			loadedMapSounds[originalTextureName] = std::move(destructionSound);
			objectDestructionSound.setBuffer(*loadedMapSounds[originalTextureName]);	
		}
		else
		{
			std::cout << "Cannot find destroyed version of sound, should be at : " << originalRelativeTexPath << std::endl;
			std::cout << "FIX THIS IMMEDIATELY, returning out, undefined behaviour could result" << std::endl;
			return sf::Sound();
		}			
	}
	
	//return the destroyed sprite all set up
	return objectDestructionSound;
}

std::vector<sf::Rect<float>> LoadedLevel::GetStaticCollisionBounds()
{
	//Done this way so we can add any non-static bounds to the end of it and deal with it in one go
	std::vector<sf::Rect<float>> returnedBounds;
	returnedBounds = collisionBounds;

	//Add the collision bounds to any destructable that isnt destroyed
	for(size_t i = 0; i < destructibleObjects.size(); i++)
	{
		if(destructibleObjects[i].GetDestructibleState() == DestructibleObject::Intact)
		{
			returnedBounds.push_back(destructibleObjects[i].GetCollisionRect());
		}
	}

	return returnedBounds;
}

std::vector<MovingCollider> LoadedLevel::GetMovingCollisionBounds()
{
	std::vector<MovingCollider> returnedBounds;
	//Add the collision bounds for any of the falling platforms
	for(size_t i = 0; i < specialPlatforms.size(); i++)
	{
		MovingCollider specialPlatformCollider;
		specialPlatformCollider.collisionBound = specialPlatforms[i].getGlobalBounds();
		specialPlatformCollider.velocity = specialPlatforms[i].GetCurrentVelocity();
		specialPlatformCollider.centerPos = specialPlatforms[i].getPosition();
		specialPlatformCollider.lastCenterPos = specialPlatforms[i].GetLastPosition();
		returnedBounds.push_back(specialPlatformCollider);
	}

	return returnedBounds;
}
std::vector<DestructibleObject> &LoadedLevel::GetDestructibleObjects()
{
	return destructibleObjects;
}

std::vector<SpecialPlatform> &LoadedLevel::GetSpecialPlatforms()
{
	return specialPlatforms;
}

std::vector<sf::Rect<float>> &LoadedLevel::GetDeathZones()
{
	return deathZones;
}

std::vector<sf::Rect<float>> &LoadedLevel::GetEndZones()
{
	return endZones;
}

std::vector<CheckPoint> &LoadedLevel::GetCheckPoints()
{
	return checkPoints;
}

std::map<std::string,MovementPath> &LoadedLevel::GetMovementPaths()
{
	return movementPaths;
}

sf::Vector2f LoadedLevel::GetSpawnPosition()
{
	return spawnPosition;
}

void LoadedLevel::DrawLayersBehindPlayer(sf::RenderWindow &window)
{
	//Clear the screen to the sky colour
	window.clear(backgroundColor);


	for(size_t i = 0; i < farFarBackGroundSprites.size(); i++)
	{
		window.draw(farFarBackGroundSprites[i]);
	}
	for(size_t i = 0; i < farBackGroundSprites.size(); i++)
	{
		window.draw(farBackGroundSprites[i]);
	}
	for(size_t i = 0; i < midBackGroundSprites.size(); i++)
	{
		window.draw(midBackGroundSprites[i]);
	}
	for(size_t i = 0; i < nearBackGroundSprites.size(); i++)
	{
		window.draw(nearBackGroundSprites[i]);
	}
	for(size_t i = 0; i < checkPoints.size(); i++)
	{
		window.draw(checkPoints[i]);
	}
	for(size_t i = 0; i < dialogueCharacters.size(); i++)
	{
		dialogueCharacters[i].Render(window);
	}
	for(size_t i = 0; i < destructibleObjects.size(); i++)
	{
		destructibleObjects[i].Render(window);
	}
	for(size_t i = 0; i < objectSprites.size(); i++)
	{
		window.draw(objectSprites[i]);
	}
	for(size_t i = 0; i < specialPlatforms.size(); i++)
	{
		window.draw(specialPlatforms[i]);
	}

	//Draw the collision bounds for debugging
	if(LEVEL_DEBUG)
	{
		for(size_t i = 0; i < GetStaticCollisionBounds().size(); i++)
		{
			sf::RectangleShape debugDrawRect;
			debugDrawRect.setSize(sf::Vector2f(GetStaticCollisionBounds()[i].width,GetStaticCollisionBounds()[i].height));
			debugDrawRect.setPosition(GetStaticCollisionBounds()[i].left,GetStaticCollisionBounds()[i].top);
			debugDrawRect.setFillColor(sf::Color::Transparent);
			debugDrawRect.setOutlineThickness(1.0f);
			debugDrawRect.setOutlineColor(sf::Color::Red);
			window.draw(debugDrawRect);
		}

		for(size_t i = 0; i < GetMovingCollisionBounds().size(); i++)
		{
			sf::RectangleShape debugDrawRect;
			debugDrawRect.setSize(sf::Vector2f(GetMovingCollisionBounds()[i].collisionBound.width,GetMovingCollisionBounds()[i].collisionBound.height));
			debugDrawRect.setPosition(GetMovingCollisionBounds()[i].collisionBound.left,GetMovingCollisionBounds()[i].collisionBound.top);
			debugDrawRect.setFillColor(sf::Color::Transparent);
			debugDrawRect.setOutlineThickness(1.0f);
			debugDrawRect.setOutlineColor(sf::Color::Blue);
			window.draw(debugDrawRect);
			
		}
	}
}

void LoadedLevel::DrawLayersInFrontOfPlayer(sf::RenderWindow &window)
{
	for(size_t i = 0; i < foregroundSprites.size(); i++)
	{
		window.draw(foregroundSprites[i]);
	}
}

