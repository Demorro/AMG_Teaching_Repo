#pragma once

//These need to be accesible everywhere
enum XBOXCONTROLLERBUTTONS
{
	A,
	B,
	X,
	Y,
	LB,
	RB,
	BACK,
	START,
	LEFT_THUMB,
	RIGHT_THUMB
};

//The boundry of stick movement, so you dont have to push a stick all the way to get the event to trigger
#define ANALOGUESTICKMOVEBOUNDRY 75