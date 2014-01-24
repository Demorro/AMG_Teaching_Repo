#include "InterStateSingleton.h"



InterStateSingleton::InterStateSingleton()
{

}

InterStateSingleton& InterStateSingleton::GetInstance()
{
	// Instantiated on first use, reference passed thereafter
	static InterStateSingleton instance;
	return instance;
}

void InterStateSingleton::SetIsVolumeOn(bool isVolumeOn)
{
	this->isVolumeOn = isVolumeOn;
}

bool InterStateSingleton::GetIsVolumeOn()
{
	return isVolumeOn;
}