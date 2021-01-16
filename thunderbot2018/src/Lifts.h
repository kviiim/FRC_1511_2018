//yo thats finna woke
#include "Feedback.h"
#include "WPILib.h"
#include"IOMap.h"
#include "ctre/Phoenix.h"

#ifndef LIFTS_H_
#define LIFTS_H_

class Lifts {
public:
	Lifts();
	/*
	 * deploys both ramps using pnuematics
	 */
	void deployLifts();
	/*
	 * lifts left ramp, stops when at upper limit/encoder
	 */
	void raiseLeftLift(bool raise);
	/*
	 * lifts right ramp, stops when at upper limit/encoder
	 */
	void raiseRightLift(bool raise);
	/*
	 * stops all motors
	 */
	void reset();
	/*
	 * sends debug of stuff
	 */
	void debug(Feedback *feedback);
	void process();
	void reverseLifts(bool reverse);

private:
	bool atLeftUpperLimit();
	bool atRightUpperLimit();
	void setLeftSpeed(float speed);
	void setRightSpeed(float speed);
	float getEncoderPercent(bool left);
	float calculateLiftSpeed(bool left);
	float getEncoderVal(bool left);
	void resetWhenDeploy();
	TalonSRX leftLift;
	TalonSRX rightLift;
	DigitalInput leftLimit;
	DigitalInput rightLimit;
	DoubleSolenoid _liftDeploy;

	bool _liftsDeployed;
	bool _leftLifting;
	bool _rightLifting;
	bool _leftLiftDone;
	bool _rightLiftDone;
	bool _liftReverse;

};

#endif /* LIFTS_H_ */

