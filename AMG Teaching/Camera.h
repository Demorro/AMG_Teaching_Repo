#pragma once
#include "SFML\Graphics.hpp"
#include <iostream>
#include <sstream>
#include "Assets.h"
#include "pugixml.hpp"
#include "VectorMath.h"

#define DEBUG_CAMERA false

class Camera
{
public:
	Camera(void);
	Camera(sf::RenderWindow &window,sf::Vector2f startPositionOffset);
	~Camera(void);

	//You can pass in null and the camera wont follow anything, or you can pass in a position and it will

	//CameraOffset Must be divisible by Stepsize otherwise bad things will happen
	void Update(sf::Event event, bool eventFired,float deltaTime, sf::Vector2f *followTarget = NULL);
	void Move(float xMove, float yMove);
	void JumpToPoint(float xPos, float yPos);
	void Zoom(float zoom);

	bool getlocked();

	void setlocked(bool value);
	private:
	sf::View cameraView;
	bool LoadConfigValues(std::string configFilePath);

	//Store a reference to the window so you can call setView after every view change
	sf::RenderWindow* window;
	float zoom;
	float ReturnToCenter;
	void MoveToPoint(sf::Vector2f Target , sf::Vector2f Start, float deltaTime);
	void InitialiseCamera(sf::Vector2f startPositionOffset);
	bool locked;
	//Updates the window to the new view, need to call after making any changes to the camera
	void UpdateView();

	// Position of the Center of the Screen
	sf::Vector2f Center_Point;

private:
	//Elliot : Added this so I can test precise collision, this is just a 1 to 1 camera/player follow as it was before, set in initialise
	bool simpleCam;
};

