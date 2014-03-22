#pragma once
#include "TweeningText.h"
#include <memory>
#include "Assets.h"
#include "UtilityFunctions.h"
#include "InterStateSingleTon.h"
#include <functional>


class EndingSequence
{
public:
	EndingSequence(Camera *stageCam, bool shouldDoGradeCountUp);
	~EndingSequence(void);

	void Update(sf::Event events, bool eventFired, double deltaTime, std::function<void()> switchToNextState = nullptr);
	void Render(sf::RenderWindow &renderWindow);

	bool IsActive();
	void SetIsActive(bool isActive);

	void ResetEndingSequence(float gameTimeInMilliseconds);

	void SetGradeTimes(float aPlusGradeMaxTimeInSeconds, float aGradeMaxTimeInSeconds, float bGradeMaxTimeInSeconds);
	bool IsSequenceDone();
private:
	//Stores whether this sequence should actually do anything, or whether it just tracks time from ending to next scene.
	bool isAScoringSequence;

	//If false, we dont do the grade overlay, and just wait for the delay before switching to the next state
	bool shouldDoGradeCountUp;
	//The time from the end of the sequence till scene transition
	float secondsTillSceneTransition;
	
	//For the part where the ending sequence is done, but we still want the scene to be on for a bit, so we can read and stuff.
	sf::Clock sceneTransitionTimer;
	bool isCountingToSceneTransition;

	bool sequenceIsDone;


	enum LevelGrade
	{
		None,
		C,
		B,
		A,
		APlus
	};

	sf::Font endFont;
	float textSize;

	bool isEndingSequenceActive;

	std::unique_ptr<TweeningText> completedText;
	std::unique_ptr<TweeningText> timeTakenText;
	std::unique_ptr<TweeningText> gradeText;

	std::string timeTakenTextString;
	std::string gradeTextString;
	
	//The time display sort of "scrolls" up, so this is the delay from the start of that onwards
	sf::Clock timeCountUpDelayClock;
	float timeCountUpDelay;
	float currentCountUpDisplay;
	float playerFinishedTime;

	std::string CountUpTimeDisplay(float &currentTimeDisplayInMilliseconds, double deltaTime, float countUpSpeed, double valueToStopAtInMilliseconds);

	//the sound played while the time is counting up
	sf::SoundBuffer countUpTimeClackerBuffer;
	sf::Sound countUpTimeSound;

	//grades
	sf::Texture emptyGradeTexture; //A correctly size empty transparent texture, so we start with no grade then it stamps in.
	sf::Texture cGradeTexture;
	sf::Texture bGradeTexture;
	sf::Texture aGradeTexture;
	sf::Texture aPlusGradeTexture;
	sf::Sprite gradeSprite;

	//grade max times in Seconds
	float aPlusGradeMaxTimeInSeconds;
	float aGradeMaxTimeInSeconds;
	float bGradeMaxTimeInSeconds;

	//returns what grade you should get for said time
	LevelGrade EndingSequence::WhatGradeFromTime(float timeInSeconds);
	
	sf::SoundBuffer gradeStampSoundBuffer;
	sf::SoundBuffer aPlusStampSoundBuffer;
	sf::Sound gradeStampSound;

	LevelGrade currentDisplayedGrade; //you count up from C to A+, stopping at what grade you got, so this is neccesary to say what grade we're on.
	sf::Clock gradeStampClock;
	float timeBetweenGradeStamps;
	//counts up the grades from c to whatever you got
	bool DoGradeStampCountUp(sf::Sprite &gradeSprite, LevelGrade currentGrade, LevelGrade finalGrade);
	void StampToNewGrade(sf::Sprite &gradeSprite, LevelGrade newGrade);

	sf::Text tempThankYouText;
	//A reference to the stage camera.
	Camera* gameCam;
};

