#include "EndingSequence.h"
#include "Application.h"

EndingSequence::EndingSequence(Camera *stageCam, bool shouldDoGradeCountUp)
{
	this->shouldDoGradeCountUp = shouldDoGradeCountUp;
	this->isAScoringSequence = shouldDoGradeCountUp;
	this->isCountingToSceneTransition = !shouldDoGradeCountUp;
	this->gameCam = stageCam;

	secondsTillSceneTransition = 4.0f;
	sceneTransitionTimer.restart();
	sequenceIsDone = false;

	endFont.loadFromFile(DEFAULTFONT);

	//Temporary thank you for playing our demo
	tempThankYouText.setFont(endFont);
	tempThankYouText.setCharacterSize(30);
	tempThankYouText.setString("Thank you for playing this demo. We're excited to know what you think.");
	tempThankYouText.setOrigin(tempThankYouText.getLocalBounds().width/2, tempThankYouText.getLocalBounds().height/2);
	tempThankYouText.setPosition(Application::GetWindow().getSize().x/2,Application::GetWindow().getSize().y - 60);

	textSize = 60;

	float endTweenBobAmount = 150.0f;
	float endTweenSpeed = 1.2f;

	float endTextXOffsetFromCenter = -150;

	std::string completedTextString = "Level Completed!";
	float completedTextYFromCenter = -270;
	completedText = std::unique_ptr<TweeningText>(new TweeningText(endFont,textSize + 20,sf::Vector2f(Application::GetWindow().getSize().x/2,Application::GetWindow().getSize().y/2 + completedTextYFromCenter),completedTextString,true,true,stageCam));
	completedText->GenerateTween(completedText->GetPosition(), completedText->GetLocalBounds(), TweenableElement::TweenInDirection::Left, endTweenBobAmount, endTweenSpeed);

	timeTakenTextString = "Completion Time     :     ";
	float timeTakingYFromCenter = -60;
	timeTakenText = std::unique_ptr<TweeningText>(new TweeningText(endFont,textSize,sf::Vector2f(Application::GetWindow().getSize().x/2 + endTextXOffsetFromCenter,Application::GetWindow().getSize().y/2 + timeTakingYFromCenter),timeTakenTextString,true,true,stageCam));
	timeTakenText->GenerateTween(timeTakenText->GetPosition(), timeTakenText->GetLocalBounds(), TweenableElement::TweenInDirection::Left, endTweenBobAmount, endTweenSpeed);
	
	gradeTextString = "Grade				        :  ";//lol
	float gradeTextYFromCenter = 100;
	gradeText = std::unique_ptr<TweeningText>(new TweeningText(endFont,textSize,sf::Vector2f(Application::GetWindow().getSize().x/2 + endTextXOffsetFromCenter,Application::GetWindow().getSize().y/2 + gradeTextYFromCenter),gradeTextString,true,true,stageCam));
	gradeText->GenerateTween(gradeText->GetPosition(), gradeText->GetLocalBounds(), TweenableElement::TweenInDirection::Left, endTweenBobAmount, endTweenSpeed);

	ResetEndingSequence(0.0f);
	timeCountUpDelay = 0.5f; //if this was 0 the time counter on the end screen would start scrollig up as soon as the tween starts
	//sound for counting up time
	float timeClackerVolume = 100.0f;
	countUpTimeClackerBuffer.loadFromFile(COUNTUPTIMESOUND);
	countUpTimeSound.setBuffer(countUpTimeClackerBuffer);
	countUpTimeSound.setLoop(true);
	countUpTimeSound.setVolume(timeClackerVolume);


	//grade images
	emptyGradeTexture.loadFromFile(EMPTYGRADE);
	cGradeTexture.loadFromFile(CGRADE);
	bGradeTexture.loadFromFile(BGRADE);
	aGradeTexture.loadFromFile(AGRADE);
	aPlusGradeTexture.loadFromFile(APLUSGRADE);

	sf::Vector2f gradeSpritePosition(Application::GetWindow().getSize().x/2 + 300, Application::GetWindow().getSize().y/2 + gradeTextYFromCenter + 20);
	gradeSprite.setTexture(emptyGradeTexture);
	gradeSprite.setOrigin(gradeSprite.getGlobalBounds().left + gradeSprite.getGlobalBounds().width/2, gradeSprite.getGlobalBounds().top + gradeSprite.getGlobalBounds().height/2);
	gradeSprite.setPosition(gradeSpritePosition);

	timeBetweenGradeStamps = 1.0f;

	//set default grade times, these should be overwritten by loaded in config files from the level state using SetGradeTimes()
	aPlusGradeMaxTimeInSeconds = 30;
	aGradeMaxTimeInSeconds = 50;
	bGradeMaxTimeInSeconds = 70;

	//grade stamp sound
	float stampVolume = 100.0f;
	gradeStampSoundBuffer.loadFromFile(GRADESTAMPSOUND);
	aPlusStampSoundBuffer.loadFromFile(APLUSSOUND);
	gradeStampSound.setBuffer(gradeStampSoundBuffer);
	gradeStampSound.setVolume(stampVolume);
	SetIsActive(false);
}


EndingSequence::~EndingSequence(void)
{
}

void EndingSequence::Update(sf::Event events, bool eventFired, double deltaTime, std::function<void()> switchToNextState)
{
	if(isEndingSequenceActive)
	{
		if(shouldDoGradeCountUp)
		{
			completedText->Update(events,eventFired,deltaTime);
			timeTakenText->Update(events,eventFired,deltaTime);
			gradeText->Update(events,eventFired,deltaTime);

			if(timeCountUpDelayClock.getElapsedTime().asSeconds() > timeCountUpDelay)
			{
				//if we havnt finished counting up
				if(currentCountUpDisplay < playerFinishedTime)
				{
					//play the clackedy sound if we're counting up
					if(countUpTimeSound.getStatus() != sf::Sound::Status::Playing)
					{
						if(interStateSingleton.GetIsVolumeOn())
						{
							countUpTimeSound.play();
						}
					}

					//count up the end-screen timer display
					timeTakenText->SetText(timeTakenTextString + CountUpTimeDisplay(currentCountUpDisplay, deltaTime, 32000.0f, playerFinishedTime));
				}
			}
			
			//These two statements control the grade countup, as well as the delay between finish and state switch
			//if the time counter has finished, we can do the scoring
			if(currentCountUpDisplay >= playerFinishedTime)
			{
				isCountingToSceneTransition = DoGradeStampCountUp(gradeSprite,currentDisplayedGrade,WhatGradeFromTime(playerFinishedTime/1000.0f)); //playerfinishedtime is in milliseconds so we have to divide it
				countUpTimeSound.stop();
				if(isCountingToSceneTransition)
				{
					sceneTransitionTimer.restart();
					shouldDoGradeCountUp = false;
				}
			}
		}
		if(isCountingToSceneTransition)
		{
			if(sceneTransitionTimer.getElapsedTime().asSeconds() > secondsTillSceneTransition)
			{
				switchToNextState();
			}
		}
	}

	

}
void EndingSequence::Render(sf::RenderWindow &renderWindow)
{
		if(isEndingSequenceActive)
		{
			if(isAScoringSequence)
			{
				completedText->Render(renderWindow);
				timeTakenText->Render(renderWindow);
				gradeText->Render(renderWindow);
		
				//tempThankYouText.move(gameCam->GetScreenSpaceOffsetVector());
				//renderWindow.draw(tempThankYouText);	
				//tempThankYouText.move(-gameCam->GetScreenSpaceOffsetVector());

				gradeSprite.move(gameCam->GetScreenSpaceOffsetVector());
				renderWindow.draw(gradeSprite);
				gradeSprite.move(-gameCam->GetScreenSpaceOffsetVector());
			}
		}
}

bool EndingSequence::IsActive()
{
	return isEndingSequenceActive;
}
void EndingSequence::SetIsActive(bool isActive)
{
	this->isEndingSequenceActive = isActive;
}

void EndingSequence::ResetEndingSequence(float gameTimeInMilliseconds)
{
	completedText->ResetTween();
	timeTakenText->ResetTween();
	gradeText->ResetTween();

	timeCountUpDelayClock.restart();
	currentCountUpDisplay = 0;
	playerFinishedTime = gameTimeInMilliseconds;

	currentDisplayedGrade = LevelGrade::None;
	sceneTransitionTimer.restart();

	gradeStampClock.restart();
}

std::string EndingSequence::CountUpTimeDisplay(float &currentTimeDisplayInMilliseconds, double deltaTime, float countUpSpeed, double valueToStopAtInMilliseconds)
{
	currentTimeDisplayInMilliseconds += (countUpSpeed * deltaTime);

	if(currentTimeDisplayInMilliseconds > valueToStopAtInMilliseconds)
	{
		return GetTimerTextFromTime(valueToStopAtInMilliseconds,false);
	}
	else
	{
		return GetTimerTextFromTime(currentTimeDisplayInMilliseconds,false);
	}
}

//just used in the below function to stamp up grades
void EndingSequence::StampToNewGrade(sf::Sprite &gradeSprite, LevelGrade newGrade)
{
	switch (newGrade)
	{
	case LevelGrade::None :
		currentDisplayedGrade = LevelGrade::None;
		gradeSprite.setTexture(emptyGradeTexture);
		break;
	case LevelGrade::C :
		currentDisplayedGrade = LevelGrade::C;
		gradeSprite.setTexture(cGradeTexture);
		break;
	case LevelGrade::B :
		currentDisplayedGrade = LevelGrade::B;
		gradeSprite.setTexture(bGradeTexture);
		break;
	case LevelGrade::A :
		currentDisplayedGrade = LevelGrade::A;
		gradeSprite.setTexture(aGradeTexture);
		break;
	case LevelGrade::APlus :
		currentDisplayedGrade = LevelGrade::APlus;
		gradeSprite.setTexture(aPlusGradeTexture);
		break;
	default : 
		break;
	}

	//play the stamp sound
	if(interStateSingleton.GetIsVolumeOn())
	{
		if(newGrade != LevelGrade::None)
		{
			if(newGrade == LevelGrade::APlus)
			{
				gradeStampSound.setBuffer(aPlusStampSoundBuffer);
				gradeStampSound.play();
			}
			else
			{
				gradeStampSound.setBuffer(gradeStampSoundBuffer);
				gradeStampSound.play();
			}
		}
	}
}

bool EndingSequence::DoGradeStampCountUp(sf::Sprite &gradeSprite, LevelGrade currentGrade, LevelGrade finalGrade)
{
	//Dont do anymore stamping if we're there.
	if(currentGrade == finalGrade)
	{
		return true;
	}

	switch (currentGrade)
	{
	case LevelGrade::None :
		if(gradeStampClock.getElapsedTime().asSeconds() > timeBetweenGradeStamps)
		{
			StampToNewGrade(gradeSprite, LevelGrade::C);
			gradeStampClock.restart();
		}
		break;
	case LevelGrade::C :
		if(gradeStampClock.getElapsedTime().asSeconds() > timeBetweenGradeStamps)
		{
			StampToNewGrade(gradeSprite, LevelGrade::B);
			gradeStampClock.restart();
		}
		break;
	case LevelGrade::B :
		if(gradeStampClock.getElapsedTime().asSeconds() > timeBetweenGradeStamps)
		{
			StampToNewGrade(gradeSprite, LevelGrade::A);
			gradeStampClock.restart();
		} 
		break;
	case LevelGrade::A :
		if(gradeStampClock.getElapsedTime().asSeconds() > timeBetweenGradeStamps)
		{
			StampToNewGrade(gradeSprite, LevelGrade::APlus);
			gradeStampClock.restart();
		}
		break;
	case LevelGrade::APlus :
		 
		break;
	default : 
		break;
	}

	return false;
}

void EndingSequence::SetGradeTimes(float aPlusGradeMaxTimeInSeconds, float aGradeMaxTimeInSeconds, float bGradeMaxTimeInSeconds)
{
	this->aPlusGradeMaxTimeInSeconds = aPlusGradeMaxTimeInSeconds;
	this->aGradeMaxTimeInSeconds = aGradeMaxTimeInSeconds;
	this->bGradeMaxTimeInSeconds = bGradeMaxTimeInSeconds;
}

EndingSequence::LevelGrade EndingSequence::WhatGradeFromTime(float timeInSeconds)
{
	if(timeInSeconds < aPlusGradeMaxTimeInSeconds)
	{
		return LevelGrade::APlus;
	}
	else if(timeInSeconds < aGradeMaxTimeInSeconds)
	{
		return LevelGrade::A;
	}
	else if(timeInSeconds < bGradeMaxTimeInSeconds)
	{
		return LevelGrade::B;
	}
	else
	{
		return LevelGrade::C;
	}
}

bool EndingSequence::IsSequenceDone()
{
	return sequenceIsDone;
}