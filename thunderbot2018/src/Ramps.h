/*

#include "WPILib.h"
#include "Feedback.h"
#include "Ramp.h"
#include "ctre/Phoenix.h"

class Ramps {
public:
	Ramps();

	void releaseRamps();
	void setLeftRamp(Ramp::RampDirection direction);
	void setRightRamp(Ramp::RampDirection direction);
	void process();
	void setBroken(bool leftEncBroken, bool rightEncBroken);
	void debug(Feedback *feedback);
	void reset();

private:
	Ramp _leftRamp;
	Ramp _rightRamp;

	DoubleSolenoid _rampDeploy;

	bool _rampsDeployed;
};*/


