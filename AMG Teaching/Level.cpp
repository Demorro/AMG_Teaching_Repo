#include "Level.h"


Level::Level()
{

}

Level::Level(std::string levelDataPath)
{
	LoadLevel(levelDataPath);
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

	LoadLayer(BACKGROUND);
	LoadLayer(OBJECTS);
	LoadLayer(FOREGROUND);
	LoadLayer(COLLISION);

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
		if(layer == BACKGROUND)
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
		else
		{
			std::cout << "Error : Level::LoadLayer()" << std::endl;
			return;
		}
	}

	//The collision data needs to be loaded in differently to the sprite based texture data.
	if(layer != COLLISION)
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
			sf::Rect<int> collisionRect(xPos,yPos,width,height);
			//And finally push it onto the collision bounds storage vector
			collisionBounds.push_back(collisionRect);
		}
	}
}

std::vector<sf::Rect<int>> *Level::GetCollisionBounds()
{
	return &collisionBounds;
}

void Level::Draw(sf::RenderWindow &window)
{
	for(size_t i = 0; i < backgroundSprites.size(); i++)
	{
		window.draw(backgroundSprites[i]);
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

