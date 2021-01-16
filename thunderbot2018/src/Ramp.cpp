/*#include "Ramp.h"
#include "IOMap.h"

const float kRampEncFinal = 184320;   // 45 * 4096 = 184320
const float kRampMaxDiff = 36864; // 0.2 * 184320, max diff in ramps before shutting down (danger)
const float kRampSpeed = 0.1; // motor command
const int kRampAdjDiff = 50; // maximum absolute difference in encoders before taking corrective action
const float kRampDiffGain = 0.002; // amount we correct proportional to the encoder differences

// for NEW_RAMP algorithm
const float kSpeedConversion = 0.00244; // 10/4069 convert from units/msec to rev/sec
const float kRampSpeedGain = 0.01; // P gain for ramp speed
const float kRampTargetSpeed = 15; // 45 revs in 3 seconds
const float kRampPosDiffGain = 0.01 / 4096.0; // P gain for ramp position diff

// stall parameters
float kStallThreshold = 80;     // max average current before stalling, amps
unsigned int kStallSamples = 5; // number of samples to average for current measurement

float clamp(float val, float min, float max){
	if(val < min){
		return min;
	}
	if(val > max){
		return max;
	}
	return val;
}

Ramp::Ramp(int nearCanId, int farCanId):
#ifndef NORAMPS
	_motorNear(nearCanId),
	_motorFar(farCanId),
#endif
	_nearSpeed(0),
	_farSpeed(0),
	_setSpeed(kRampTargetSpeed),
	_isDeployed(false),
	_isEncBroken(false),
	_direction(Ramp::STOPPED),
	_nearStallCheck(kStallThreshold, kStallSamples),
	_farStallCheck(kStallThreshold, kStallSamples)
{
#ifndef NORAMPS
	_motorNear.ConfigSelectedFeedbackSensor(FeedbackDevice::QuadEncoder, 0, 0);
	_motorFar.ConfigSelectedFeedbackSensor(FeedbackDevice::QuadEncoder, 0, 0);

	_motorNear.SetNeutralMode(NeutralMode::Brake);
	_motorFar.SetNeutralMode(NeutralMode::Brake);
#endif
}


void Ramp::setDirection(Ramp::RampDirection direction) {
	_direction = direction;
}

void Ramp::setDeployed(bool deployed){
	_isDeployed = deployed;
}

void Ramp::reset() {
	_nearSpeed = 0;
	_farSpeed = 0;
#ifndef	NORAMPS
	_motorNear.Set(ControlMode::PercentOutput, 0);
	_motorFar.Set(ControlMode::PercentOutput, 0);
	_motorNear.SetSelectedSensorPosition(0, 0, 10);
	_motorFar.SetSelectedSensorPosition(0, 0, 10);
#endif
}

void Ramp::calcCmdSpeed() {
#ifndef NORAMPS
	int nearRampPos;
	int farRampPos;
	int rampDiff;

	float newNearSpeed;
	float newFarSpeed;
	float correction;
	nearRampPos = _motorNear.GetSelectedSensorPosition(0);
	farRampPos = _motorFar.GetSelectedSensorPosition(0);
	rampDiff = nearRampPos - farRampPos;


	// the default action
	newNearSpeed = 0;
	newFarSpeed = 0;
	correction = 0;

	// if the ramp is not deployed, don't do anything with the ramps
	if(_isDeployed)
	{
		if(_isEncBroken){
			// if the encoders are broken, just go up or down at the specified speed
			if(_direction == Ramp::UP) {
				newNearSpeed = kRampSpeed;
				newFarSpeed = kRampSpeed;
			}
			else if(_direction == Ramp::DOWN) {
				newNearSpeed = -kRampSpeed;
				newFarSpeed = -kRampSpeed;
			}
		} else {
			if(_direction == Ramp::UP)
			{
#ifdef NEW_RAMP
				correction = -kRampPosDiffGain * rampDiff;
				newNearSpeed = _nearSpeed - kRampSpeedGain * (nearActualSpeed() - _setSpeed) + correction;
				newFarSpeed = _farSpeed - kRampSpeedGain * (farActualSpeed() - _setSpeed) - correction;

#else
				if(abs(rampDiff) > kRampAdjDiff){
					// if the difference of the ramps is greater than the threshold, adjust it proportional to the diff
					// if rampDiff > 0, then nearRamp > farRamp:
					//    spin the far motor faster and the near motor more slowly
					correction = kRampDiffGain * rampDiff;
					newNearSpeed = kRampSpeed - correction;
					newFarSpeed = kRampSpeed + correction;
				} else {
					// if the difference is not greater than the threshold, drive both motors at the desired speed
					newNearSpeed = kRampSpeed;
					newFarSpeed = kRampSpeed;
				}
#endif

				// stop ramps if they are at the desired position
				if(nearRampPos >= kRampEncFinal){
					newNearSpeed = 0;
				}
				if(farRampPos >= kRampEncFinal){
					newFarSpeed = 0;
				}
			}
			else if(_direction == Ramp::DOWN)
			{
				// TODO - currently just going down at constant speed, not checking encoders
				newNearSpeed = -kRampSpeed;
				newFarSpeed = -kRampSpeed;
			}

			// stop the ramps if the difference is too large - means one is
			// probably dead
			if(abs(rampDiff) > kRampMaxDiff){
				newNearSpeed = 0;
				newFarSpeed = 0;
			}
		}
	}

	// actually assign the new speeds
	_nearSpeed = clamp(newNearSpeed, -1.0, 1.0);
	_farSpeed = clamp(newFarSpeed, -1.0, 1.0);
#endif
}

void Ramp::process() {
	calcCmdSpeed();
#ifndef NORAMPS

	_nearStallCheck.feedCurrentMeasurement(_motorNear.GetOutputCurrent());
	_farStallCheck.feedCurrentMeasurement(_motorFar.GetOutputCurrent());

	// check for stall
	if(_nearStallCheck.shouldStall()){
		printf("NEAR RAMP MOTOR STALL DETECTED\n");
		_nearSpeed = 0;
	}
	if(_farStallCheck.shouldStall()){
		printf("FAR RAMP MOTOR STALL DETECTED\n");
		_farSpeed = 0;
	}

	_motorNear.Set(ControlMode::PercentOutput, _nearSpeed);
	_motorFar.Set(ControlMode::PercentOutput, _farSpeed);
#endif
}

int Ramp::nearPos(){
#ifndef NORAMPS
	return _motorNear.GetSelectedSensorPosition(0);
#else
	return true;
#endif
}

int Ramp::farPos(){
#ifndef NORAMPS
	return _motorFar.GetSelectedSensorPosition(0);
#else
	return true;
#endif
}

float Ramp::nearActualSpeed(){
#ifndef NORAMPS
	return kSpeedConversion * _motorNear.GetSelectedSensorVelocity(0);
#else
	return 0;
#endif
}

float Ramp::farActualSpeed(){
#ifndef NORAMPS
	return kSpeedConversion * _motorFar.GetSelectedSensorVelocity(0);
#else
	return 0;
#endif
}

float Ramp::nearSpeed(){
	return _nearSpeed;
}

float Ramp::farSpeed(){
	return _farSpeed;
}

void Ramp::setBroken(bool broken) {
	_isEncBroken = broken;
}
*/
