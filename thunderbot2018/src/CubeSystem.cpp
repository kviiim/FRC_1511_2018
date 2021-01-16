/*
 * CubeSystem.cpp
 *
 *  Created on: Jan 24, 2018
 *      Author: Robotics
 */

#include <CubeSystem.h>
CubeSystem::CubeSystem(Arm *arm, Intake *intake):
	_armPosition(Arm::REST),
	_cubeOperation(STOP),
	_intakePosition(RETRACT),
	_armBroken(false),
	_armSpeed(0),
	_beamBroken(false),
	_inAuto(false)
{
	_arm = arm;
	_intake = intake;
}

void CubeSystem::Process(){

	if(!_armBroken){	//Intake controls, if the arm pot works then just automatically move based on that
		if(!_arm->_manualMode){
			switch(_armPosition){
				case Arm::FRONT:
					_intake->setPosition(Intake::EXTEND);
					break;
				case Arm::SWITCHFRONT:
					_intake->setPosition(Intake::RETRACT);
					break;
				case Arm::BACK:
					_intake->setPosition(Intake::EXTEND);
					break;
				case Arm::SWITCHBACK:
					break;
				case Arm::REST:
					break;
				case Arm::SECONDLEVEL:
					_intake->setPosition(Intake::EXTEND);
					break;
			}
			if (_armPosition == Arm::SWITCHBACK || _armPosition == Arm::REST){
				if(_arm->isArmInFront() && !_inAuto){
					_intake->setPosition(Intake::EXTEND);
					}
					else{
						_intake->setPosition(Intake::RETRACT);
					}
			}
		}
			if((_arm->isArmInFront() || _armPosition == Arm::BACK || _armPosition == Arm::FRONT || _armPosition == Arm::SECONDLEVEL) && !_inAuto){
			_intake->setPosition(Intake::EXTEND);
			}
			else if(_armPosition != Arm::BACK){
				_intake->setPosition(Intake::RETRACT);
			}

	}
	else{
		switch(_intakePosition){	//if arm pot doesnt work then go based on controls
			case EXTEND:
				_intake->setPosition(Intake::EXTEND);
		 		break;
			case RETRACT:
				_intake->setPosition(Intake::RETRACT);
				break;
		}
	}

	switch(_cubeOperation){
		case STOP:
			if(_arm->isFullMotion() && _arm->isCubePresent() && !_arm->isCubeSensorBroken()){
				_arm->setArmState(Arm::INTAKE);
			}
			_intake->setDirection(Intake::OFF);
			_arm->setArmState(Arm::STOP);
			break;
		case IN:
			if((!_arm->isCubePresentSensor() || _arm->isFullMotion())  || _arm->isCubeSensorBroken()){
				if(canRunIntake()){
						_intake->setDirection(Intake::IN);
						_arm->setArmState(Arm::INTAKE);
				}
				else{
					_arm->setArmState(Arm::INTAKE);
					_intake->setDirection(Intake::OFF);
				}
			}
			else{
				_arm->setArmState(Arm::STOP);
				_intake->setDirection(Intake::OFF);
			}
			break;
		case OUT:
			if(canRunIntake()){
				_intake->setDirection(Intake::OUT);
				_arm->setArmState(Arm::OUTTAKE);
			}
			else{
				_arm->setArmState(Arm::OUTTAKE);
				_intake->setDirection(Intake::OFF);
			}
			break;
	}
	armCubeInterfere();
}

void CubeSystem::setArmPosition(Arm::armPosition position){
	if(_armPosition != position){
		if((_armPosition == Arm::FRONT && position == Arm::BACK) || (_armPosition == Arm::BACK && position == Arm::FRONT) || (_armPosition == Arm::SECONDLEVEL && position == Arm::BACK)){
			_arm->armFullMotion(true);
		}

		else{
			_arm->armFullMotion(false);
		}
	}
	_arm->_manualMode = false;
	_arm->setArmPosition(position);
	_armPosition = position;
}

void CubeSystem::setCubeOperation(cubeOperation operation){
	_cubeOperation = operation;
}

void CubeSystem::setCubeBroken(bool borken){
	_armBroken = borken;
	_arm->setArmBroken(borken);
}

void CubeSystem::moveArmManually(float speed){
	_armSpeed = speed;
	_arm->setArmSpeed(speed);
	_arm->_manualMode = true;
}

void CubeSystem::inAuto(bool inAuto){
	_inAuto = inAuto;
}

void CubeSystem::setIntakePosition(intakePosition position) {
	_intakePosition = position;
}

bool CubeSystem::wasArmAtPosition(){
	return _arm->isArmAtPosition();
}

void CubeSystem::setBackLimitBroken(bool broken){
	_arm->setBackLimitBroken(broken);
}

void CubeSystem::setFrontLimitBroken(bool broken){
	_arm->setFrontLimitBroken(broken);
}

void CubeSystem::setBeamBreakBroken(bool broken){
	_arm->setCubeSensorBroken(broken);
}

void CubeSystem::setIntakeSensorBroken(bool broken){
	_intake->setIntakeSensorBroken(broken);
}

void CubeSystem::armCubeInterfere(){
#ifdef rb2
	_arm->armIntake(false);//_arm->isArmInFront() && !_intake->isIntakeExtended() && !_intake->isIntakeSensorBroken()); //THIS IS BAD
#else
	_arm->armIntake(_arm->isArmInFront() && !_intake->isIntakeExtended() && !_intake->isIntakeSensorBroken());
#endif
}

bool CubeSystem::canRunIntake(){
#ifdef rb2
	return true;//(_intake->isIntakeExtended() && !_intake->isIntakeSensorBroken() && _arm->isArmInFront()) || (_intake->isIntakeSensorBroken() && (_arm->isArmInFront() || _armBroken));	//THIS NEEDS TO BE FIXED!!!!!!!!!
#else
	return (_intake->isIntakeExtended() && !_intake->isIntakeSensorBroken() && _arm->isArmInFront()) || (_intake->isIntakeSensorBroken() && (_arm->isArmInFront() || _armBroken));	//THIS NEEDS TO BE FIXED!!!!!!!!!
#endif
}

void CubeSystem::reverseLeft(bool left){
	_intake->_reverseLeft = left;
}
void CubeSystem::reverseRight(bool right){
	_intake->_reverseRight = right;
}

void CubeSystem::debug(Feedback *feedback) {
	feedback->send_Debug_String("Cube System", "Can run intake?", canRunIntake() ? "true":"false");
	feedback->send_Debug_Double("Cube System", "arm speed", _armSpeed);
	switch(_intakePosition){
	case EXTEND:
		feedback->send_Debug_String("Cube System", "Intake Position", "Extended");
		break;
	case RETRACT:
		feedback->send_Debug_String("Cube System", "Intake Position", "Retract");
		break;
	}
	feedback->send_Debug_Double("thunderdashboard", "green", _arm->isCubePresent() ? 1:0);
	feedback->send_Debug_Double("thunderdashboard", "armpos", _arm->getDashboardArmPos());
	feedback->send_Debug_Double("thunderdashboard", "intake", _intake->isIntakeExtended() ? 1:0);

}

bool CubeSystem::isCubePresent(){
	return _arm->isCubePresent();
}

void CubeSystem::reset(){

}

void CubeSystem::handPressureOff(bool off){
	_arm->turnHandPressureOff(off);
}
