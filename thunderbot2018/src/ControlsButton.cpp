#include "WPILib.h"
#include <ControlsButton.h>

// constructor
// joystick		- pointer to joystick that has the button to use
// button		- button number to use (1 means button 1)
ControlsButton::ControlsButton(Joystick *joystick, int button)
{
	_joystick = joystick;
	_button = button;
	_isPressed = false;
	_isPov = false;
	_init = false;
}
ControlsButton::ControlsButton(Joystick *joystick, int button, bool isPov)
{
	_joystick = joystick;
	_button = button;
	_isPressed = false;
	_isPov = isPov;
	_init = false;
}

// destructor
ControlsButton::~ControlsButton()
{
	// nothing to do
}

// check to see if the button state changed.
// call this when processing controls.
// if it returns true, then call Pressed() to get the current state
bool ControlsButton::Process()
{
	bool ret = !_init;	// what to return if press not detected. that way we
						// say the state changed the very first time Process()
						// is called. this helps the caller get things started

	// have now initialized
	_init = true;

	// validate our variables
	if (_joystick == NULL)
	{
		return (ret);
	}

	// if not POV, then it is a button
	if (!_isPov)
	{
		if (_button < 1)
		{
			return (ret);
		}

		// see if button state is different than the last time we looked
		if (_joystick->GetRawButton(_button) != _isPressed){
			// state changed, so flip our variable
			_isPressed = !_isPressed;
			return (true);
		}

		// button state did not change
		return (ret);
	}

	// if here, then a POV

	// make sure the setting is valid
	switch (_button)
	{
		default:
			return (ret);
		case 0:
		case 90:
		case 180:
		case 270:
			break;
	}

	// see if it is 'pressed'
	if (_joystick->GetPOV() == _button)
	{
		// is pressed, so if we didn't know it was pressed, then it just became pressed
		if (!_isPressed)
		{
			_isPressed = true;
			return (true);
		}
	}
	else
	{
		// is NOT pressed, so if we didn't know it was not pressed, then it just became not pressed
		if (_isPressed)
		{
			_isPressed = false;
			return (true);
		}
	}

	// button state did not change
	return (ret);
}

// get the current state of the button
bool ControlsButton::Pressed()
{
	return (_isPressed);
}
