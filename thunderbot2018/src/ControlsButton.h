#ifndef CONTROLSBUTTON_H
#define CONTROLSBUTTON_H

#include "WPILib.h"

class ControlsButton{

public:
	// constructor
	// joystick		- pointer to joystick that has the button to use
	// button		- button number to use (1 means button 1)
	//					OR the POV angle if isPov is true
	// isPov        - it is on the POV?
	ControlsButton(Joystick *joystick, int button);
	ControlsButton(Joystick *joystick, int button, bool isPov);

	// destructor
	~ControlsButton();

	// check to see if the button state has changed.
	// call this when processing controls.
	// if it returns true, then call Pressed() to get the current state
	bool Process();

	// get the current state of the button
	bool Pressed();

private:
	// joystick that has the button
	Joystick *_joystick = NULL;

	// button on the joystick OR POV angle
	int _button = -1;

	// current state of the button
	bool _isPressed = false;

	// if this a POV based button?
	bool _isPov = false;

	// did we do an initialization yet?
	bool _init = false;
};

#endif // CONTROLSBUTTON_H
