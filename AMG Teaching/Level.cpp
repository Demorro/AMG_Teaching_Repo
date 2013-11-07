#include "Level.h"


Level::Level()
{
	backgroundColor = sf::Color::Black;
}

Level::Level(std::string levelDataPath)
{
	LoadLevel(levelDataPath);
	backgroundColor = sf::Color::Black;
}

Level::~Level(void)
{
}

bool Level::LoadLevel(std::string levelPath)
{

	//Load the xml file into memory
	std::cout << "Loading Level : " << levelPath << std::endl;
	pugi::xml_parse_result result = levelDoc.load_file(levelPath.c_str());
	if (result)
	{
		std::cout << "XML File [" << levelPath << "] was loaded without errors." << std::endl;
	}
	else
	{
		std::cout << "XML File [" << levelPath << "] had some trouble loading and failed" << std::endl;
		std::cout << "Error description: " << result.description() << "\n";
		return false;
	}

	LoadLayer(BACKGROUNDCOLOUR);
	LoadLayer(BACKGROUND);
	LoadLayer(OBJECTS);
	LoadLayer(FOREGROUND);
	LoadLayer(COLLISION);
	LoadLayer(DESTRUCTIBLES);

	return true;
}

void Level::LoadLayer(LevelLayers layer)
{
	pugi::xml_node startNode;

	//nodeName is required because .value() returns a const char*, whereas we want to compare it to an std::string. Putting into nodeName does an implicit cast, which is cool
	std::string nodeName;
	//Scan through the document and find the layer node that is the one we are trying to load
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
		else if(layer == BACKGROUND)
		{
			nodeName = beginNode.attribute("Name").value();
			if(nodeName == "Background")
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
	//The collision data and destructibles needs to be loaded in differently to the normal sprite based texture data.
	else if((layer != COLLISION) && (layer != BACKGROUNDCOLOUR))
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
			if(textureMap.find(texName) != textureMap.end())
			{
				objectSprite.setTexture(*textureMap[texName]);
			}
			else
			{
				std::cout << "Loading Texture : " << relativeTexPath << std::endl;
				tex->loadFromFile("..\\" + relativeTexPath);
				textureMap[texName] = std::move(tex);

				objectSprite.setTexture(*textureMap[texName]);
			}

			objectSprite.setOrigin(objectSprite.getGlobalBounds().left + (objectSprite.getLocalBounds().width/2) , objectSprite.getGlobalBounds().top + (objectSprite.getLocalBounds().height/2));

			//load up the sprite with the extracted date
			objectSprite.setPosition(float(xPos),float(yPos));
			objectSprite.setRotation(rotation);
			objectSprite.setScale(xScale,yScale);

			//And now actually place the sprites into the correct storage for rendering
			if(layer == BACKGROUND)
			{
				backgroundSprites.push_back(objectSprite);
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
				//if we are here, we have just loaded in a non destructible sprite as a normal sprite
				//Create a sprite for the destroyed version of this object, the same as objectSprite for now
				sf::Sprite destroyedObjectSprite = objectSprite;

				//Load the destroyed object image
				std::unique_ptr<sf::Texture> destroyedTex(new sf::Texture());

				//Find the image in the same directory which is names the same as the default image, but has the destructible suffix, indicating that it is the destroyed image
				//This means we need to change textureName.png or whatever to textureName-Destructed.png
				texName = texName + DESTRUCTABLESUFFIX;
				relativeTexPath.insert(relativeTexPath.find('.'),DESTRUCTABLESUFFIX);

				//This if statement checks if the key is already in the map. If the key is already in the map loading another texture will be wasteful, so we just use the entry already stored
				if(textureMap.find(texName) != textureMap.end())
				{
					destroyedObjectSprite.setTexture(*textureMap[texName]);
				}
				else
				{
					std::cout << "Loading Texture : " << relativeTexPath << std::endl;
					if(destroyedTex->loadFromFile("..\\" + relativeTexPath))
					{
						textureMap[texName] = std::move(destroyedTex);
						destroyedObjectSprite.setTexture(*textureMap[texName]);	
					}
					else
					{
						std::cout << "Cannot find destroyed version of texture, should be at : " << relativeTexPath << std::endl;
						std::cout << "FIX THIS IMMEDIATELY, returning out, undefined behaviour could result" << std::endl;
						return;
					}
					
				}
				//Push the sprite pair into the destuctibles vector
				destructibleObjects.push_back(DestructibleObject(objectSprite,destroyedObjectSprite));
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
			sf::Rect<float> collisionRect(xPos,yPos,width,height);
			//And finally push it onto the collision bounds storage vector
			collisionBounds.push_back(collisionRect);
		}
	}
}

std::vector<sf::Rect<float>> Level::GetCollisionBounds()
{
	//Done this way so we can add any non-static bounds to the end of it and deal with it in one go
	std::vector<sf::Rect<float>> returnedBounds;
	returnedBounds = collisionBounds;

	//Add the collision bounds to any destructable that isnt destroyed
	for(size_t i = 0; i < destructibleObjects.size(); i++)
	{
		if(destructibleObjects[i].IsIntact())
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
void Level::Draw(sf::RenderWindow &window)
{
	//Clear the screen to the sky colour
	window.clear(backgroundColor);

	for(size_t i = 0; i < backgroundSprites.size(); i++)
	{
		window.draw(backgroundSprites[i]);
	}
	for(size_t i = 0; i < destructibleObjects.size(); i++)
	{
		destructibleObjects[i].Render(window);
	}
	for(size_t i = 0; i < objectSprites.size(); i++)
	{
		window.draw(objectSprites[i]);
	}
	for(size_t i = 0; i < foregroundSprites.size(); i++)
	{
		window.draw(foregroundSprites[i]);
	}

	//Draw the collision bounds for debugging
	if(LEVEL_DEBUG)
	{
		for(size_t i = 0; i < collisionBounds.size(); i++)
		{
			sf::RectangleShape debugDrawRect;
			debugDrawRect.setSize(sf::Vector2f(collisionBounds[i].width,collisionBounds[i].height));
			debugDrawRect.setPosition(collisionBounds[i].left,collisionBounds[i].top);
			debugDrawRect.setFillColor(sf::Color::Transparent);
			debugDrawRect.setOutlineThickness(1.0f);
			debugDrawRect.setOutlineColor(sf::Color::Red);
			window.draw(debugDrawRect);
			
		}
	}
}

