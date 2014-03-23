#include "Camera.h"
#include "Application.h"

Camera::Camera(void)
{
	
}

Camera::Camera(sf::RenderWindow &window,sf::Vector2f startPositionOffset)
{
	this->window = &window;
	InitialiseCamera(startPositionOffset);

	Last_Center_Point = sf::Vector2f(-999999,-999999);
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

		simpleCam = true;
	}
	else
	{
		std::cout << "Cannot Initialise Camera, no window exists" << std::endl;
	}

	
}

//This function returns a vector that you can subtract from any position to get it into screenspace, just remember to add it again after
sf::Vector2f Camera::GetScreenSpaceOffsetVector()
{
	sf::Vector2f screenCorrectionMoveVector = GetPosition();
	screenCorrectionMoveVector.x -= Application::GetWindow().getSize().x/2;
	screenCorrectionMoveVector.y -= Application::GetWindow().getSize().y/2;

	return screenCorrectionMoveVector;
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

	if(simpleCam)
	{
		if(!locked)
		{
			JumpToPoint(followTarget->x, followTarget->y);
		}
	}
	else
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
	Center_Point = cameraView.getCenter();
	velocity = (Center_Point - Last_Center_Point);

	//HAAAAAAAAAAAAACK. Stops the parralax from fucking up positions on the first iteration
	if(Last_Center_Point == sf::Vector2f(-999999,-999999))
	{
		velocity = sf::Vector2f(0,0);
	}

	Last_Center_Point = cameraView.getCenter();


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

sf::Vector2f Camera::GetPosition()
{
	return cameraView.getCenter();
}

sf::Vector2f Camera::GetVelocity()
{
	return velocity;
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

bool Camera::GetLocked()
{
	return locked;
}

void Camera::SetLocked(bool value)
{
	locked = value;
}

void Camera::Reset()
{
	JumpToPoint(Application::GetWindow().getSize().x/2,Application::GetWindow().getSize().y/2);
}