#pragma once

#include <iostream>
#include "pugixml.hpp"
#include "pugiconfig.hpp"
#include <sstream>

//Taken from stackoverflow user "Gerog Fritzsche" answer found here : http://stackoverflow.com/questions/3613284/c-stdstring-to-boolean
inline bool to_bool(std::string str)
{
	std::transform(str.begin(), str.end(), str.begin(), ::tolower);
	std::istringstream is(str);
	bool b;
	is >> std::boolalpha >> b;
	return b;
}

//A generic function that loads in a numerical value from the XML in the structure of the provided config file and puts it into the float thats passed in.
//For example, if you wanted to load a value "CheesesEaten" from a config file into float cheeses, you would call LoadNumericalValue(cheeses,rootNode,"CheesesEaten");
inline void LoadNumericalValue(float &valueToLoadInto, pugi::xml_node &rootNode, std::string valueNodeName)
{
	pugi::xml_node workingNode = rootNode.child(valueNodeName.c_str());
	if(workingNode)
	{
		std::stringstream valueStream(workingNode.child_value());
		valueStream >> valueToLoadInto;
	}
	else
	{
		std::cout << "Couldn't find " << valueNodeName << " in config file, using default" << std::endl;
	}
}

inline void LoadTextValue(std::string &valueToLoadInto, pugi::xml_node &rootNode, std::string valueNodeName)
{
	pugi::xml_node workingNode = rootNode.child(valueNodeName.c_str());
	if(workingNode)
	{
		valueToLoadInto = workingNode.child_value();
	}
	else
	{
		std::cout << "Couldn't find " << valueNodeName << " in config file, using default" << std::endl;
	}
}

inline void LoadBooleanValue(bool &boolToLoadInto, pugi::xml_node &rootNode, std::string valueNodeName)
{
	pugi::xml_node workingNode = rootNode.child(valueNodeName.c_str());
	if(workingNode)
	{
		std::string boolString = workingNode.child_value();
		boolToLoadInto = to_bool(boolString);
	}
	else
	{
		std::cout << "Couldn't find " << valueNodeName << " in config file, using true" << std::endl;
		boolToLoadInto = true;
	}
}

inline bool LoadXMLDoc(pugi::xml_document &configDoc, std::string configFilePath)
{
	//Load the config file into memory
	std::cout << std::endl;
	std::cout << "LOADING XML FILE : " << configFilePath << std::endl;
	pugi::xml_parse_result result = configDoc.load_file(configFilePath.c_str());
	if (result)
	{
		std::cout << "XML File "  << configFilePath <<  " was loaded without errors." << std::endl;
		return true;
	}
	else
	{
		std::cout << "XML File " << configFilePath << " had some trouble loading and failed" << std::endl;
		std::cout << "Error description: " << result.description() << "\n";
		return false;
	}
	std::cout << std::endl;
}

//Adapted from stackoverflow users "billz" answer found here : http://stackoverflow.com/questions/13172158/c-split-string-by-line
inline std::vector<std::string> SplitStringIntoLines(std::string input)
{
	std::vector<std::string> returnStrings;

	std::stringstream ss(input);
	std::string to;

	if(input.size() > 0)
	{
		while(std::getline(ss,to))
		{
			returnStrings.push_back(to);
		}
	}

	return returnStrings;
}
