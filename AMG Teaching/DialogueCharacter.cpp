#include "DialogueCharacter.h"


DialogueCharacter::DialogueCharacter(sf::Vector2f position, sf::Texture &characterTexture, std::vector<std::pair<std::string, std::string>> speechTextAndAudioPaths, sf::Vector2f speechBubblePosition,  sf::Texture &speechBubbleTexture, bool shouldPhrasesLoop, float speechVolume, bool shouldShowBubble, bool shouldWobble)
{
	//Whether or not the speech bubble is dispalyed.
	shouldDisplaySpeechBubble = shouldShowBubble;
	shouldDoWobble = shouldWobble;

	//whether or not the dialogue phrases loop back to the beginning or whether they just stay at the end when they get there
	shouldLoopPhrases = shouldPhrasesLoop;

	//Whether the character should wobble back and forth
	shouldWobble = true;

	characterTexture.setSmooth(true);
	speechBubbleSprite.setTexture(speechBubbleTexture);

	character.setTexture(characterTexture);
	character.setOrigin(character.getGlobalBounds().left + character.getGlobalBounds().width/2,character.getGlobalBounds().top + character.getGlobalBounds().height);
	//the position needs to be modified as GLEEED loads in origins from the center of the sprite, where for the characters we want the origin to be at the base of their feet
	position.y += character.getGlobalBounds().height/2;
	character.setPosition(position);
	//load up the stuff relating to speech
	for(int i = 0; i < speechTextAndAudioPaths.size(); i++)
	{
		phrases.push_back(std::pair<sf::SoundBuffer,std::string>());
		if(phrases.back().first.loadFromFile(speechTextAndAudioPaths[i].second) == false)
		{
			std::cout << "Error. Audio file for character speech : " << speechTextAndAudioPaths[i].second << " cannot be found, it should be in the same directory as the character" << std::endl;
		}
		phrases.back().second = speechTextAndAudioPaths[i].first;
	}

	xOffsetFromTopLeftSpeechBubble = speechBubblePosition.x;
	yOffsetFromTopLeftSpeechBubble = speechBubblePosition.y;

	isSpeechHappening = false;

	expressionWobbleSpeed = 8.0f;
	expressionWobbleIntensity = 7.5f;

	currentSpeechDataIndex = 0;
	textColor = sf::Color::Black;

	voice.setVolume(speechVolume);
}


DialogueCharacter::~DialogueCharacter(void)
{
}

void DialogueCharacter::Load()
{
	//the constructors being a prick, so load some shit here. (I really need to read up on constructors, they seem to always misbehave for me)
	//set the bubble origin the to bottom left of it
	speechBubbleSprite.setOrigin(speechBubbleSprite.getGlobalBounds().left, speechBubbleSprite.getGlobalBounds().top + speechBubbleSprite.getGlobalBounds().height);
	//the passed in coordinates from XML are from top left, so do this dumb by easy thing to get it there.
	speechBubbleSprite.setPosition(character.getGlobalBounds().left, character.getGlobalBounds().top);
	speechBubbleSprite.move(xOffsetFromTopLeftSpeechBubble,yOffsetFromTopLeftSpeechBubble);
	//set the bubble sprite alpha to 0.
	sf::Color transColor = sf::Color::White;
	transColor.a = 0;
	speechBubbleSprite.setColor(transColor);

	isBubbleOn = false;
	playerWasCollidingLastFrame = false;
	fadeSpeed = 1400.0f;

	//the text
	speechFont.loadFromFile(DEFAULTFONT);
}

void DialogueCharacter::Update(double deltaTime, Player &player)
{
	if(player.GetCollider().intersects(character.getGlobalBounds()))
	{
		if(playerWasCollidingLastFrame == false)
		{
			DoSpeech();
		}
		playerWasCollidingLastFrame = true;
	}
	else
	{
		playerWasCollidingLastFrame = false;
	}

	//fading in and out of speech bubble
	if(IsDoingSpeech())
	{
		//fade in
		if(speechBubbleSprite.getColor().a < 255)
		{
			sf::Color currentColor = speechBubbleSprite.getColor();

			if( ((int)currentColor.a + (fadeSpeed * deltaTime)) <=255) //check the uint8 wont overflow
			{
     			currentColor.a += (fadeSpeed * deltaTime);
				speechBubbleSprite.setColor(currentColor);
				//set the current text line vector the correct colors too
				for(int i = 0; i < speechTextLines.size(); i++)
				{
					sf::Color newTextColor = textColor;
					newTextColor.a = currentColor.a;
					speechTextLines[i].setColor(newTextColor);
				}

			}
			else
			{
				currentColor.a = 255;
				speechBubbleSprite.setColor(currentColor);
				//set the current text line vector the correct colors too
				for(int i = 0; i < speechTextLines.size(); i++)
				{
					sf::Color newTextColor = textColor;
					newTextColor.a = currentColor.a;
					speechTextLines[i].setColor(newTextColor);
				}
			}

		}
	}
	else
	{
		//fade out
		if(speechBubbleSprite.getColor().a > 0)
		{
			sf::Color currentColor = speechBubbleSprite.getColor();

			if( ((int)currentColor.a - (fadeSpeed * deltaTime)) > 0) //check the uint8 wont overflow
			{
     			currentColor.a -= (fadeSpeed * deltaTime);
				speechBubbleSprite.setColor(currentColor);
				//set the current text line vector the correct colors too
				for(int i = 0; i < speechTextLines.size(); i++)
				{
					sf::Color newTextColor = textColor;
					newTextColor.a = currentColor.a;
					speechTextLines[i].setColor(newTextColor);
				}
			}
			else
			{
				currentColor.a = 0;
				speechBubbleSprite.setColor(currentColor);
				//set the current text line vector the correct colors too
				for(int i = 0; i < speechTextLines.size(); i++)
				{
					sf::Color newTextColor = textColor;
					newTextColor.a = currentColor.a;
					speechTextLines[i].setColor(newTextColor);
				}
			}

		}
	}

	//detect when the speech stops and reset
	if((IsDoingSpeech() == true) && (character.IsWobbling() == false))
	{
		isSpeechHappening = false;
	}

	character.Update(deltaTime);
}

void DialogueCharacter::Render(sf::RenderWindow &window)
{
	window.draw(character);
	if(shouldDisplaySpeechBubble == true)
	{
		window.draw(speechBubbleSprite);
		for(int i = 0; i < speechTextLines.size(); i++)
		{
			window.draw(speechTextLines[i]);
		}
	}
}

void DialogueCharacter::DoSpeech()
{
	if(IsDoingSpeech() == false)
	{
		isSpeechHappening = true;
		//The length of the wobble and stuff is defined by the length of the audio file
		float phraseLength = 3.0f;

		if(currentSpeechDataIndex < phrases.size())
		{
			std::string textToDisplay = phrases[currentSpeechDataIndex].second;
			voice.setBuffer(phrases[currentSpeechDataIndex].first);
			phraseLength = phrases[currentSpeechDataIndex].first.getDuration().asSeconds();
			MakeTextBeCenterOfBubble(textToDisplay,speechBubbleSprite);
		}
		else
		{
			voice.setBuffer(sf::SoundBuffer()); //if we dont do this, in the case of an error or no sound the last sound will play and wont match the text. DEATH BEFORE MINOR INCONGRUITIES
			std::cout << "Error in background character, phrase index out of bound" << std::endl;
		}

		voice.play();
		if(shouldDoWobble)
		{
			character.DoExpressionWobble(phraseLength,expressionWobbleSpeed,expressionWobbleIntensity);
		}

		//this increment means the character will go down the phrase list, and then stop on the final one, saying that forever!
		if(currentSpeechDataIndex < phrases.size() - 1)
		{
			currentSpeechDataIndex++;
		}
		else
		{
			if(shouldLoopPhrases)
			{
				currentSpeechDataIndex = 0;
			}
		}
	}
}

bool DialogueCharacter::IsDoingSpeech()
{
	return isSpeechHappening;
}

void DialogueCharacter::MakeTextBeCenterOfBubble(std::string text, sf::Sprite &bubbleSprite)
{
	speechTextLines.clear();

	int speechSize = 25;

	//split the input text into lines
	std::vector<std::string> textLines = SplitStringIntoLines(text);

	for(int i = 0; i < textLines.size(); i++)
	{
		//erase any tabs from the start of the string
		textLines[i].erase(std::remove(textLines[i].begin(), textLines[i].end(), '\t'), textLines[i].end());
		speechTextLines.push_back(sf::Text(sf::String(textLines[i]),speechFont,speechSize));
		speechTextLines.back().setColor(textColor);
		//set the position of this line to be centered in the bubble, will be offset later depending on what line it is
		speechTextLines.back().setPosition((bubbleSprite.getGlobalBounds().left + bubbleSprite.getGlobalBounds().width/2) -speechTextLines.back().getGlobalBounds().width/2, (bubbleSprite.getGlobalBounds().top + bubbleSprite.getGlobalBounds().height/2) - speechTextLines.back().getGlobalBounds().height/2);
	}

	int centerIndex = (speechTextLines.size()/2); //Thanks to the magic of ints this should work, the index this points to gets to stay in the center

	//if the number of lines is even, it needs to be translated half a line down to make it center right
	bool needHalfTranslate = false;
	if(speechTextLines.size()%2 == 0)
	{
		needHalfTranslate = true;
	}

	int lineYOffset = speechTextLines.front().getCharacterSize();
	for(int i = 0; i < speechTextLines.size(); i++)
	{
		//if the line index was 0, and there were 4 lines in total. The center would be 2, so this will come out at -2, meaning we have to translate this -2 lines up, which is correct
		int amountOfLinesOff = i - centerIndex;
		if(needHalfTranslate)
		{
			speechTextLines[i].move(0, lineYOffset/2);
		}
		speechTextLines[i].move(0, amountOfLinesOff * lineYOffset);
	}


}