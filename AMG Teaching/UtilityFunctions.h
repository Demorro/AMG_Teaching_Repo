#pragma once
#include <SFML/Graphics.hpp>
#include <sstream>

//take in a time object, and return a formatted H:M:S string
inline std::string GetTimerTextFromTime(sf::Time time, bool haveTimeOnFrontOfString)
{
	int milliSeconds = time.asMilliseconds();
	int seconds;
	int minutes;
	int hours;

	hours = milliSeconds / (1000*60*60);
	minutes = (milliSeconds % (1000*60*60)) / (1000*60);
	seconds = ((milliSeconds % (1000*60*60)) % (1000*60)) / 1000;


	std::string returnableTimeString;
	if(haveTimeOnFrontOfString)
	{
		returnableTimeString = "Time : ";
	}
	else
	{
		returnableTimeString = "";
	}

	//because we dont want the clock to look like "3:30:3" but rather 3:30:03, add an 0 to the front of the segment if the string size is just 1
	std::string timeString;

	std::stringstream timeStream1(std::stringstream::in | std::stringstream::out);
	timeStream1 << hours;
	timeString = timeStream1.str();
	if(timeString.size() <= 1)
	{
		timeString = "0" + timeString;
	}
	returnableTimeString += timeString;
	returnableTimeString += " : ";

	std::stringstream timeStream2(std::stringstream::in | std::stringstream::out);
	timeStream2 << minutes;
	timeString = timeStream2.str();
	if(timeString.size() <= 1)
	{
		timeString = "0" + timeString;
	}
	returnableTimeString += timeString;
	returnableTimeString += " : ";

	std::stringstream timeStream3(std::stringstream::in | std::stringstream::out);
	timeStream3 << seconds;
	timeString = timeStream3.str();
	if(timeString.size() <= 1)
	{
		timeString = "0" + timeString;
	}
	returnableTimeString += timeString;

	return returnableTimeString;
}

//Same as above, but takes in a millisecond value
inline std::string GetTimerTextFromTime(float timeAsMilliseconds, bool haveTimeOnFrontOfString)
{
	int milliSeconds = timeAsMilliseconds;
	int seconds;
	int minutes;
	int hours;

	hours = milliSeconds / (1000*60*60);
	minutes = (milliSeconds % (1000*60*60)) / (1000*60);
	seconds = ((milliSeconds % (1000*60*60)) % (1000*60)) / 1000;


	std::string returnableTimeString;
	if(haveTimeOnFrontOfString)
	{
		returnableTimeString = "Time : ";
	}
	else
	{
		returnableTimeString = "";
	}

	//because we dont want the clock to look like "3:30:3" but rather 3:30:03, add an 0 to the front of the segment if the string size is just 1
	std::string timeString;

	std::stringstream timeStream1(std::stringstream::in | std::stringstream::out);
	timeStream1 << hours;
	timeString = timeStream1.str();
	if(timeString.size() <= 1)
	{
		timeString = "0" + timeString;
	}
	returnableTimeString += timeString;
	returnableTimeString += " : ";

	std::stringstream timeStream2(std::stringstream::in | std::stringstream::out);
	timeStream2 << minutes;
	timeString = timeStream2.str();
	if(timeString.size() <= 1)
	{
		timeString = "0" + timeString;
	}
	returnableTimeString += timeString;
	returnableTimeString += " : ";

	std::stringstream timeStream3(std::stringstream::in | std::stringstream::out);
	timeStream3 << seconds;
	timeString = timeStream3.str();
	if(timeString.size() <= 1)
	{
		timeString = "0" + timeString;
	}
	returnableTimeString += timeString;

	return returnableTimeString;
}
