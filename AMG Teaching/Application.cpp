#include "Application.h"

bool Application::running = true;
sf::RenderWindow* Application::windowRef = NULL;

Application::Application()
{
	// Initialise window
	if(WINDOW_FULLSCREEN)
	{
		window.create(sf::VideoMode(WINDOW_PIXEL_WIDTH, WINDOW_PIXEL_HEIGHT, 32), WINDOW_TITLE_TEXT, sf::Style::Fullscreen);
	}
	else
	{
		window.create(sf::VideoMode(WINDOW_PIXEL_WIDTH, WINDOW_PIXEL_HEIGHT, 32), WINDOW_TITLE_TEXT);
	}

	//Set the window reference to the created window so we can grab it from anywhere
	SetWindow(&window);

	if(WINDOW_FRAMELIMIT)
	{
		window.setFramerateLimit(WINDOW_MAX_FRAMERATE);
	}
	
	window.setVerticalSyncEnabled(WINDOW_V_SYNC);
	stateManager = std::unique_ptr<StateManager>(new StateManager());

}

Application::~Application()
{
}

void Application::Run()
{
	while(window.isOpen())
    {
		bool eventFired = false;

		sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
			{
                window.close();
			}
			eventFired = true;
        }

		stateManager->Update(event, eventFired);
		window.clear();
		stateManager->Draw(window);
        window.display();

		if(!GetRunning())
		{
			window.close();
		}
    }
}

void Application::SetRunning(bool running)
{
	Application::running = running;
}

bool Application::GetRunning()
{
	return running;
}

sf::RenderWindow& Application::GetWindow()
{
		return *windowRef;
}

void Application::SetWindow(sf::RenderWindow* window)
{
	windowRef = window;
}
