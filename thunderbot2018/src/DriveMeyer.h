#ifndef SRC_DRIVEMEYER_H_
#define SRC_DRIVEMEYER_H_

#define OLDGYRO

#include "WPILib.h"
#include "IOMap.h"
#include "Vision.h"
#include "Feedback.h"
#include <Counter.h>
#include <ADXRS450_Gyro.h>
#include "DriveCounter.h"

class DriveMeyer {
public:
	DriveMeyer(Vision *vision);

	/*
	 * sets the motor speed of each side (parameters are ranged from -1 to 1,
	 * anything negative meaning reverse), cancels any current driving instructions
	 */
	void drive(float leftSpeed, float rightSpeed);
	/*
	 * sets the motor's max speed ranged from -1 to 1, negative meaning reverse
	 * distance is measured in inches
	 * giving a negative distance will not change the direction. speed will, however.
	 * also cancels any current driving instructions
	 * ramps speed up to maxSpeed if possible
	 *
	 */
	void autoDriveStraight(float distance, float maxSpeed);
	/*
	 * returns true if drive straight is finished
	 */
	bool isAutoStraightFinished();
	/*
	 * turns the robot specified number of degrees, positive is to the right and negative is to the left
	 * also cancels any current driving instructions
	 */
	void autoTurn(float angle);
	/*
	 * turns robot specified speeds to a specific angle.
	 * sign of speeds determines turn direction
	 */
	void autoArcTurn(float leftSpeed, float rightSpeed, float angle);
	/*
	 * turns the robot the center of the blob found is at some spot on the image (-1 to 1)
	 * angle is the approximate angle it will turn, positive is to the right, negative is to the left
	 */
	void autoVisionTurn(float offset, float angle);
	/*
	 * tracks a target and goes to it
	 * stops when target area in camera is greater than or equal to what we want
	 * the startTurnLeft is used if we dont initially see a target
	 */
	void autoVisionGoTo(bool startTurnLeft, double area, Vision::VisionTarget visionTarget, bool keepGoing);
	/*
	 * returns true if we have reached the target we are tracking
	 */
	bool isAutoVisionGoToFinished();
	/*
	 * returns true if the turn (normal or vision) is finished
	 */
	bool isAutoTurnFinished();
	/*
	 * stops any current driving action
	 * resets all speeds and encoder values to zero
	 */
	void reset();
	/*
	 * acts upon any actions in progress
	 */
	void process();
	/*
	 * calibrates the gyro if not already calibrated
	 * must be called when the robot is S T I L L
	 */
	void calibrateGyro();
	/*
	 * if true, any auto action involving usage of encoder will not happen
	 * any auto action involving the usage of encoder will rely on only one side
	 * if both encoders are broken, action will be terminated
	 */
	void setLeftCounterBroken(bool broken);
	/*
	 * if true, any auto action involving usage of encoder will not happen
	 * any auto action involving the usage of encoder will rely on only one side
	 * if both encoders are broken, action will be terminated
	 */
	void setRightCounterBroken(bool broken);
	/*
	 * Sends dashboard debug values
	 */
	void Debug(Feedback *feedback);
private:
	float getLeftSpeed();
	float getRightSpeed();

	enum driveState {
			TELEOP,
			AUTO_STRAIGHT,
			AUTO_TURN,
			AUTO_ARC_TURN,
			AUTO_VISION_TURN,
			AUTO_VISION_GO_TO,
			DISABLED
	};

	enum visionDirection{
		TURNLEFT,
		TURNRIGHT,
		NOTURN
	};

	Vision *_vision;
	driveState _state;
	visionDirection _visionDirection;

#ifdef rb2017
	SpeedController* leftMini;
	SpeedController* leftMotor;
	SpeedController* rightMini;
	SpeedController* rightMotor;
#else
	Spark* _leftMotors;
	Spark* _rightMotors;
#endif

	DriveCounter* _leftCounter;
	DriveCounter* _rightCounter;
#ifndef OLDGYRO
	ADXRS450_Gyro* _gyro;
#else
	AnalogGyro _gyro;
#endif
	bool _gyroCalibrated;

	bool _leftCounterBroken;
	bool _rightCounterBroken;

	float _leftMotorSpeed;
	float _rightMotorSpeed;

	float _leftFinalSpeed;
	float _rightFinalSpeed;

	float _autoMaxSpeed;

	float _visionTurnOffset;
	float _turnAngle;

	bool _autoTurnFinished;
	bool _autoStraightFinished;

	float getLeftCounter(); //figure this out on your own
	float getRightCounter(); // ^^^

	float rampAutoTurnSpeed();

	float getCounterAverage();

	float _distance;
	float _maxSpeed;
	float _accumulatedCounterDifference;

	float distancePastRampZone(float distanceGone);

	void setLeftMotors(float speed);
	void setRightMotors(float speed);

	float _turnSpeed;

	Timer _gyroAnnoyanceTimer;
	bool _annoyanceVal;
	bool _visionTrackFinished;
	double _wantedTargetArea;
	bool _guessTurnLeft;
	Timer _visionTurnTimer;
	bool _visionKeepGoing;
	Timer _autoDoneTimer;

};

#endif /* SRC_DRIVEMEYER_H_ */
//in honor of austin meyer (2014-2017)
