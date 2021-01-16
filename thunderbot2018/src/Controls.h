#ifndef SRC_CONTROLS_H_
#define SRC_CONTROLS_H_

#include "Lifts.h"
#include "WPILib.h"
#include "DriveMeyer.h"
#include "Arm.h"
#include "Intake.h"
#include "ControlsButton.h"
#include "Feedback.h"
#include "CubeSystem.h"


class Controls {
	public:
		Controls(CubeSystem *cubeSystem, DriveMeyer *driveMeyer, Lifts *lifts, Feedback *feedback);
		virtual ~Controls();

		typedef enum {TELE_OP, AUTO, DISABLED} RobotMode;

		/*
		 * gets controller input and acts upon it for given mode
		 * //(where the magic happens)
		 */
		void process();
		/*
		 * send feedback values to dashboard
		 */
		void Debug(Feedback *feedback);
	private:
		void ProcessControllerDriver();
		void ProcessControllerAux();
		void ProcessBroken();
		float GetPower(float power, bool slow, bool turbo);

		//Pointers
		CubeSystem *_cubeSystem;
		DriveMeyer *_drive;
		Lifts *_lifts;
		Feedback *_feedback;

		//Joysticks for Drivers
		Joystick *_driverJoystick;
		Joystick *_auxJoystick;
		Joystick *_brokenJoystick;

		//Broken Stuff
		ControlsButton *_brokenArmIntakeMove; //no preset positions work, intake doesn't move with arm
		ControlsButton *_brokenArmLimitFront; //limit switch front broken
		ControlsButton *_brokenArmLimitBack; //limit switch back broken
		//ControlsButton *_brokenRampLiftLeft; //left ramp encoder broken
		//ControlsButton *_brokenRampLiftRight; //right ramp encoder broken
		ControlsButton *_brokenIntakeSensor;
		ControlsButton *_brokenCubeIntake; //intake beam break broken
		ControlsButton *_driverSwapDrive;
		ControlsButton *_reverseLifts;

		bool _swapDrive;
		bool _wasTriggerDown;
		float GetPosition(Joystick *joystick, int axis, bool fullrange = false);
		float GetPowerTurn(float power, bool slow, bool turbo);
};

#endif /* SRC_CONTROLS_H_ */



