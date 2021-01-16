/*
 * CubeSystem.h
 *
 *  Created on: Jan 24, 2018
 *      Author: Robotics
 */

#ifndef SRC_CUBESYSTEM_H_
#define SRC_CUBESYSTEM_H_

#include "WPILib.h"
#include "Arm.h"
#include "Intake.h"
#include "Feedback.h"


class CubeSystem {
public:
	CubeSystem(Arm *arm, Intake *intake);
	/*
	 * Where stuff happens
	 */
	void Process();

	enum cubeOperation {
		STOP,
		OUT,
		IN
	};
	enum intakePosition {
		RETRACT,
		EXTEND
	};
	/*
	 * Set what were doing with a cube, either out, in, or stop
	 */
	void setCubeOperation(cubeOperation operation);
	/*
	 * Set where the arm is, either front, back, or switch on either side of robot
	 * moves intake accordingly, if arm is in front it is down, if arm is anywhere else its up
	 */
	void setArmPosition(Arm::armPosition position);
	/*
 	 * sets the arm pot to broken
	 */
	void setCubeBroken(bool borken);
	/*
	 * Sets the speed the arm moves at
	 * forwards is positive, back is negative
	 */
	void moveArmManually(float speed);
	/*
	 * ONLY NEEDED WHEN ARM IS BROKEN
	 * if arm pot is broken then move intake based on this
	 * need to figure out a button
	 */
	void setIntakePosition(intakePosition position);
	/*
	 * returns if the arm at some point reached the inteded destination
	 */
	bool wasArmAtPosition();
	/*
	 * sets cube present sensor broken
	 */
	void setBeamBreakBroken(bool broken);
	/*
	 * sets front limit broken
	 */
	void setFrontLimitBroken(bool broken);
	/*
	 * sets back limit broken
	 */
	void setBackLimitBroken(bool broken);
	/*
	 * sets the intake deployed sensor to broken
	 */
	void setIntakeSensorBroken(bool broken);

	void debug(Feedback *feedback);

	void reset();

	void armCubeInterfere();

	bool isCubePresent();

	void reverseLeft(bool left);
	void reverseRight(bool right);

	void handPressureOff(bool off);

	void inAuto(bool inAuto);

private:
	Arm::armPosition _armPosition;
	cubeOperation _cubeOperation;
	intakePosition _intakePosition;
	bool _armBroken;
	float _armSpeed;
	bool _beamBroken;
	bool canRunIntake();
	bool _inAuto;

	Intake *_intake;
	Arm *_arm;
};

#endif /* SRC_CUBESYSTEM_H_ */

