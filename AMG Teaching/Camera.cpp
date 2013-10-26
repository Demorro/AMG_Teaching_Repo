#include "Camera.h"

Camera::Camera(void)
{
	
}

Camera::Camera(sf::RenderWindow &window, float zoomOutAmount, sf::Vector2f startPositionOffset)
{
	this->window = &window;
	InitialiseCamera(zoomOutAmount, startPositionOffset);
}


Camera::~Camera(void)
{
}

void Camera::InitialiseCamera(float zoomOutAmount, sf::Vector2f startPositionOffset)
{
	if(window != NULL)
	{
		cameraView = window->getDefaultView();
		cameraView.zoom(zoomOutAmount);
		cameraView.move(startPositionOffset);

		UpdateView();
	}
	else
	{
		std::cout << "Cannot Initialise Camera, no window exists" << std::endl;
	}
}

void Camera::Update( sf::Event event, bool eventFired, sf::Vector2f *followTarget, sf::Vector2f followOffset)
{
	if(followTarget != NULL)
	{
		JumpToPoint(followTarget->x + followOffset.x,followTarget->y + followOffset.y);
	}

	//If the window is resized, we dont want the image to squash and stretch, so do this
	if(eventFired)
	{
		if (event.type == sf::Event::Resized)
		{
			// update the view to the new size of the window
			sf::Vector2f visibleArea(event.size.width, event.size.height);
			cameraView.setSize(visibleArea);
			UpdateView();
		}
	}
}

void Camera::Move(float xMove, float yMove)
{
	cameraView.move(xMove, yMove);
	UpdateView();
}

void Camera::JumpToPoint(float xPos, float yPos)
{
	cameraView.setCenter(xPos,yPos);
	UpdateView();
}

void Camera::Zoom(float zoom)
{
	cameraView.zoom(zoom);
	UpdateView();
}

void Camera::UpdateView()
{
	if(window != NULL)
	{
		window->setView(cameraView);
	}
	else
	{
		std::cout << "Cannot update view in Camera class as the window dosen't seem to exist" << std::endl;
	}
}
