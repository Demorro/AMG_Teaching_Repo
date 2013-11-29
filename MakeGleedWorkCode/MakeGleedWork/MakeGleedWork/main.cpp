#include <iostream>
#include "pugiconfig.hpp"
#include "pugixml.hpp"
#include <Windows.h>
#include "dirent.h"
#include <vector>

std::string gleedSettingsPath = "settings.xml";

bool ChangeLevelContentRoot(std::string levelPath)
{
	return false;
}


int main()
{

	//First set the default root content folder in gleed2D
	pugi::xml_document gleedSettings;
	
	//Load the xml file into memory
	std::cout << "Loading Gleed Settings XML File " << std::endl;
	pugi::xml_parse_result result = gleedSettings.load_file(gleedSettingsPath.c_str());
	if (result)
	{
		std::cout << "Gleed settings was loaded without errors." << std::endl;
	}
	else
	{
		std::cout << "Gleed settings had some trouble loading and failed" << std::endl;
		std::cout << "Error description: " << result.description() << "\n";
		std::cin.get();
		return false;
	}
	std::cout << std::endl;

	//Get the path that the executeable is in, make it a string
	HMODULE hModule = GetModuleHandleW(NULL);
	WCHAR path[MAX_PATH];
	GetModuleFileNameW(hModule, path, MAX_PATH);

	//convert from wide char to narrow char array
    char ch[MAX_PATH];
    char DefChar = ' ';
    WideCharToMultiByte(CP_ACP,0,path,-1, ch,260,&DefChar, NULL);

	//make a nice string with just the path
	std::string currentFilePath(ch);

	//This is the file and name of the executeable, needs to be removed to set the correct content root folder
	std::string stuffToRemove = "/LevelEditor/MakeGleedWork.exe";
	currentFilePath.erase(currentFilePath.end() - stuffToRemove.size(), currentFilePath.end());

	std::cout << "Setting content root folder of Gleed2d setting as : " << currentFilePath << std::endl;

	gleedSettings.child("Constants").child("DefaultContentRootFolder").first_child().set_value(currentFilePath.c_str());
	gleedSettings.save_file(gleedSettingsPath.c_str());

	std::cout << std::endl;

	//save the root folder path for later
	std::string rootFolderPath = currentFilePath;

	//go to the levels folder
	std::string fromRootToLevels = "\\Assets\\Levels";
	currentFilePath.append(fromRootToLevels);

	std::cout << "Moving to :" << currentFilePath << std::endl;

	//copy the template level from here to there
	std::string templateLevelName = "TemplateLevel.xml";
	pugi::xml_document templateLevel;
	templateLevel.load_file(templateLevelName.c_str());
	std::string savePath = currentFilePath;
	savePath.append("\\" + templateLevelName);
	templateLevel.save_file(savePath.c_str());
	std::cout << "Putting TemplateLevel.xml in :" << savePath << std::endl;
	
	


	std::cout << "Files found in levels folder : " << std::endl;
	//stores the file names from a scan through this directory
	std::vector<std::string> levelFileNames;
	//MOST OF THIS CODE SNIPPET IS NOT MINE, TAKEN FROM http://stackoverflow.com/questions/612097/how-can-i-get-a-list-of-files-in-a-directory-using-c-or-c uses dirent.h
	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir (currentFilePath.c_str())) != NULL) {
	  /* print all the files and directories within directory */
	  while ((ent = readdir (dir)) != NULL) {
		std::cout << ent->d_name << std::endl;
		levelFileNames.push_back(std::string(ent->d_name));
	  }
	  closedir (dir);
	} else {
	  /* could not open directory */
	  std::cout << "Could not open levels directory" << std::endl;
	  perror ("");
	  return EXIT_FAILURE;
	}

	for(int i = 0; i < levelFileNames.size(); i++)
	{
		if((levelFileNames[i] == ".") || (levelFileNames[i] == ".."))
		{
			//ignore the weird directory dots
			continue;
		}
		else
		{
			std::cout << std::endl;
			std::cout << "Changing the content root folder in : " << levelFileNames[i] << std::endl;

			//Load the xml file into memory
			pugi::xml_document levelFile;
			std::cout << "Loading Gleed Settings XML File " << std::endl;
			pugi::xml_parse_result result = levelFile.load_file((currentFilePath + "\\" + levelFileNames[i]).c_str());
			if (result)
			{
				std::cout << "Gleed settings was loaded without errors." << std::endl;
			}
			else
			{
				std::cout << "Gleed settings had some trouble loading and failed" << std::endl;
				std::cout << "Error description: " << result.description() << "\n";
				std::cin.get();
				return false;
			}

			pugi::xml_node rootFolder = levelFile.child("Level").child("EditorRelated").child("ContentRootFolder").first_child();
			rootFolder.set_value(rootFolderPath.c_str());
			levelFile.save_file((currentFilePath + "\\" + levelFileNames[i]).c_str());
		}
	}
	////



	//C:\Coding\C++\AMG Teaching\AMG_Teaching_Repo
	std::cin.get();
	return 0;
}
