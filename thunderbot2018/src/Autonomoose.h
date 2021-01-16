#ifndef SRC_AUTONOMOOSE_H_
#define SRC_AUTONOMOOSE_H_

#include "DriveMeyer.h"
#include "CubeSystem.h"
#include "Feedback.h"
#include <iostream>

class Autonomoose {
public:

	enum AutoMode {
		kDoNothing,
		kCrossAutoLine,
		kSwitchLeft,
		kSwitchCenter,
		kSwitchRightIn,
		kSwitchRightOut,
		kNullZone,
		kSwitchToExchange,
		kSwitchTwice,
		kExchange,
		kVisionFollowPrism
	};

	enum startPos {
		kLeft,
		kCenter,
		kRightIn,
		kRightOut
	};

	AutoMode currentAutoMode;

	void sendModesToDashboard();

	void setAutoMode(AutoMode mode);

	DriveMeyer *_drive;
	CubeSystem *_cubeSystem;

	Autonomoose(DriveMeyer *drive, CubeSystem *cubeSystem);

	void Debug(Feedback *feedback);

	void process();
private:
	int _currentStep;

	void dont();
	void doNothing();
	void crossAutoLine();
	void goToSwitch();
	void nullZone();
	void switchToExchange();
	void exchange();
	void switchAgain();
	void visionFollowPrism();
	bool _blueAlliance;
	startPos _startPos;
	float _waitTime;

	Timer _timer;

	std::string _fmsData;
};

#endif /* SRC_AUTONOMOOSE_H_ */
