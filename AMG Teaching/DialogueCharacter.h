#pragma once
#include "NPC.h"
#include "SFML\Audio.hpp"
#include "Assets.h"
#include <iostream>
#include "Player.h"
#include "XMLParseUtilities.h"

class DialogueCharacter
{
public:
	//speechtextandaudiopaths, first is the text, second is the audio path.
	DialogueCharacter(sf::Vector2f position, sf::Texture &characterTexture, std::vector<std::pair<std::string, std::string>> speechTextAndAudioPaths, sf::Vector2f speechBubblePosition, sf::Texture &speechBubbleTexture, bool shouldPhrasesLoop, float speechVolume, bool shouldShowBubble, bool shouldWobble);
	~DialogueCharacter(void);

	void Update(double deltaTime, Player &player);
	void Render(sf::RenderWindow &window);
	void Load();

	bool IsDoingSpeech();
	void DoSpeech();

private:
	//The speech goes through the vectors loaded in from XML, doing the first entry, then the second etc, and it loops in the last, meaning the last entry is played over and over. This index keeps track of what vector entry we're on.
	int currentSpeechDataIndex;

	float fadeSpeed;

	//sf::Texture speechBubbleTexture;
	sf::Sprite speechBubbleSprite;
	float xOffsetFromTopLeftSpeechBubble;
	float yOffsetFromTopLeftSpeechBubble;
	bool isBubbleOn;

	float expressionWobbleSpeed;
	float expressionWobbleIntensity;

	bool isSpeechHappening;

	//This is because we dont want the speech to constantly trigger if the player is just standing on the background character.
	bool playerWasCollidingLastFrame;
	NPC character;

	sf::Font speechFont;
	sf::Color textColor;
	//this is a vector cause it's easier to split the text up into lines like this
	std::vector<sf::Text> speechTextLines;

	void MakeTextBeCenterOfBubble(std::string text, sf::Sprite &bubbleSprite);

	std::vector<std::pair<sf::SoundBuffer, std::string>> phrases;
	sf::Sound voice;

	bool shouldLoopPhrases;
	bool shouldDisplaySpeechBubble;
	bool shouldDoWobble;
};

