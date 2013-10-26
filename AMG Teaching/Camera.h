#pragma once
#include "SFML\Graphics.hpp"
#include <iostream>

class Camera
{
public:
	Camera(void);
	Camera(sf::RenderWindow &window, float zoomOutAmount, sf::Vector2f startPositionOffset);
	~Camera(void);

	//You can pass in null and the camera wont follow anything, or you can pass in a position and it will
	void Update(sf::Event event, bool eventFired, sf::Vector2f *followTarget = NULL, sf::Vector2f followOffset = sf::Vector2f(0,0));
	void Move(float xMove, float yMove);
	void JumpToPoint(float xPos, float yPos);
	void Zoom(float zoom);
	//Call to make the camera follow the position supplied, pass null to stop following
	

private:
	sf::View cameraView;

	//Store a reference to the window so you can call setView after every view change
	sf::RenderWindow* window;

	void InitialiseCamera(float zoomOutAmount, sf::Vector2f startPositionOffset);
	
	//Updates the window to the new view, need to call after making any changes to the camera
	void UpdateView();
};

