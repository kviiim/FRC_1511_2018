/*
 * Intake.h

 *
 *  Created on: Jan 17, 2018
 *      Author: Robotics
 */

#ifndef SRC_INTAKE_H_
#define SRC_INTAKE_H_

#include "WPILib.h"
#include "Feedback.h"
#include "ctre/Phoenix.h"

class Intake {
public:
	Intake();
	enum intakeDirection {
		OFF,
		OUT,
		IN
	};

	enum intakePosition {
		EXTEND,
		RETRACT
	};
	/*
	 * Sets the direction the intake is spinning, either out, in, or off
	 */
	void setDirection(intakeDirection direction);
	/*
	 * Sets where the intake is, either extend or retract
	 */
	void setPosition(intakePosition position);
	/*
	 * Sets all motor speeds to zero
	 */
	void reset();
	/*
	 *acts upon any actions in progress
	 */
	void process();
	/*
	 * returns true if intake is extended
	 */
	bool isIntakeExtended();
	/*
	 * sends debug values to dashboard
	 */
	void debug(Feedback *feedback);
	/*
	 * sets intake sensor broken
	 */
	void setIntakeSensorBroken(bool broken);
	/*
	 * returns if intake sensor is broken
	 */
	bool isIntakeSensorBroken();
	/*
	 * toggles reverse direction of left wheel
	 */
	bool _reverseLeft;
	/*
	 * toggles reverse direction of right wheel
	 */
	bool _reverseRight;
private:
	Timer _intakeTimer;
	intakePosition _intakePosition;
	void setMotorSpeed(double speed);
	intakeDirection _intakeDirection;
	TalonSRX intakeMotorLeft;
	TalonSRX intakeMotorRight;
	DigitalInput intakeDeployed;
	DoubleSolenoid intakePnu;
	bool _intakeSensorBroken;


};

#endif /* SRC_INTAKE_H_ */
