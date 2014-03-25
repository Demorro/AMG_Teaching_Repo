#include "ScrollingCredits.h"


ScrollingCredits::ScrollingCredits(void)
{
	creditsFont.loadFromFile(DEFAULTFONT);
	textSize = 30;
	scrollSpeed = 10;
	lineGap = 30;
	timeToStayInCredits = 30;

	timeToStayInCreditsClock.restart();

	startPos.x = 0;
	startPos.y = 0;

	creditsTimeHasPassed = false;

	LoadCreditsText(CREDITSDATA);
}


ScrollingCredits::~ScrollingCredits(void)
{
}

void ScrollingCredits::LoadCreditsText(std::string creditsConfigPath)
{
	//load the xml doc
	pugi::xml_document creditsDoc;
	LoadXMLDoc(creditsDoc,creditsConfigPath);

	std::string creditsTextNodeName = "CreditsText";
	std::string textSizeNodeName = "TextSize";
	std::string scrollSpeedNodeName = "ScrollSpeed";
	std::string lineGapNodeName = "LineGap";
	std::string startPosXNodeName = "StartPosX";
	std::string startPosYNodeName = "StartPosY";
	std::string menuTimeNodeName = "TimeToGoBackToMenuInSeconds";

	for(pugi::xml_node node = creditsDoc.child("Credits").first_child(); node; node = node.next_sibling())
	{
		std::string nodeName = node.name();

		if(nodeName == creditsTextNodeName)
		{
			LoadTextValue(creditString,creditsDoc.child("Credits"),creditsTextNodeName);
		}
		else if(nodeName == textSizeNodeName)
		{
			LoadNumericalValue(textSize,creditsDoc.child("Credits"),textSizeNodeName);
		}
		else if(nodeName == scrollSpeedNodeName)
		{
			LoadNumericalValue(scrollSpeed,creditsDoc.child("Credits"),scrollSpeedNodeName);
		}
		else if(nodeName == lineGapNodeName)
		{
			LoadNumericalValue(lineGap,creditsDoc.child("Credits"),lineGapNodeName);
		}
		else if(nodeName == startPosXNodeName)
		{
			LoadNumericalValue(startPos.x,creditsDoc.child("Credits"),startPosXNodeName);
		}
		else if(nodeName == startPosYNodeName)
		{
			LoadNumericalValue(startPos.y,creditsDoc.child("Credits"),startPosYNodeName);
		}
		else if(nodeName == menuTimeNodeName)
		{
			LoadNumericalValue(timeToStayInCredits,creditsDoc.child("Credits"),menuTimeNodeName);
		}
	}

	LoadStringIntoRenderableStrings(creditString);
	AllignRenderableCreditStrings();
}

void ScrollingCredits::LoadStringIntoRenderableStrings(std::string stringToLoad)
{
	std::vector<std::string> splitStrings;
	splitStrings = SplitStringIntoLines(stringToLoad);

	for(std::string str : splitStrings)
	{
		renderableStrings.push_back(sf::Text(sf::String(str),creditsFont,textSize));
	}
}

void ScrollingCredits::AllignRenderableCreditStrings()
{
	for(int i = 0; i < renderableStrings.size(); i++)
	{
		renderableStrings[i].setPosition(startPos.x, startPos.y + (lineGap * i));
	}
}

void ScrollingCredits::Update(float deltaTime)
{
	for(int i = 0; i < renderableStrings.size(); i++)
	{
		renderableStrings[i].move(0, scrollSpeed * deltaTime);
	}

	if(timeToStayInCreditsClock.getElapsedTime().asSeconds() > timeToStayInCredits)
	{
		creditsTimeHasPassed = true;
	}
}

void ScrollingCredits::Render(sf::RenderWindow &window)
{
	for(sf::Text str : renderableStrings)
	{
		window.draw(str);
	}
}

bool ScrollingCredits::ShouldSwitchOutOfCreditsState()
{
	if(creditsTimeHasPassed)
	{
		return true;
	}
	else
	{
		return false;
	}
}