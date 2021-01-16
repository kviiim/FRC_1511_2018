//rip ramperoni (2018-2018)
/*
#include <Ramps.h>
#include "IOMap.h"

Ramps::Ramps():
	_leftRamp(CAN_OUT_RAMP_LEFT_NEAR, CAN_OUT_RAMP_LEFT_FAR),
	_rightRamp(CAN_OUT_RAMP_RIGHT_NEAR, CAN_OUT_RAMP_RIGHT_FAR),
	_rampDeploy(PCM_OUT_RAMP_HOLD, PCM_OUT_RAMP_DEPLOY),
	_rampsDeployed(false)
{
	_rampDeploy.Set(DoubleSolenoid::kForward);
}

void Ramps::releaseRamps() {
	_rampDeploy.Set(DoubleSolenoid::kReverse);
	_rampsDeployed = true;
	_leftRamp.setDeployed(true);
	_rightRamp.setDeployed(true);
}

void Ramps::setLeftRamp(Ramp::RampDirection direction){
	_leftRamp.setDirection(direction);
}

void Ramps::setRightRamp(Ramp::RampDirection direction){
	_rightRamp.setDirection(direction);
}

void Ramps::reset() {
	_leftRamp.reset();
	_rightRamp.reset();
}

void Ramps::process() {
	_leftRamp.process();
	_rightRamp.process();
}

void Ramps::debug(Feedback *feedback) {
	feedback->send_Debug_Double("Ramp", "Ramps deployed", _rampsDeployed);
	feedback->send_Debug_Double("Ramp", "Encoder Left Near", _leftRamp.nearPos());
	feedback->send_Debug_Double("Ramp", "Encoder Left Far", _leftRamp.farPos());
	feedback->send_Debug_Double("Ramp", "Encoder Right Near", _rightRamp.nearPos());
	feedback->send_Debug_Double("Ramp", "Encoder Right Far", _rightRamp.farPos());
	feedback->send_Debug_Double("Ramp", "Speed Left Near", _leftRamp.nearSpeed());
	feedback->send_Debug_Double("Ramp", "Speed Left Far", _leftRamp.farSpeed());
	feedback->send_Debug_Double("Ramp", "Speed Right Near", _rightRamp.nearSpeed());
	feedback->send_Debug_Double("Ramp", "Speed Right Far", _rightRamp.nearSpeed());
}

void Ramps::setBroken(bool leftEncBroken, bool rightEncBroken) {
	_leftRamp.setBroken(leftEncBroken);
	_rightRamp.setBroken(rightEncBroken);
}
*/
