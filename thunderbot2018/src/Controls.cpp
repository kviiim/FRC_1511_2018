/*
 * Controls.cpp

 *
 *  Created on: Jan 17, 2018
 *      Author: Robotics
 */

#include <Controls.h>
#include "Ramp.h"
#include <GenericHID.h>

// XBOX controller mapping
// axis
// 0 - Left stick left/right
// 1 - Left stick up/down
// 2 - Left trigger (+ only)
// 3 - Right trigger (+ only)
// 4 - Right stick left/right
// 5 - Right stick up/down
// buttons
// 1 - A
// 2 - B
// 3 - X
// 4 - Y
// 5 - left bumper
// 6 - right bumper
// 7 - select
// 8 - start
// 9 - left stick
// 10 - right stick
// POV - D-pad

#define JOYSTICK_DRIVER 0
#define DRIVER_LEFT_Y_AXIS 1			// up/down
#define DRIVER_RIGHT_X_AXIS 4     // left/right
#define DRIVER_RIGHT_Y_AXIS 5

#define DRIVER_SLOWLEFT_BUTTON 5
#define DRIVER_SLOWRIGHT_BUTTON 6
#define DRIVER_TURBOLEFT_TRIGGER 2
#define DRIVER_TURBORIGHT_TRIGGER 3
#define DRIVER_SWAP_DRIVE_BUTTON 1
#define DRIVER_CALIBRATE_GYRO_BUTTON 3


#define JOYSTICK_AUX 1
#define AUX_INTAKE_TRIGGER 3 //normal intake
#define AUX_OUTTAKE_TRIGGER 2 // normal outtake
#define AUX_RAMPDEPLOY_BUTTON_ONE 7//press with RAMPDEPLOY_TWO to deploy ramps
#define AUX_RAMPDEPLOY_BUTTON_TWO 8//press with RAMPDEPLOY_ONE to deploy ramps
//#define AUX_ARMINTAKE_BUTTON 5 //arm intake
//#define AUX_ARMOUTTAKE_BUTTON 2 //arm outtake
#define AUX_RAMPLIFTLEFT_BUTTON 6 //lift right ramp
#define AUX_RAMPLIFTRIGHT_BUTTON 5 //lift right ramp
#define AUX_ARMMANUAL_RIGHT_Y_AXIS 5 //fine manual arm control
#define AUX_ARMFRONTPOSITION_BUTTON 4 //arm to front position
#define AUX_ARMBACKPOSITION_BUTTON 1 //arm to back position
#define AUX_ARMSWITCHFRONTPOSITION_BUTTON 3 //arm to switch front position
#define AUX_ARMSWITCHBACKPOSITION_BUTTON 2 //arm to switch back position
#define AUX_ARMSWITCHCENTERPOSITION_DPAD_LEFT 270
#define AUX_ARMSWITCHSECONDLEVEL_DPAD_RIGHT 90
#define AUX_HANDPRESSUREOFF_DPAD_DOWN 180


#define AUX_INTAKEUP_DPAD_UP 0 //if arm is broken, moves the intake in
#define AUX_INTAKEDOWN_DPAD_DOWN 180 //if arm is broken, moves the intake out

#define AUX_LEFTREVERSE_DPAD_LEFT 345
#define AUX_RIGHTREVERSE_DPAD_RIGHT 45

#define JOYSTICK_BROKEN 2
#define ARM_INTAKE_BROKEN 1
#define ARM_LIMITSWITCH_FRONT_BROKEN 2
#define ARM_LIMITSWITCH_BACK_BROKEN 3
#define REVERSE_LIFTS 4
//#define RAMPLIFTRIGHT_BROKEN 5
#define INTAKE_BROKEN_SENSOR 5
#define INTAKE_BROKEN 6

const int kDriveYAxisFlipped = 1;

Controls::Controls(CubeSystem *cubeSystem, DriveMeyer *driveMeyer, Lifts *lifts, Feedback *feedback) {

	_cubeSystem = cubeSystem;
	_drive = driveMeyer;
	_lifts = lifts;
	_feedback = feedback;

	_driverJoystick = new Joystick(JOYSTICK_DRIVER);
	_driverSwapDrive = new ControlsButton(_driverJoystick, DRIVER_SWAP_DRIVE_BUTTON);
	_auxJoystick = new Joystick(JOYSTICK_AUX);



	_brokenJoystick = new Joystick(JOYSTICK_BROKEN);
	_brokenArmIntakeMove = new ControlsButton(_brokenJoystick, ARM_INTAKE_BROKEN);
	_brokenArmLimitFront = new ControlsButton(_brokenJoystick, ARM_LIMITSWITCH_FRONT_BROKEN);
	_brokenArmLimitBack = new ControlsButton(_brokenJoystick, ARM_LIMITSWITCH_BACK_BROKEN);
	//_brokenRampLiftLeft = new ControlsButton(_brokenJoystick, RAMPLIFTLEFT_BROKEN);
	//_brokenRampLiftRight = new ControlsButton(_brokenJoystick, RAMPLIFTRIGHT_BROKEN);
	_brokenIntakeSensor = new ControlsButton(_brokenJoystick, INTAKE_BROKEN_SENSOR);
	_reverseLifts = new ControlsButton(_brokenJoystick, REVERSE_LIFTS);
	_brokenCubeIntake = new ControlsButton(_brokenJoystick, INTAKE_BROKEN);

	_swapDrive = false;
	_wasTriggerDown = false;

}

Controls::~Controls() {

}

void Controls::process() {
	ProcessControllerDriver();
	ProcessControllerAux();
	ProcessBroken();
}

float Controls::GetPowerTurn(float power, bool slow, bool turbo) {
	// Cube the power to ramp input
	power = power * power * power;

	// Cut power to /10 for slower driving
	if (slow) {

		power *= .45;
	} else if (turbo) {
		// Do nothing, keep full power

	} else {
		// Cut power to 8/10 for normal operation
		power *= .6;
	}
	return (power);
}

float Controls::GetPower(float power, bool slow, bool turbo) {
	// Cube the power to ramp input
	power = power * power * power;

	// Cut power to 3/10 for slower driving
	if (slow) {
		power *= .3;
	} else if (turbo) {
		// Do nothing, keep full power
	} else {
		// Cut power to 8/10 for normal operation
		power *= .8;
	}
	return (power);
}

//check wiki for

void Controls::ProcessControllerDriver()
{
		double driveLeftY;
		double driveRightX;
		bool slowLeft;
		bool slowRight;
		bool turboLeft;
		bool turboRight;
		double motorDriveLeft;
		double motorDriveRight;
		bool buttonGyroCalibrate;
		bool invTurn;
		int turnMultiplier;

		buttonGyroCalibrate = _driverJoystick->GetRawButton(DRIVER_CALIBRATE_GYRO_BUTTON);

//		if ((_driverSwapDrive->Process() && _driverSwapDrive->Pressed()))
//		{
//			_swapDrive = !_swapDrive;
//		}

		float driveY = GetPosition(_driverJoystick, DRIVER_LEFT_Y_AXIS);

		if(driveY > 0) {
			turnMultiplier = -1;
		}
		else {
			turnMultiplier = 1;
		}

		if(buttonGyroCalibrate) {
			_drive->calibrateGyro();
		}
/*	so casey randomly decided to change his mind bout swap drive so im leaving this here just in case
 *
		if(swapDrive >= 0.3){		//sees that the trigger is down
			if(_wasTriggerDown == false){ //if wasTriggerDown is false the last time it went through the loop...
				_swapDrive = !_swapDrive; //it sets swap drive to the opposite of what it currently is
				_wasTriggerDown = true;
			}
		}
		else{ //if trigger is let go...
			_wasTriggerDown = false;
		}*/

		if(!_swapDrive){
			// the front of the robot goes forward
			driveLeftY = kDriveYAxisFlipped*(-GetPosition(_driverJoystick, DRIVER_LEFT_Y_AXIS)); // joystick Y values are inverted
			driveRightX = turnMultiplier * GetPosition(_driverJoystick, DRIVER_RIGHT_X_AXIS);
			turboLeft = (GetPosition(_driverJoystick, DRIVER_TURBOLEFT_TRIGGER) > 0.15);
			turboRight = (GetPosition(_driverJoystick, DRIVER_TURBORIGHT_TRIGGER) > 0.15);
			slowLeft = _driverJoystick->GetRawButton(DRIVER_SLOWLEFT_BUTTON);
			slowRight = _driverJoystick->GetRawButton(DRIVER_SLOWRIGHT_BUTTON);
		}
		else{
			// the back of the robot goes forward
			driveLeftY = GetPosition(_driverJoystick, DRIVER_LEFT_Y_AXIS); // joystick Y values are inverted
			if(driveLeftY < .05 && driveLeftY > -.05) {
				driveRightX = turnMultiplier * GetPosition(_driverJoystick, DRIVER_RIGHT_X_AXIS);
			}
			else {
				driveRightX = turnMultiplier * -GetPosition(_driverJoystick, DRIVER_RIGHT_X_AXIS);
			}
			turboLeft = (GetPosition(_driverJoystick, DRIVER_TURBOLEFT_TRIGGER) > 0);
			turboRight = (GetPosition(_driverJoystick, DRIVER_TURBORIGHT_TRIGGER) > 0);
			slowLeft =  _driverJoystick->GetRawButton(DRIVER_SLOWLEFT_BUTTON);
			slowRight =  _driverJoystick->GetRawButton(DRIVER_SLOWRIGHT_BUTTON);
		}

		if (driveRightX == 0)
		{

			// drive forward and back
			motorDriveLeft = GetPower(driveLeftY, slowLeft, turboLeft);
			motorDriveRight = GetPower(driveLeftY, slowRight, turboRight);
		}
		else if (driveLeftY == 0)
		{
			// spins left and right
			motorDriveLeft = .7*GetPowerTurn(driveRightX, slowLeft, turboLeft);
			motorDriveRight = .7*-GetPowerTurn(driveRightX, slowRight, turboRight);
		}
		else
		{
			// arching - so set both to same speed (for now)
			motorDriveLeft = GetPower(driveLeftY, slowLeft, turboLeft);
			motorDriveRight = GetPower(driveLeftY, slowRight, turboRight);
			// decrease one side, scale it by how much the right stick is moved
			if (driveRightX > 0)
			{
				// arcing right (decrease right motor)
				motorDriveRight *= (1-(driveRightX * .75));
			}
			else
			{
				// arcing left (decrease left motor)
				// NOTE: invert joystick so we have a positive value for the scale factor
				motorDriveLeft *= -(-1-(driveRightX * .75));
			}

			motorDriveLeft *= .7;
			motorDriveRight *= .7;
		}
		_drive->drive(motorDriveLeft, motorDriveRight);

}

//right trigger: intake
//left trigger: outtake
//both triggers: do nothing
void Controls::ProcessControllerAux()
{
	double triggerIntake;
	double triggerOuttake;
	bool buttonDeployRampOne;
	bool buttonDeployRampTwo;
	bool buttonLeftLift;
	bool buttonRightLift;

	bool buttonArmFrontPosition;
	bool buttonArmBackPosition;
	bool buttonArmSwitchFrontPosition;
	bool buttonArmSwitchBackPosition;
	double axisArmManual;
	//bool buttonArmIntake;
	//bool triggerArmOuttake;

	//buttonArmIntake = _auxJoystick->GetRawButton(AUX_ARMINTAKE_BUTTON);
	//triggerArmOuttake = _auxJoystick->GetRawButton(AUX_ARMOUTTAKE_BUTTON);
	triggerIntake =  _auxJoystick->GetRawAxis(AUX_INTAKE_TRIGGER);
	triggerOuttake = _auxJoystick->GetRawAxis(AUX_OUTTAKE_TRIGGER);

	buttonArmFrontPosition = _auxJoystick->GetRawButton(AUX_ARMFRONTPOSITION_BUTTON);
	buttonArmBackPosition = _auxJoystick->GetRawButton(AUX_ARMBACKPOSITION_BUTTON);
	buttonArmSwitchFrontPosition = _auxJoystick->GetRawButton(AUX_ARMSWITCHFRONTPOSITION_BUTTON);
	buttonArmSwitchBackPosition = _auxJoystick->GetRawButton(AUX_ARMSWITCHBACKPOSITION_BUTTON);
	axisArmManual = GetPosition(_auxJoystick, AUX_ARMMANUAL_RIGHT_Y_AXIS);

	//buttonIntake =  _auxJoystick->GetRawButton(AUX_INTAKE_BUTTON);
	//triggerOuttake = _auxJoystick->GetRawButton(AUX_OUTTAKE_BUTTON);
	buttonDeployRampOne = _auxJoystick->GetRawButton(AUX_RAMPDEPLOY_BUTTON_ONE);
	buttonDeployRampTwo = _auxJoystick->GetRawButton(AUX_RAMPDEPLOY_BUTTON_TWO);

	buttonLeftLift = _auxJoystick->GetRawButton(AUX_RAMPLIFTLEFT_BUTTON);
	buttonRightLift = _auxJoystick->GetRawButton(AUX_RAMPLIFTRIGHT_BUTTON);
	//buttonArmIntake = _auxJoystick->GetRawButton(AUX_ARMINTAKE_BUTTON);
	//triggerArmOuttake = _auxJoystick->GetRawButton(AUX_ARMOUTTAKE_BUTTON);

	if(triggerIntake >= 0.3 && triggerOuttake < 0.3){ //if intake is pressed and outtake isn't...
			_cubeSystem->setCubeOperation(CubeSystem::IN); //intake
	}
	else if(triggerOuttake >= 0.3 && triggerIntake < 0.3){ //if outtake is pressed and intake isn't...
		_cubeSystem->setCubeOperation(CubeSystem::OUT); //outtake
	}
	else{ //if both are pressed or none are pressed...
		_cubeSystem->setCubeOperation(CubeSystem::STOP); //do nothing
	}

	//printf("%f, %f", triggerOuttake, triggerIntake);



  		//preset positions for arm
	if(buttonArmFrontPosition){
		_cubeSystem->setArmPosition(Arm::FRONT);
	}
	else if(buttonArmSwitchFrontPosition){
		_cubeSystem->setArmPosition(Arm::SWITCHFRONT);
	}
	else if(buttonArmBackPosition){
		_cubeSystem->setArmPosition(Arm::BACK);
	}
	else if(buttonArmSwitchBackPosition){
		_cubeSystem->setArmPosition(Arm::SWITCHBACK);
	}
	else if(_auxJoystick->GetPOV() == AUX_ARMSWITCHCENTERPOSITION_DPAD_LEFT){
		_cubeSystem->setArmPosition(Arm::REST);
	}
	else if(_auxJoystick->GetPOV() == AUX_ARMSWITCHSECONDLEVEL_DPAD_RIGHT){
		_cubeSystem->setArmPosition(Arm::SECONDLEVEL);
	}

	if(_cubeSystem->wasArmAtPosition()){
		_cubeSystem->moveArmManually(-axisArmManual);
	}

		//only if broken switch 1 is flipped
	if(_auxJoystick->GetPOV() == AUX_INTAKEUP_DPAD_UP){
		_cubeSystem->setIntakePosition(CubeSystem::EXTEND);
	}
	else if(_auxJoystick->GetPOV() == AUX_INTAKEDOWN_DPAD_DOWN){
		_cubeSystem->setIntakePosition(CubeSystem::RETRACT);
	}

	if(_auxJoystick->GetPOV() == AUX_LEFTREVERSE_DPAD_LEFT){
		_cubeSystem->reverseLeft(true);
	}
	else{
		_cubeSystem->reverseLeft(false);
	}

	if(_auxJoystick->GetPOV() == AUX_RIGHTREVERSE_DPAD_RIGHT){
		_cubeSystem->reverseRight(true);
	}
	else{
		_cubeSystem->reverseRight(false);
	}

	if(_auxJoystick->GetPOV() == AUX_HANDPRESSUREOFF_DPAD_DOWN){
		_cubeSystem->handPressureOff(true);
	}
	else{
		_cubeSystem->handPressureOff(false);
	}

	if(buttonDeployRampOne && buttonDeployRampTwo){
		_lifts->deployLifts();
	}

	if(buttonLeftLift){
		_lifts->raiseLeftLift(true);
	}
	else{
		_lifts->raiseLeftLift(false);
	}
	if(buttonRightLift){
		_lifts->raiseRightLift(true);
	}
	else{
		_lifts->raiseRightLift(false);
	}

#ifdef ARM_TEST
	if(_auxJoystick->GetPOV() == 90) {
		_cubeSystem->_arm->setFrontLimit(true);
		_cubeSystem->_arm->setBackLimit(false);
	} else if(_auxJoystick->GetPOV() == 270) {
		_cubeSystem->_arm->setFrontLimit(false);
		_cubeSystem->_arm->setBackLimit(true);
	} else {
		_cubeSystem->_arm->setFrontLimit(false);
		_cubeSystem->_arm->setBackLimit(false);
	}
#endif
}

void Controls::ProcessBroken(){
	_brokenArmIntakeMove->Process();
	_brokenArmLimitFront->Process();
	_brokenArmLimitBack->Process();
	_brokenIntakeSensor->Process();
	_brokenCubeIntake->Process();
	_reverseLifts->Process();

	_cubeSystem->setCubeBroken(_brokenArmIntakeMove->Pressed());
	_cubeSystem->setFrontLimitBroken(_brokenArmLimitFront->Pressed());
	_cubeSystem->setBackLimitBroken(_brokenArmLimitBack->Pressed());
	//_ramps->setBroken(_brokenRampLiftLeft->Pressed(), _brokenRampLiftRight->Pressed());
	_cubeSystem->setBeamBreakBroken(_brokenCubeIntake->Pressed());
	_lifts->reverseLifts(_reverseLifts->Pressed());
	_cubeSystem->setIntakeSensorBroken(_brokenIntakeSensor->Process());

}

float Controls::GetPosition(Joystick *joystick, int axis, bool fullrange) {
	float position;

	position = joystick->GetRawAxis(axis);

	// Deadzone, prevents motors from running
	if ((position > -.3) && (position < .3)) {
		position = 0; //when not touching the joysticks
	}
	else if (fullrange) {
		if (position > 0) {
			position = (position - .3) / .7;
		}
		else {
			position = ((abs(position) - .3) / .7) * -1;
		}
	}

	return (position);
}

void Controls::Debug(Feedback *feedback) {
	feedback->send_Debug_String("Controls", "Swap Drive", _swapDrive ? "true":"false");
}
