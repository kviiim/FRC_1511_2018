#include "WPILib.h"

#include "Arm.h"
#include "Autonomoose.h"
#include "Controls.h"
#include "DriveMeyer.h"
#include "Intake.h"
#include "Vision.h"
#include "Feedback.h"
#include "CubeSystem.h"
#include "Lifts.h"
#include "Lights.h"

class Robot : public frc::SampleRobot {
private:
	Feedback feedback;
	DriveMeyer driveMeyer;
	Arm arm;
	Lifts lifts;
	Intake intake;
	Autonomoose autonomoose;
	Controls controls;
	Vision vision;
	CubeSystem cubeSystem;
	Lights lights;

	const double WAIT_TIME = 0.05;

	/*
	 *Loop that calls Iteration
	 */
	static void visionProcess(Vision *vision) {
#ifndef NOVISION
		vision->Process();
#endif
	}

public:
	Robot():
		feedback(),
		driveMeyer(&vision),
		arm(),
		lifts(),
		intake(),
		autonomoose(&driveMeyer, &cubeSystem),
		controls( &cubeSystem, &driveMeyer, &lifts, &feedback),
		vision(),
		cubeSystem(&arm, &intake),
		lights()
	{

	}

	void RobotInit() {
#ifndef NOVISION
		std::thread visionThread(visionProcess, &vision);
		visionThread.detach();
#endif
		autonomoose.sendModesToDashboard();

	}
	void Disabled()
	{
		Reset();
		intake.reset();

		while (IsDisabled()) {
			//printf("Left Motor: %3.2f\% | Right Motor: %3.2f\%\n", driveMeyer.getLeftSpeed(), driveMeyer.getRightSpeed());
			controls.process();
			vision.inAuto(true);
			Debug(&feedback);
			ProcessLights();
			frc::Wait(WAIT_TIME);
		}
	}

	void Autonomous() {
		Reset();
		Autonomoose::AutoMode autoMode = (Autonomoose::AutoMode) SmartDashboard::GetNumber("Auto_Mode", 0); //gets auto mode index from dashboard, uses 0 is none is selected
		autonomoose.setAutoMode(autoMode); //sets auto mode in autonomous class, casts to AutoMode enumerator
		driveMeyer.calibrateGyro();
		vision.inAuto(true);
		while (IsAutonomous() && IsEnabled())
		{
			//printf("Left Motor: %3.2f\% | Right Motor: %3.2f\%\n", driveMeyer.getLeftSpeed(), driveMeyer.getRightSpeed());
			driveMeyer.process();
			autonomoose.process();
			arm.process();
			cubeSystem.Process();
			intake.process();

			Debug(&feedback);
			ProcessLights();
			frc::Wait(WAIT_TIME);
			cubeSystem.inAuto(true);

		}
	}

	void OperatorControl() override {
		cubeSystem.inAuto(false);
		Reset();
		driveMeyer.calibrateGyro();
		vision.inAuto(false);
		while (IsOperatorControl() && IsEnabled()) {
			controls.process();
			driveMeyer.process();
			arm.process();
			lifts.process();
			intake.process();
			cubeSystem.Process();
			Debug(&feedback);
			ProcessLights();
			//printf("Left Motor: %3.2f\% | Right Motor: %3.2f\%\n", driveMeyer.getLeftSpeed(), driveMeyer.getRightSpeed());
			frc::Wait(WAIT_TIME);
		}
	}
	void Test()
	{
		while (IsTest() && IsEnabled()) {
			Wait(WAIT_TIME);
		}
	}

	void Debug(Feedback *feedback) {
		intake.debug(feedback);
		arm.Debug(feedback);
		driveMeyer.Debug(feedback);
		lifts.debug(feedback);
		cubeSystem.debug(feedback);
		autonomoose.Debug(feedback);
		controls.Debug(feedback);
	}

	void Reset() {
		driveMeyer.reset();
		arm.reset();
		lifts.reset();
		intake.reset();
		cubeSystem.reset();
		intake.reset();
	}

	void ProcessLights() {
		lights.process(IsAutonomous(), IsEnabled());
	}
};

START_ROBOT_CLASS(Robot)
