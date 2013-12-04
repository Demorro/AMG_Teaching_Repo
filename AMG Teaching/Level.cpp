#include "Level.h"


Level::Level(AudioManager *audioManager)
{
	this->audioManager = audioManager;
	backgroundColor = sf::Color::Black;
}

Level::Level(std::string levelDataPath, AudioManager *audioManager)
{
	this->audioManager = audioManager;

	//This is probably as good as place as any to load the level specific audio
	if(audioManager != NULL)
	{
		audioManager->LoadSoundFile(PLATFORMFALLSOUND,AudioManager::PlatformFall);
	}

	LoadLevel(levelDataPath);
}

Level::~Level(void)
{
}

void Level::LoadLevelConfigDoc(std::string configPath)
{
	farParralaxSpeed = 0.2f;
	midParralaxSpeed = 0.6f;
	closeParralaxSpeed = 0.9f;
	
	defaultPlatformFallDelay = 1.0f;
	defaultPlatformFallGravity = 50.0f;
	defaultPlatformTerminalVelocity = 1200.0f;

	pugi::xml_document levelConfigDoc;

	LoadXMLDoc(levelConfigDoc,configPath);
	pugi::xml_node configRoot = levelConfigDoc.child("LevelConfig");

	LoadNumericalValue(farParralaxSpeed,configRoot,"FarBackgroundParralax");
	LoadNumericalValue(midParralaxSpeed,configRoot,"MidBackgroundParralax");
	LoadNumericalValue(closeParralaxSpeed,configRoot,"CloseBackgroundParralax");

	LoadNumericalValue(defaultPlatformFallDelay,configRoot,"DefaultPlatformFallDelay");
	LoadNumericalValue(defaultPlatformFallGravity,configRoot,"DefaultPlatformFallGravity");
	LoadNumericalValue(defaultPlatformTerminalVelocity,configRoot,"DefaultPlatformTerminalVelocity");

	if(LEVEL_DEBUG)
	{
		std::cout << std::endl;
		std::cout << "LEVEL CONFIG VALUES : " << std::endl;
		std::cout << "FarBackgroundParralax : " << farParralaxSpeed << std::endl;
		std::cout << "MidBackgroundParralax : " << midParralaxSpeed << std::endl;
		std::cout << "CloseBackgroundParralax : " << closeParralaxSpeed << std::endl;
		std::cout << "DefaultPlatformFallDelay : " << defaultPlatformFallDelay << std::endl;
		std::cout << "DefaultPlatformFallGravity : " << defaultPlatformFallGravity << std::endl;
		std::cout << "DefaultPlatformTerminalVelocity : " << defaultPlatformTerminalVelocity << std::endl;
		std::cout << std::endl;
	}
}

void Level::Update(double deltaTime, Player &player, sf::Vector2f &cameraVelocity)
{
	//run the updates for the destructibles, but only if neccesary (ie the destroy animations are playing,)
	for(size_t i = 0; i < destructibleObjects.size(); i++)
	{
		destructibleObjects[i].Update(deltaTime);
	}

	for(size_t i = 0; i < fallingPlatforms.size(); i++)
	{
		fallingPlatforms[i].Update(deltaTime,player);

		//Check to see if we need to destroy any of the platforms
		if(fallingPlatforms[i].ShouldDestroy())
		{
			fallingPlatforms.erase(fallingPlatforms.begin() + i);
		}
	}

	HandleParralax(deltaTime,cameraVelocity);
}

void Level::HandleParralax(float deltaTime, sf::Vector2f &cameraVelocity)
{
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
}

bool Level::LoadLevel(std::string levelPath)
{

	LoadLevelConfigDoc(LEVELCONFIG);

	LoadXMLDoc(levelDoc,levelPath);

	LoadLayer(BACKGROUNDCOLOUR);
	LoadLayer(FARBACKGROUND);
	LoadLayer(MIDBACKGROUND);
	LoadLayer(NEARBACKGROUND);
	LoadLayer(OBJECTS);
	LoadLayer(FOREGROUND);
	LoadLayer(COLLISION);
	LoadLayer(DESTRUCTIBLES);
	LoadLayer(FALLINGPLATFORMS);
	LoadLayer(DEATHZONES);

	return true;
}

void Level::LoadLayer(LevelLayers layer)
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
		else if(layer == FALLINGPLATFORMS)
		{
			nodeName = beginNode.attribute("Name").value();
			if(nodeName == "FallingPlatforms")
			{
				startNode = beginNode;
			}
		}
		else if(layer == DEATHZONES)
		{
			nodeName = beginNode.attribute("Name").value();
			if(nodeName == "THEDEATHZONE")
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
	//The collision data, death zone data and background colour needs to be loaded in differently to the normal sprite based texture data.
	else if((layer != COLLISION) && (layer != DEATHZONES) && (layer != BACKGROUNDCOLOUR))
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
			if(layer == FARBACKGROUND)
			{
				farBackGroundSprites.push_back(objectSprite);
			}
			if(layer == MIDBACKGROUND)
			{
				midBackGroundSprites.push_back(objectSprite);
			}
			if(layer == NEARBACKGROUND)
			{
				nearBackGroundSprites.push_back(objectSprite);
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
			else if(layer == FALLINGPLATFORMS)
			{
				fallingPlatforms.push_back(FallingPlatform(objectSprite, defaultPlatformFallDelay, defaultPlatformFallGravity, defaultPlatformTerminalVelocity, &GetDeathZones(), audioManager));
			}
		}
	}
	//The collision layer needs to be loaded into rects
	else if((layer == COLLISION) || (layer == DEATHZONES))
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
			else if(layer == DEATHZONES)
			{
				deathZones.push_back(loadedRect);
			}
		}
	}
}

sf::Sprite Level::LoadDestroyedDebrisImage(sf::Sprite &originalSprite, std::string originalTextureName, std::string originalRelativeTexPath)
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

sf::Sound Level::LoadDestroyedAudioFile(std::string originalTextureName, std::string originalRelativeTexPath)
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

std::vector<sf::Rect<float>> Level::GetCollisionBounds()
{
	//Done this way so we can add any non-static bounds to the end of it and deal with it in one go
	std::vector<sf::Rect<float>> returnedBounds;
	returnedBounds = collisionBounds;


	//Add the collision bounds for any of the falling platforms
	for(size_t i = 0; i < fallingPlatforms.size(); i++)
	{
		returnedBounds.push_back(fallingPlatforms[i].getGlobalBounds());
	}

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

std::vector<DestructibleObject> &Level::GetDestructibleObjects()
{
	return destructibleObjects;
}

std::vector<FallingPlatform> &Level::GetFallingPlatforms()
{
	return fallingPlatforms;
}

std::vector<sf::Rect<float>> &Level::GetDeathZones()
{
	return deathZones;
}

void Level::Draw(sf::RenderWindow &window)
{
	//Clear the screen to the sky colour
	window.clear(backgroundColor);

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
	for(size_t i = 0; i < destructibleObjects.size(); i++)
	{
		destructibleObjects[i].Render(window);
	}
	for(size_t i = 0; i < objectSprites.size(); i++)
	{
		window.draw(objectSprites[i]);
	}
	for(size_t i = 0; i < fallingPlatforms.size(); i++)
	{
		window.draw(fallingPlatforms[i]);
	}
	for(size_t i = 0; i < foregroundSprites.size(); i++)
	{
		window.draw(foregroundSprites[i]);
	}

	//Draw the collision bounds for debugging
	if(LEVEL_DEBUG)
	{
		for(size_t i = 0; i < GetCollisionBounds().size(); i++)
		{
			sf::RectangleShape debugDrawRect;
			debugDrawRect.setSize(sf::Vector2f(GetCollisionBounds()[i].width,GetCollisionBounds()[i].height));
			debugDrawRect.setPosition(GetCollisionBounds()[i].left,GetCollisionBounds()[i].top);
			debugDrawRect.setFillColor(sf::Color::Transparent);
			debugDrawRect.setOutlineThickness(1.0f);
			debugDrawRect.setOutlineColor(sf::Color::Red);
			window.draw(debugDrawRect);
			
		}
	}
}

