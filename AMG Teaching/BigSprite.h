#pragma once
#include <SFML\Graphics.hpp>
#include <list>

//BigSprite by Laurent Gomilla, taken from http://en.sfml-dev.org/forums/index.php?topic=6023.0, editied to suit my needs
//Neccesary because sf::Textures that are really big can't be stored on old graphics cards.
class BigSprite : public sf::Transformable, public sf::Drawable
{
public:

    BigSprite(const sf::Image& source)
    {
		noOfTexturesX = 0;
		noOfTexturesY = 0;

		unsigned int size = sf::Texture::getMaximumSize();
		for (unsigned int i = 0; i < source.getSize().x; i += size)
		{
			noOfTexturesX++;
			for (unsigned int j = 0; j < source.getSize().y; j += size)
            {

				noOfTexturesY++;
                sf::Texture texture;
				texture.loadFromImage(source, sf::IntRect(i, j, size, size));
                myTextures.push_back(texture);

                sf::Sprite sprite(myTextures.back());
				sprite.setPosition(i, j);
                mySprites.push_back(sprite);
            }
		}
	}

	void draw(sf::RenderTarget &target, sf::RenderStates states = sf::RenderStates::Default) const
    {
        for (std::list<sf::Sprite>::const_iterator it = mySprites.begin(); it != mySprites.end(); ++it)
        {
            // combine self's transform with the current sprite's transform (API not defined yet)
			target.draw(*it);
        }
    }

	void move(const sf::Vector2f &offset)
	{
		for(sf::Sprite &image : mySprites)
		{
			image.move(offset);
		}
	}

	void move(float offsetX, float offsetY)
	{
		for(sf::Sprite &image : mySprites)
		{
			image.move(offsetX, offsetY);
		}
	}

	//Expensive and dumb
	sf::Vector2f GetSize()
	{
		float lowestXBound = 0;
		float highestXBound = 0;
		float lowestYBound = 0;
		float highestYBound = 0;

		lowestXBound = mySprites.front().getGlobalBounds().left;
		lowestYBound = mySprites.front().getGlobalBounds().top;

		for(sf::Sprite &image : mySprites)
		{
			if(image.getGlobalBounds().left < lowestXBound)
			{
				lowestXBound = image.getGlobalBounds().left;
			}

			if(image.getGlobalBounds().top < lowestYBound)
			{
				lowestYBound = image.getGlobalBounds().top;
			}


			if(image.getGlobalBounds().left + image.getGlobalBounds().width > highestXBound)
			{
				highestXBound = image.getGlobalBounds().left + image.getGlobalBounds().width;
			}

			if(image.getGlobalBounds().top + image.getGlobalBounds().height > highestYBound)
			{
				highestYBound = image.getGlobalBounds().top + image.getGlobalBounds().height;
			}
		}

		float xWidth = highestXBound - lowestXBound;
		float yWidth = highestYBound - lowestYBound;

		return sf::Vector2f(xWidth,yWidth);
	}

private:

	int noOfTexturesX;
	int noOfTexturesY;

    std::list<sf::Sprite> mySprites;
    std::list<sf::Texture> myTextures;
};