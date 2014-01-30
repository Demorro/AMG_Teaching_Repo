#pragma once
#include "TweeningText.h"
#include <memory>
#include "Assets.h"
#include "UtilityFunctions.h"
#include "InterStateSingleTon.h"

class EndingSequence
{
public:
	EndingSequence(Camera *stageCam);
	~EndingSequence(void);

	void Update(sf::Event events, bool eventFired, double deltaTime);
	void Render(sf::RenderWindow &renderWindow);

	bool IsActive();
	void SetIsActive(bool isActive);

	void ResetEndingSequence(float gameTimeInMilliseconds);

private:
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
	
	sf::SoundBuffer gradeStampSoundBuffer;
	sf::SoundBuffer aPlusStampSoundBuffer;
	sf::Sound gradeStampSound;

	LevelGrade currentDisplayedGrade; //you count up from C to A+, stopping at what grade you got, so this is neccesary to say what grade we're on.
	sf::Clock gradeStampClock;
	float timeBetweenGradeStamps;
	//counts up the grades from c to whatever you got
	void DoGradeStampCountUp(sf::Sprite &gradeSprite, LevelGrade currentGrade, LevelGrade finalGrade);
	void StampToNewGrade(sf::Sprite &gradeSprite, LevelGrade newGrade);

	sf::Text tempThankYouText;
	//A reference to the stage camera.
	Camera* gameCam;
};

