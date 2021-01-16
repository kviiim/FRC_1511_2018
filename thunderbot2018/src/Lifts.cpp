#include "Lifts.h"

const float kEncoderMax = 60000;
const float kEncoderMid = .99;
const float kEncoderMidEnd =  1;
const float kStartSpeed = 1;
const float kEndSpeed = .2;
const float kReverseSpeed = -.3;

Lifts::Lifts():
		leftLift(CAN_OUT_LIFT_LEFT),
		rightLift(CAN_OUT_LIFT_RIGHT),
		leftLimit(DIGITAL_IN_LIFT_LEFT_LIMIT),
		rightLimit(DIGITAL_IN_LIFT_RIGHT_LIMIT),
		_liftDeploy(PCM_OUT_RAMP_HOLD, PCM_OUT_RAMP_DEPLOY),
		_liftsDeployed(false),
		_leftLifting(false),
		_rightLifting(false),
		_leftLiftDone(false),
		_rightLiftDone(false),
		_liftReverse(false)

{
	leftLift.ConfigSelectedFeedbackSensor(FeedbackDevice::QuadEncoder, 0, 0);
	rightLift.ConfigSelectedFeedbackSensor(FeedbackDevice::QuadEncoder, 0, 0);
}

void Lifts::process(){
	if(_liftsDeployed){
		if(!_liftReverse){	//if were going forwards which should be most of the time
			if(_leftLifting && !atLeftUpperLimit() && !_leftLiftDone){		//if not done lifting
				setLeftSpeed(calculateLiftSpeed(true));		//go up at speed based on where we are
			}
			else if(_leftLifting){
				_leftLiftDone = true;	//when done then just stop
				setLeftSpeed(0);
			}
			else {
				setLeftSpeed(0);	//sets to 0 when not moving just in case
			}
			if(_rightLifting && !atRightUpperLimit() && !_rightLiftDone){
				setRightSpeed(calculateLiftSpeed(false));
			}
			else if (_rightLifting){
				_rightLiftDone = true;
				setRightSpeed(0);

			}
			else {
				setRightSpeed(0);
			}
		}
		else {
			if(_leftLifting){	//otherwise if we are putting lifts down
				setLeftSpeed(kReverseSpeed);	//then go at a slow speed
			}
			else {
				setLeftSpeed(0);
			}
			if(_rightLifting){
				setRightSpeed(kReverseSpeed);
			}
			else {
				setRightSpeed(0);
			}
		}
	}
}

void Lifts::reset(){
	setLeftSpeed(0);
	setRightSpeed(0);
	leftLift.SetSelectedSensorPosition(0, 0, 0);
	rightLift.SetSelectedSensorPosition(0, 0, 0);
	_liftsDeployed = false;
	_leftLiftDone = false;
	_rightLiftDone = false;
	_leftLifting = false;
	_rightLifting = false;
	_liftDeploy.Set(DoubleSolenoid::kForward);
}

void Lifts::resetWhenDeploy(){
	setLeftSpeed(0);
	setRightSpeed(0);
	leftLift.SetSelectedSensorPosition(0, 0, 0);
	rightLift.SetSelectedSensorPosition(0, 0, 0);
	_leftLiftDone = false;
	_rightLiftDone = false;
}

void Lifts::debug(Feedback *feedback){
	feedback->send_Debug_String("Lifts", "lifts deployed?", _liftsDeployed ? "True":"False");
	feedback->send_Debug_String("Lifts", "left limit?", atLeftUpperLimit() ? "True":"False");
	feedback->send_Debug_String("Lifts", "right limit?", atRightUpperLimit() ? "True":"False");
	feedback->send_Debug_Double("Lifts", "left encoder value", abs(leftLift.GetSelectedSensorPosition(0)));
	feedback->send_Debug_Double("Lifts", "right encoder value", abs(rightLift.GetSelectedSensorPosition(0)));
	feedback->send_Debug_String("Lifts", "right lifting?", _rightLifting ? "True":"False");
	feedback->send_Debug_String("Lifts", "left lifting?", _leftLifting ? "True":"False");
	feedback->send_Debug_String("Lifts", "left done", _leftLiftDone ? "True":"False");
	feedback->send_Debug_String("Lifts", "right done", _rightLiftDone ? "True":"False");
	feedback->send_Debug_String("Lifts", "REVESER", _liftReverse ? "True":"False");
	feedback->send_Debug_Double("Lifts", "left speed", calculateLiftSpeed(true));


}

void Lifts::raiseLeftLift(bool raise){
	if(_liftsDeployed){
		if(raise){
			if(_rightLifting != true){
				_leftLiftDone = false;
			}
			_leftLifting = true;
		}
		else{
			_leftLifting = false;
		}
	}
}

void Lifts::raiseRightLift(bool raise){
	if(_liftsDeployed){
		if(raise){
			if(_rightLifting != true){
				_rightLiftDone = false;
			}
			_rightLifting = true;
		}
		else{
			_rightLifting = false;
		}
	}
}

bool Lifts::atLeftUpperLimit(){
	return leftLimit.Get();
}

bool Lifts::atRightUpperLimit(){
	return rightLimit.Get();
}

void Lifts::setLeftSpeed(float speed){
	leftLift.Set(ControlMode::PercentOutput, speed);
}

void Lifts::setRightSpeed(float speed){
	rightLift.Set(ControlMode::PercentOutput, speed);
}

void Lifts::deployLifts(){
	resetWhenDeploy();
	_liftDeploy.Set(DoubleSolenoid::kReverse);
	_liftsDeployed = true;
}

float Lifts::getEncoderPercent(bool left){
	if(left){
		return abs(leftLift.GetSelectedSensorPosition(0))/kEncoderMax;
	}
	else{
		return abs(rightLift.GetSelectedSensorPosition(0))/kEncoderMax;

	}
}

float Lifts::getEncoderVal(bool left){
	if(left){
		return abs(leftLift.GetSelectedSensorPosition(0));
	}
	else{
		return abs(rightLift.GetSelectedSensorPosition(0));
	}
}

float Lifts::calculateLiftSpeed(bool left){
	//THIS MIGHT NEED TO BE CHANGED, CURRENTLY JUST STARTS FULL SPEED AND THEN DECREASES
	if(getEncoderPercent(left) < kEncoderMid){
		return kStartSpeed;
	}
	else if (getEncoderPercent(left) < kEncoderMidEnd){
		return ((-(kStartSpeed - kEndSpeed) * (getEncoderPercent(left)) * kEncoderMid) + kStartSpeed)/(kEncoderMidEnd-kEncoderMid);
	}
	else{
		return kEndSpeed;
	}
}

void Lifts::reverseLifts(bool reverse){
	_liftReverse = reverse;
}

