/*
 * Arm.h

 *
 *  Created on: Jan 17, 2018
 *      Author: Robotics
 */
#define STALLIDXNUM 15
#ifndef SRC_ARM_H_
#define SRC_ARM_H_
#include "WPILib.h"
#include"IOMap.h"
#include "ctre/Phoenix.h"
#include "Feedback.h"


class Arm {
public:
	Arm();

	enum armPosition {
		FRONT,
		SWITCHFRONT
#ifndef ARMNOWORK
		,
		BACK,
		SWITCHBACK,
		REST,
		SECONDLEVEL
#endif
	};
	enum armState {
		INTAKE,
		OUTTAKE,
		STOP
	};

	typedef enum {STALL, NOTSTALL} Stall;
	/*
	 * acts upon any actions in progress
	 */
	void process();
	/*
	 * stops any action in process, arm position remains in same place
	 */
	void reset();
	/*
	 * Rotates the arm to one of four specified location (pointing forwards, at an angle for the switch, straight up, or backwards)
	 */
	void setArmPosition(armPosition position);
	/*
	 * Sets the intake on the arm to bring cube in, stop, or spit it out
	 */
	void setArmState(armState state);
	/*
	 * moves arm based on an input speed, not potentionmeter. brake needs to be set manually
	 * positive moves arm forwards, negative back
	 */
	void setArmSpeed(float speed);
	/*
	 * Sets brake, true to lock, false to unlock
	 */
	void setBrake(bool lock);
	/*
	 * returns true if the arm is in the front of the robot
	 */
	bool isArmInFront();
	/*
	 * returns true if a cube is in the hand
	 */
	bool isCubePresent();
	/*
	 * returns true if cube detection sensor is set to broken
	 */
	bool isCubeSensorBroken();
	/*
	 * sets the arm to broken mode or not
	 */
	void setArmBroken(bool broken);
	/*
	 * sets cube sensor to broken
	 */
	void setCubeSensorBroken(bool broken);
	/*
	 * Sends debug values to dashboard
	 */
	void Debug(Feedback *feedback);
	/*
	 * sets front arm limit switch's broken mode
	 */
	void setFrontLimitBroken(bool broken);
	/*
	 * sets back arm limit switch's broken mode
	 */
	void setBackLimitBroken(bool broken);
	/*
	 * returns if arm has is at selected preset position
	 */
	bool isArmAtPosition();
	/*
	 * returns if arm is being used in manual mode
	 */
	bool _manualMode;
	/*
	 * sets if intake will interfere with arm
	 */
	void armIntake(bool interfere);
	/*
	 * sets whether or not to make hand apply pressure
	 */
	void setHandPnu(bool clamp);
	/*
	 * returns scaled value of arm position for dashboard arm meter
	 */
	double getDashboardArmPos();
	/*
	 * runs arm at full speed (ideally from one side to the other)
	 */
	void armFullMotion(bool full);
	/*
	 * returns if arm is set to run at full speed
	 */
	bool isFullMotion();
	bool isCubePresentSensor();
	void turnHandPressureOff(bool off);

private:
	armPosition _armPosition;
	armState _armState;

	TalonSRX arm;
	TalonSRX intake;

	AnalogInput potentiometer;
	DigitalInput cubeSensor;

	bool isFrontLimitFlipped();
	bool isBackLimitFlipped();
	void goToPosition(float maxVal, float minVal);
	float getPotValue();
	void setArmMotorSpeed(float speed);
	void setIntakeSpeed(float speed);
	float calculateArmSpeed(float speed);
	void Stall_Zero();
	bool _potBroken;
	bool _cubeSensorBroken;
	float _armSpeed;
	bool _frontLimitBroken;
	bool _backLimitBroken;
	DoubleSolenoid _brakePnu;
	DoubleSolenoid _handPnu;
	DigitalInput _frontLimit;
	DigitalInput _backLimit;
	bool _isBrakeSet;
	bool _atPosition;
	bool canMoveArm();
	bool _hasBeenAtPosition;
	Stall _armStall;
	Timer _timer;
	int _stallIdx;
	float _currentValues[STALLIDXNUM];
	bool _armIntakeInterfere;
	bool _haveHadCube;
	int _brakeCounter;
	bool _armFullMotion;
	bool _hadCubeLostIt;
	float _outtakeSpeed;
	bool _handPressureOff;
};

#endif /* SRC_ARM_H_ */
