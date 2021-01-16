/*
 * Ramp.h
 *
 *  Created on: Jan 17, 2018
 *      Author: Robotics
 */
/*
#ifndef SRC_RAMP_H_
#define SRC_RAMP_H_

#define NORAMPS

//#define NEW_RAMP

#include "WPILib.h"
#include "Feedback.h"
#include "StallCheck.h"
#include "ctre/Phoenix.h"

class Ramp {
public:
	Ramp(int nearCanId, int farCanId);

	enum RampDirection {
		STOPPED,
		UP,
		DOWN
	};


	void setDirection(RampDirection direction);
	void process();
	void setBroken(bool broken);
	void setDeployed(bool deployed);
	void reset();

	int nearPos();
	int farPos();
	float nearSpeed();
	float farSpeed();

private:
#ifndef NORAMPS
	TalonSRX _motorNear;
	TalonSRX _motorFar;
#endif

	float nearActualSpeed();
	float farActualSpeed();

	float _nearSpeed;
	float _farSpeed;
	float _setSpeed;

	bool _isDeployed;
	bool _isEncBroken;

	RampDirection _direction;

	StallCheck _nearStallCheck;
	StallCheck _farStallCheck;

	void calcCmdSpeed();
};
*/
//#endif /* SRC_RAMP_H_ /*
