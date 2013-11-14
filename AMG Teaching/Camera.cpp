#include "Camera.h"

Camera::Camera(void)
{
	
}

Camera::Camera(sf::RenderWindow &window,sf::Vector2f startPositionOffset)
{
	this->window = &window;
	InitialiseCamera(startPositionOffset);
}


Camera::~Camera(void)
{
}

void Camera::InitialiseCamera(sf::Vector2f startPositionOffset)
{
	if(window != NULL)
	{
		LoadConfigValues(CAMERACONFIG);
		cameraView = window->getDefaultView();
		cameraView.zoom(zoom);
		//cameraView.move(startPositionOffset);
		JumpToPoint(startPositionOffset.x,startPositionOffset.y);
		locked = false;
		Center_Point = cameraView.getCenter();
		UpdateView();
	}
	else
	{
		std::cout << "Cannot Initialise Camera, no window exists" << std::endl;
	}

	
}

// Load in Camera Config
// Same Deal as in Player.cpp
bool Camera::LoadConfigValues(std::string configFilePath)
{
	//set the default move values, in case the loading dosent work these will be used
	ReturnToCenter = 2.0;
	zoom = 1.0;

	pugi::xml_document configDoc;

	//Load the config file into memory
	std::cout << "Loading Camera Config File : " << configFilePath << std::endl;
	pugi::xml_parse_result result = configDoc.load_file(configFilePath.c_str());
	if (result)
	{
		std::cout << "XML Camera Config File [" << configFilePath << "] was loaded without errors." << std::endl;
	}
	else
	{
		std::cout << "XML Camera Config File [" << configFilePath << "] had some trouble loading and failed, using default player values" << std::endl;
		std::cout << "Error description: " << result.description() << "\n";

		return false;
	}

	//Work through all the variables we need to load and load em, checking for if they're there or not each time
	pugi::xml_node rootNode = configDoc.child("CameraConfig");

	pugi::xml_node workingNode = rootNode.child("CameraReturnToCenterSpeed");
	if(workingNode)
	{
		std::stringstream CameraReturnToCenterSpeedStream(workingNode.child_value());
		CameraReturnToCenterSpeedStream >> ReturnToCenter;
	}
	else
	{
		std::cout << "Couldn't find CameraReturnToCenterSpeed in config file, using default" << std::endl;
	}

	workingNode = rootNode.child("Zoom");
	if(workingNode)
	{
		std::stringstream ZoomStream(workingNode.child_value());
		ZoomStream >> zoom;
	}
	else
	{
		std::cout << "Couldn't find Zoom in config file, using default" << std::endl;
	}
	
	if(DEBUG_CAMERA)
	{
		std::cout << "CAMERA VALUES : " << std::endl;
		std::cout << "Zoom : " << zoom << std::endl;
		std::cout << "ReturntoCenter : " << ReturnToCenter << std::endl;
	}

	return true;
}


void Camera::Update( sf::Event event, bool eventFired,float deltaTime, sf::Vector2f *followTarget)
{

	if(followTarget != NULL)
	{
		// Update Position
		Center_Point = cameraView.getCenter();

		//Target Y Coordinate is the Screen Center position
		JumpToPoint(Center_Point.x, followTarget->y);

		//if not locked move
		if(!locked)
		{
			//Smoothly move Camera to Center on the Target
			MoveToPoint(*followTarget,Center_Point, deltaTime);
		}

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

void Camera::Draw(sf::RenderWindow &window)
{

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

void Camera::MoveToPoint(sf::Vector2f Target, sf::Vector2f Start, float deltaTime)
{

	//calculate the X and Y Distances
	float xDist = ((Target.x - Start.x));
	float yDist = ((Target.y - Start.y));

	//remove stutter when x is close to 0
	if((xDist < 10) && (xDist > 0))
	{
		xDist = 0;
	}

	if((xDist > -5) && (xDist < 0))
	{
		xDist = 0;
	}

	cameraView.move((xDist * deltaTime * ReturnToCenter) , (yDist * deltaTime * ReturnToCenter) );
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

bool Camera::getlocked()
{
	return locked;
}

void Camera::setlocked(bool value)
{
	locked = value;
}