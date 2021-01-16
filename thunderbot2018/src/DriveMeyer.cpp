#include "DriveMeyer.h"
#include <stdio.h>
#include <math.h>

const float MOTOR_RAMP_AMOUNT = .1;
const float kAutoRampAmount = .01;
const int kCounterCountPerRotation = 256;
const float kInchesPerWheelRotation = 12.57;
const float kVisionTurnDeadzone = .1;
const float kTurnSpeedMin = .3;
const float kAngleDeadzone = 20;
const int kAngleStartSlow = 30;
const float kStraightDeadzone = 3; //13
const float kStraightDrift = 5;
const float kStartRampingDistance = 12;
const float kMaxMotorSpeed = 4000;
const float kMaxTurnSpeed = .75;
const float kAnnoyanceSec = .5;
const float kTurnSlow = .75;

DriveMeyer::DriveMeyer(Vision *vision):
	_visionDirection(NOTURN),
#ifdef OLDGYRO
	_gyro(ANALOG_IN_DRIVE_GYRO),
#endif
	_gyroCalibrated(false),
	_leftCounterBroken(false),
	_rightCounterBroken(false),
	_leftMotorSpeed(0),
	_rightMotorSpeed(0),
	_leftFinalSpeed(0),
	_rightFinalSpeed(0),
	_autoMaxSpeed(0),
	_visionTurnOffset(0),
	_turnAngle(0),
	_autoTurnFinished(true),
	_autoStraightFinished(true),
	_distance(0),
	_maxSpeed(0),
	_accumulatedCounterDifference(0),
	_turnSpeed(0),
	_annoyanceVal(false),
	_visionTrackFinished(false),
	_wantedTargetArea(0),
	_guessTurnLeft(false),
	_visionKeepGoing(false),
	_autoDoneTimer()

{
	_vision = vision;
	_state = driveState::DISABLED;
	_leftCounter = new DriveCounter(DIGITAL_IN_DRIVE_LEFT_ENCODER);
	_rightCounter = new DriveCounter(DIGITAL_IN_DRIVE_RIGHT_ENCODER);
#ifdef rb2017
	leftMotor = new Spark(PWM_OUT_DRIVE_LEFT);
	leftMini = new Spark(PWM_OUT_MINI_LEFT);
	rightMotor = new Spark(PWM_OUT_DRIVE_RIGHT);
	rightMini = new Spark(PWM_OUT_MINI_RIGHT);
#else
	_leftMotors = new Spark(PWM_OUT_DRIVE_LEFT);
	_rightMotors = new Spark(PWM_OUT_DRIVE_RIGHT);
#endif
#ifndef OLDGYRO
	_gyro = new ADXRS450_Gyro(SPI::kOnboardCS0);
#endif
	_gyroAnnoyanceTimer.Start();
	_visionTurnTimer.Start();
	_autoDoneTimer.Start();
}

void DriveMeyer::drive(float leftSpeed, float rightSpeed) {
	_state = TELEOP;
	_leftFinalSpeed = leftSpeed;
	_rightFinalSpeed = rightSpeed;
}

void DriveMeyer::autoDriveStraight(float distance, float maxSpeed) {
	reset();
	_autoStraightFinished = false;
	_state = AUTO_STRAIGHT;
	_distance = distance;
	_leftCounter->Reset();
	_rightCounter->Reset();
	_autoMaxSpeed = maxSpeed;
	_leftMotorSpeed = maxSpeed;
	_rightMotorSpeed = maxSpeed;
}

void DriveMeyer::autoTurn(float angle) {
	_autoTurnFinished = false;
	_turnAngle = angle;
#ifndef OLDGYRO
	_gyro->Reset();
#else
	_gyro.Reset();
#endif
	_state = AUTO_TURN;
	_turnSpeed = ((.8 * abs(angle)) / 180) + .2;
}

void DriveMeyer::autoArcTurn(float leftSpeed, float rightSpeed, float angle){
	_leftFinalSpeed = leftSpeed;
	_rightFinalSpeed = rightSpeed;
	_turnAngle = angle;
	_autoTurnFinished = false;
	_state = AUTO_ARC_TURN;
#ifndef OLDGYRO
	_gyro->Reset();
#else
	_gyro.Reset();
#endif
}

void DriveMeyer::autoVisionTurn(float offset, float angle){
	_state = AUTO_VISION_TURN;
	_autoTurnFinished = false;
	_turnAngle = angle;
	_visionTurnOffset = offset;
}

void DriveMeyer::autoVisionGoTo(bool startTurnLeft, double area, Vision::VisionTarget visionTarget, bool keepGoing){
	_wantedTargetArea = area;
	_leftMotorSpeed = 0;
	_rightMotorSpeed = 0;
	_guessTurnLeft = startTurnLeft;
	_state = AUTO_VISION_GO_TO;
	_visionTrackFinished = false;
	_vision->setTarget(visionTarget);
	_visionDirection = NOTURN;
	_visionKeepGoing = keepGoing;
}

void DriveMeyer::reset() {
	_leftMotorSpeed = 0;
	_rightMotorSpeed = 0;
	_leftFinalSpeed = 0;
	_rightFinalSpeed = 0;
	_leftCounter->Reset();
	_rightCounter->Reset();
	setLeftMotors(0);
	setRightMotors(0);
	_state = DISABLED;
	_leftCounter->Reset();
	_rightCounter->Reset();
}

void DriveMeyer::process() {

	//printf("Gyro Angle: %f", _gyro->GetAngle());
//printf("%d %d \n", getLeftCounter(), getRightCounter());

	_leftCounter->process();
	_rightCounter->process();

	switch(_state) {
		case driveState::TELEOP:
			_leftMotorSpeed = _leftFinalSpeed;
			_rightMotorSpeed = _rightFinalSpeed;
			break;
		case driveState::AUTO_STRAIGHT:
			//if(_autoMaxSpeed < 0) _distance *= -1;
			if(!(_leftCounterBroken && _rightCounterBroken)){
				if(getCounterAverage() < _distance - kStraightDeadzone)
				{
					_autoDoneTimer.Reset();
					float remainingDistance = _distance - getCounterAverage();
					if(remainingDistance < (((_leftMotorSpeed + _rightMotorSpeed)/2) - .2)*10){
						printf("IT GOT HERE \n");
						//decreases the speed by .1 from starting speed to .2 as it approaches the end
						_leftMotorSpeed -= .1;
						_rightMotorSpeed -= .1;
					}
					if(getLeftSpeed() < getRightSpeed() - 2) {
						_leftMotorSpeed += .05;
						_rightMotorSpeed -= .05;
					}
					if(getLeftSpeed() > getRightSpeed() + 2) {
						_leftMotorSpeed -= .05;
						_rightMotorSpeed += .05;
					}
				} else {
					_leftMotorSpeed = 0;
					_rightMotorSpeed = 0;
					if(_autoDoneTimer.Get() > .15){
						_autoStraightFinished = true;
					}
				}
			}
			else {
				_leftMotorSpeed = 0;
				_rightMotorSpeed = 0;
				printf("Auto Straight Failed: Both encoders are broken");
			}
			//_leftMotorSpeed -= .001 * (getLeftSpeed() - 50.0); printf("%f, %f\n", getLeftSpeed(), _leftMotorSpeed);
			//_rightMotorSpeed -= .001 * (getRightSpeed() - 50.0);
			break;
		case driveState::AUTO_TURN:
			if(!_autoTurnFinished){
#ifndef OLDGYRO
				if(_turnAngle > _gyro->GetAngle() + kAngleDeadzone && _turnAngle > 0){
#else
				if(_turnAngle > -_gyro.GetAngle() + kAngleDeadzone && _turnAngle > 0){
#endif
					_autoDoneTimer.Reset();
					_rightMotorSpeed = -rampAutoTurnSpeed();
					_leftMotorSpeed = rampAutoTurnSpeed();
				}
#ifndef OLDGYRO
				else if(_turnAngle < _gyro->GetAngle() - kAngleDeadzone && _turnAngle < 0){
#else
				else if(_turnAngle < -_gyro.GetAngle() - kAngleDeadzone && _turnAngle < 0){
#endif
						_autoDoneTimer.Reset();
						_rightMotorSpeed = rampAutoTurnSpeed();
						_leftMotorSpeed = -rampAutoTurnSpeed();
				}
				else{
					_rightMotorSpeed = 0;	//if turn is done then set speeds to zero, and say turn is done
					_leftMotorSpeed = 0;
					if(_autoDoneTimer.Get() > .25){
						_autoTurnFinished = true;
					}
				}
			}

			break;
		case driveState::AUTO_ARC_TURN:
			if(!_autoTurnFinished){
#ifdef OLDGYRO
				if(abs(-_gyro.GetAngle()) < abs(_turnAngle) - kAngleDeadzone){
#else
					if(abs(_gyro->GetAngle()) < abs(_turnAngle) - kAngleDeadzone){
#endif
					_leftMotorSpeed = _leftFinalSpeed;
					_rightMotorSpeed = _rightFinalSpeed;
				}
				else{
					_autoTurnFinished = true;
					_rightMotorSpeed = 0;
					_leftMotorSpeed = 0;
				}
			}
			break;
		case driveState::AUTO_VISION_GO_TO:
			if(!_visionTrackFinished){
				if(_vision->targetsFound()){
					_visionTurnTimer.Reset();
					if(_vision->getArea() < _wantedTargetArea){
						float multiplier = 1;
						if(_vision->getArea() > _wantedTargetArea - 5){
							multiplier = .5;
						}
						_leftMotorSpeed = (-.25 - (_vision->getXPosition()/-4))*multiplier;
						_rightMotorSpeed = (-.25 + (_vision->getXPosition()/-4))*multiplier;
						if(_leftMotorSpeed > _rightMotorSpeed ){
							_visionDirection = TURNRIGHT;
						}
						else if(_rightMotorSpeed > _leftMotorSpeed){
							_visionDirection = TURNLEFT;
						}
					}
					else{
						_visionTrackFinished = true;
						_leftMotorSpeed = 0;
						_rightMotorSpeed = 0;
					}
				}
				else if(_visionTurnTimer.Get() < 1.5){
						switch (_visionDirection){
							case TURNLEFT:
								_leftMotorSpeed = -.35;
								_rightMotorSpeed = .35;
								break;
							case TURNRIGHT:
								_leftMotorSpeed = .35;
								_rightMotorSpeed = -.35;
								break;
							case NOTURN:
								if(_guessTurnLeft){
									_leftMotorSpeed = -.35;
									_rightMotorSpeed = .35;
								}
								else{
									_leftMotorSpeed = .35;
									_rightMotorSpeed = -.35;
								}
								break;
						}
					}
					else if(_visionTurnTimer.Get() < 3){
						_leftMotorSpeed = 0;
						_rightMotorSpeed = 0;
					}
					else{
						_visionTurnTimer.Reset();
					}
				}
			break;
		case driveState::AUTO_VISION_TURN:
			if(!_autoTurnFinished){
				if(_vision->targetsFound()){	//if we actually find targets
					if(_vision->getXPosition() > _visionTurnOffset + kVisionTurnDeadzone){ //if our current position is greater than where we want to be...
						if(_vision->getXPosition() - _visionTurnOffset > kTurnSpeedMin){	//changes speed based off of how far away we are from the target
							_rightMotorSpeed = _vision->getXPosition() - _visionTurnOffset;
							_leftMotorSpeed = -(_vision->getXPosition() - _visionTurnOffset);
						}
						else{
							_rightMotorSpeed = kTurnSpeedMin;	//but it has a minimum so the robot actually turns all the way
							_leftMotorSpeed = -kTurnSpeedMin;
						}
					}
					else if(_vision->getXPosition() < _visionTurnOffset - kVisionTurnDeadzone){		//same thing as the other one but turn the other way
						if(_visionTurnOffset - _vision->getXPosition() > kTurnSpeedMin){
							_rightMotorSpeed = -(_visionTurnOffset - _vision->getXPosition());
							_leftMotorSpeed = _vision->getXPosition() - _visionTurnOffset;

						}
						else{
							_rightMotorSpeed = -kTurnSpeedMin;
							_rightMotorSpeed = kTurnSpeedMin;
						}
					}
					else{
						_autoTurnFinished = true;	//if we are within a certain range of the target, then stop and say turn finished
						_rightMotorSpeed = 0;
						_leftMotorSpeed = 0;
					}
				}
				else{	//if we dont find a target then make an educated guess of direction
					if(_turnAngle > 0){
						_rightMotorSpeed = -kTurnSpeedMin;
						_rightMotorSpeed = kTurnSpeedMin;
					}
					else{
						_rightMotorSpeed = kTurnSpeedMin;
						_leftMotorSpeed = -kTurnSpeedMin;
					}
				}
			}
			break;
		case driveState::DISABLED:
			_leftMotorSpeed = 0;
			_rightMotorSpeed = 0;
			break;
	}
	setLeftMotors(_leftMotorSpeed);
	setRightMotors(_rightMotorSpeed);
}

void DriveMeyer::calibrateGyro() {
	if(!_gyroCalibrated) {
		_gyroCalibrated = true;
#ifndef OLDGYRO
		_gyro->Calibrate();
#else
		_gyro.Calibrate();
#endif
		_gyroAnnoyanceTimer.Stop();
		SmartDashboard::PutNumber("thunderdashboard_gyro",0);

	}
}

void DriveMeyer::setLeftCounterBroken(bool broken) {
	_leftCounterBroken = broken;
}

void DriveMeyer::setRightCounterBroken(bool broken) {
	_rightCounterBroken = broken;
}

float DriveMeyer::getLeftCounter() {
	return ((float)_leftCounter->Get()/kCounterCountPerRotation)*kInchesPerWheelRotation;
}

float DriveMeyer::getRightCounter() {
	return ((float)_rightCounter->Get()/kCounterCountPerRotation)*kInchesPerWheelRotation;
}
float DriveMeyer::rampAutoTurnSpeed(){
#ifndef OLDGYRO
	return (-.7*(kAngleStartSlow - (_turnAngle - -_gyro->GetAngle()))/kAngleStartSlow + _turnSpeed);
#else
	return .4;//kTurnSlow*(-.7*(kAngleStartSlow - (_turnAngle - (_gyro.GetAngle())))/kAngleStartSlow + _turnSpeed);
#endif
}

void DriveMeyer::setLeftMotors(float speed) {
#ifndef rb2017
	_leftMotors->Set(speed);
#else
	leftMotor->Set(-speed);
	leftMini->Set(-speed);
#endif
}

void DriveMeyer::setRightMotors(float speed) {
#ifndef rb2017
	_rightMotors->Set(speed);
#else
	rightMotor->Set(speed);
	rightMini->Set(speed);
#endif
}

float DriveMeyer::getCounterAverage(){
	if(!_leftCounterBroken && !_rightCounterBroken)
	{
		if(getLeftCounter() < 0 && getRightCounter() < 0) {
				return ((getLeftCounter() - getRightCounter())/2);
			} else {
				return ((getLeftCounter() + getRightCounter())/2);
			}
	}
	else if(_leftCounterBroken && !_rightCounterBroken) {
		return getLeftCounter();
	}
	else if(!_leftCounterBroken && _rightCounterBroken) {
		return getRightCounter();
	}
	else {
		return 0;
	}
}

float DriveMeyer::distancePastRampZone(float distanceGone) {
	return distanceGone - (_distance - kStartRampingDistance);
}


float DriveMeyer::getLeftSpeed() {
	return (_leftCounter->GetSpeed()/kMaxMotorSpeed)*100;
}

float DriveMeyer::getRightSpeed() {
	return (_rightCounter->GetSpeed()/kMaxMotorSpeed)*100;
}

bool DriveMeyer::isAutoStraightFinished(){
	return _autoStraightFinished;
}

bool DriveMeyer::isAutoTurnFinished(){
	return _autoTurnFinished;
}

bool DriveMeyer::isAutoVisionGoToFinished(){
	return _visionTrackFinished;
}

void DriveMeyer::Debug(Feedback *feedback){
	feedback->send_Debug_String("Drive", "broken leftCounter", _leftCounterBroken ? "true" : "false");
	feedback->send_Debug_String("Drive", "broken rightCounter", _rightCounterBroken ? "true" : "false");
	feedback->send_Debug_Double("Drive", "Speed Left", _leftMotorSpeed);
	feedback->send_Debug_Double("Drive", "Speed Right", _rightMotorSpeed);
	feedback->send_Debug_Double("Drive", "CounterLeft", getLeftCounter());
	feedback->send_Debug_Double("Drive", "CounterRight", getRightCounter());
	feedback->send_Debug_Double("Drive", "CounterRawLeft", _leftCounter->Get());
	feedback->send_Debug_Double("Drive", "CounterRawRight", _rightCounter->Get());
	feedback->send_Debug_Double("Drive", "CounterSpeed_Left", getLeftSpeed());
	feedback->send_Debug_Double("Drive", "CounterSpeed_Right", getRightSpeed());
	feedback->send_Debug_Double("Drive", "vision x offset", _vision->getXPosition());
	feedback->send_Debug_String("Drive", "vision targets found?", _vision->targetsFound()? "true":"false");
#ifndef OLDGYRO
	feedback->send_Debug_Double("Drive", "gyro", _gyro->GetAngle());
#else
	feedback->send_Debug_Double("Drive", "gyro", -_gyro.GetAngle());
#endif
	feedback->send_Debug_String("Drive", "straight finished?", isAutoStraightFinished() ? "True":"false");
	feedback->send_Debug_String("Drive", "turn finished?", isAutoTurnFinished() ? "True":"false");
	feedback->send_Debug_Double("Drive", "turn angle", _turnAngle);
	feedback->send_Debug_Double("Drive", "auto distance", _distance);
	feedback->send_Debug_String("Drive", "vision tracking done?", _visionTrackFinished? "True":"False");
	feedback->send_Debug_Double("Drive", "vision area", _vision->getArea());
	feedback->send_Debug_Double("Drive", "vision wanted area", _wantedTargetArea);
	switch(_state){
	case TELEOP:
		feedback->send_Debug_String("Drive", "state", "Teleop");
		break;
	case AUTO_STRAIGHT:
		feedback->send_Debug_String("Drive", "state", "Auto Straight");
		break;
	case AUTO_TURN:
		feedback->send_Debug_String("Drive", "state", "Auto Turn");
		break;
	case AUTO_ARC_TURN:
		feedback->send_Debug_String("Drive", "state", "Auto Arc Turn");
		break;
	case AUTO_VISION_TURN:
		feedback->send_Debug_String("Drive", "state", "vision turn");
		break;
	case AUTO_VISION_GO_TO:
		feedback->send_Debug_String("Drive", "state", "vision go to");
		break;
	case DISABLED:
		feedback->send_Debug_String("Drive", "state", "Disabled");
		break;
	}
	switch(_visionDirection){
		case TURNLEFT:
			feedback->send_Debug_String("Drive", "vision turn direction", "left");
			break;
		case TURNRIGHT:
			feedback->send_Debug_String("Drive", "vision turn direction", "right");
			break;
		case NOTURN:
			feedback->send_Debug_String("Drive", "vision turn direction", "no turn");
			break;
	}

	if(!_gyroCalibrated){
		if (_gyroAnnoyanceTimer.Get() > kAnnoyanceSec) {
			SmartDashboard::PutNumber("thunderdashboard_gyro", _annoyanceVal ? 1 : 0);
			_annoyanceVal = !_annoyanceVal;
			_gyroAnnoyanceTimer.Reset();
		}
	}
}
