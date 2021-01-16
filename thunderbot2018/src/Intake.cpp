/*
 * Intake.cpp
 *
 *  Created on: Jan 17, 2018
 *      Author: Robotics
 */

#include "Intake.h"
#include "IOMap.h"
#include "WPILib.h"
#include "Feedback.h"

const double kIntakeSpeed = .55; //subject to change
const float kTimeDeployIntake = .5;

Intake::Intake():
	_reverseLeft(false),
	_reverseRight(false),
	_intakeTimer(),
	_intakePosition(RETRACT),
	_intakeDirection(OFF),
	intakeMotorLeft(CAN_OUT_CUBE_INTAKE_LEFT),
	intakeMotorRight(CAN_OUT_CUBE_INTAKE_RIGHT),
	intakeDeployed(DIGITAL_IN_INTAKE_DEPLOYED),
	intakePnu(CAN_PNUEMATICS,PCM_OUT_INTAKE_RAISE,PCM_OUT_INTAKE_LOWER),
	_intakeSensorBroken(false)
{
	_intakeTimer.Start();
	intakeMotorLeft.SetNeutralMode(NeutralMode::Brake);
	intakeMotorRight.SetNeutralMode(NeutralMode::Brake);
	intakePnu.Set(DoubleSolenoid::kForward);
}

void Intake::process(){
	switch(_intakeDirection){
	case IN:
		setMotorSpeed(kIntakeSpeed);	//direction might need to b changed
		break;
	case OUT:
		setMotorSpeed(-kIntakeSpeed);	//direction might need to b changed
		break;
	case OFF:
		setMotorSpeed(0);
		break;
	}
	switch(_intakePosition){
	case RETRACT:
		intakePnu.Set(DoubleSolenoid::kForward);
		break;
	case EXTEND:
		if(_intakeTimer.Get() > kTimeDeployIntake && !intakeDeployed.Get()){
			_intakePosition = RETRACT;
		}
		intakePnu.Set(DoubleSolenoid::kReverse);
		break;
	}
}

void Intake::setDirection(intakeDirection direction)
{
	_intakeDirection = direction;
}
void Intake::setPosition(intakePosition position)
{
	if(_intakePosition != position){
		_intakeTimer.Reset();
	}
	_intakePosition = position;
}
void Intake::reset(){
	_intakeDirection = OFF;
	intakeMotorLeft.Set(ControlMode::PercentOutput, 0);
	intakeMotorRight.Set(ControlMode::PercentOutput, 0);
}
void Intake::setMotorSpeed(double speed){
	if(!_reverseLeft){
		intakeMotorLeft.Set(ControlMode::PercentOutput, speed);
	}
	else{
		intakeMotorLeft.Set(ControlMode::PercentOutput, -speed);
	}
	if(!_reverseRight){
		intakeMotorRight.Set(ControlMode::PercentOutput, speed);
	}
	else{
		intakeMotorRight.Set(ControlMode::PercentOutput, -speed);

	}
}
void Intake::setIntakeSensorBroken(bool broken) {
	_intakeSensorBroken = broken;
}

bool Intake::isIntakeSensorBroken(){
	return _intakeSensorBroken;
}

bool Intake::isIntakeExtended(){
	if(isIntakeSensorBroken()){
		if(_intakeTimer.Get() > kTimeDeployIntake && _intakePosition == EXTEND){
			return true;
		}
		else{
			return false;
		}
	}
	else{
		return !intakeDeployed.Get();
	}
}

void Intake::debug(Feedback *feedback) {
	feedback->send_Debug_String("Intake", "Sensor Deployed?", isIntakeExtended() ? "True":"False");
	feedback->send_Debug_String("Intake", "intake sensor broke", _intakeSensorBroken ? "True":"False");

	switch(_intakeDirection) {
	case IN:
		feedback->send_Debug_String("Intake", "Direction", "In");
		break;
	case OUT:
		feedback->send_Debug_String("Intake", "Direction", "Out");
		break;
	case OFF:
		feedback->send_Debug_String("Intake", "Direction", "Off");
		break;
	}

	switch(_intakePosition) {
	case EXTEND:
		feedback->send_Debug_String("Intake", "Position", "Extended");
		break;
	case RETRACT:
		feedback->send_Debug_String("Intake", "Position", "Retracted");
		break;
	}
}
