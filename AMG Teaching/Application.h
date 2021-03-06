#pragma once
#include <SFML\System.hpp>
#include "StateManager.h"

// Window properties
#define WINDOW_TITLE_TEXT		"Loch Ness Warrior"
#define WINDOW_PIXEL_WIDTH		1280
#define WINDOW_PIXEL_HEIGHT		720
#define WINDOW_MAX_FRAMERATE	240
//NEVER HAVE A FRAMELIMITER AND VSYNC AT THE SAME TIME
#define WINDOW_FRAMELIMIT	false
#define WINDOW_V_SYNC		false
#define WINDOW_FULLSCREEN   false

#define TRANSITION_FADE_SPEED 0.005f

class Application
{
public:
	Application();
	~Application();

	// The big update loop, runs the statemanger, refreshes the screen and does event grabbing 
	void Run();

	// Setters
	static void SetRunning(bool running);
	static void SetWindow(sf::RenderWindow* window);

	// Getters
	static bool GetRunning();
	static sf::RenderWindow& GetWindow();

private:
	std::unique_ptr<StateManager> stateManager;

	// Instance of the window used to catch events and display graphical content.
	sf::RenderWindow window;

	// Determines whether or not the application should continue to process.
	static bool running;

	// Pointer to the window instance. 
	static sf::RenderWindow* windowRef;
};
