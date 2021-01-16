#include "Autonomoose.h"
const float kTimeOuttake = 1.5;
const float kExchangeArea = 4;

Autonomoose::Autonomoose(DriveMeyer *drive, CubeSystem *cubeSystem):
currentAutoMode(kDoNothing),
_currentStep(0),
_blueAlliance(false),
_startPos(kCenter),
_waitTime(0),
_timer()
{
	_drive = drive;
	_cubeSystem = cubeSystem;
	_timer.Start();
	_fmsData = frc::DriverStation::GetInstance().GetGameSpecificMessage();

}

void Autonomoose::setAutoMode(AutoMode mode) {
	_timer.Reset();
	currentAutoMode = mode;
	_currentStep = 0;
}

void Autonomoose::process() {
	printf("currentStep %d \n", _currentStep);

	switch(currentAutoMode) {
	case kDoNothing:
		doNothing();
		break;
	case kCrossAutoLine:
		crossAutoLine();
		break;
	case kSwitchLeft:
		_startPos = kLeft;
		goToSwitch();
		break;
	case kSwitchCenter:
		_startPos = kCenter;
		goToSwitch();
		break;
	case kSwitchRightIn:
		_startPos = kRightIn;
		goToSwitch();
		break;
	case kSwitchRightOut:
		_startPos = kRightOut;
		goToSwitch();
		break;
	case kNullZone:
		nullZone();
		break;
	case kSwitchToExchange:
		switchToExchange();
		break;
	case kSwitchTwice:
		switchAgain();
		break;
	case kExchange:
		exchange();
		break;
	case kVisionFollowPrism:
		visionFollowPrism();
		break;
	}
}

void Autonomoose::dont() {
	//if(goingToCrashIntoEachOther) {
	//	dont();
	//}
}

void Autonomoose::sendModesToDashboard(){
	char buffer[256] = "";
	char key[32];

	sprintf(&buffer[strlen(buffer)],",%d", kDoNothing);
	sprintf(key,"thunderdashboard_auto_%d", kDoNothing);
	SmartDashboard::PutString(key, "does nothing what a surpise im surprised");

	sprintf(&buffer[strlen(buffer)],",%d", kCrossAutoLine);
	sprintf(key,"thunderdashboard_auto_%d", kCrossAutoLine);
	SmartDashboard::PutString(key, "Crosses Auto Line");

	sprintf(&buffer[strlen(buffer)],",%d", kSwitchLeft);
	sprintf(key,"thunderdashboard_auto_%d", kSwitchLeft);
	SmartDashboard::PutString(key, "Starts left, goes to switch");

	sprintf(&buffer[strlen(buffer)],",%d", kSwitchCenter);
	sprintf(key,"thunderdashboard_auto_%d", kSwitchCenter);
	SmartDashboard::PutString(key, "Starts center, goes to switch");

	sprintf(&buffer[strlen(buffer)],",%d", kSwitchRightIn);
	sprintf(key,"thunderdashboard_auto_%d", kSwitchRightIn);
	SmartDashboard::PutString(key, "Starts right across from switch, goes to switch");

	sprintf(&buffer[strlen(buffer)],",%d", kSwitchRightOut);
	sprintf(key,"thunderdashboard_auto_%d", kSwitchRightOut);
	SmartDashboard::PutString(key, "Starts right corner, goes to switch");

	sprintf(&buffer[strlen(buffer)],",%d", kSwitchToExchange);
	sprintf(key,"thunderdashboard_auto_%d", kSwitchToExchange);
	SmartDashboard::PutString(key, "Starts center, go switch then exchange");

	sprintf(&buffer[strlen(buffer)],",%d", kSwitchTwice);
	sprintf(key,"thunderdashboard_auto_%d", kSwitchTwice);
	SmartDashboard::PutString(key, "Starts center, goes to switch multiple times");

	sprintf(&buffer[strlen(buffer)],",%d", kExchange);
	sprintf(key,"thunderdashboard_auto_%d", kExchange);
	SmartDashboard::PutString(key, "Start right, exchange");

	sprintf(&buffer[strlen(buffer)],",%d", kVisionFollowPrism);
	sprintf(key,"thunderdashboard_auto_%d", kVisionFollowPrism);
	SmartDashboard::PutString(key, "hopefully follows yellow object");

	SmartDashboard::PutString("thunderdashboard_auto_list", buffer);
}

void Autonomoose::Debug(Feedback *feedback){
	//char data[3];
	//std::strcpy(data,_);
	_fmsData = frc::DriverStation::GetInstance().GetGameSpecificMessage();
	feedback->send_Debug_String("Autonomoose", "fms data", _fmsData.c_str());
	feedback->send_Debug_Double("Autonomoose", "Step", _currentStep);

	_waitTime = feedback->get_Editable_Double("thunderdashboard", "auto_start_delay", 0);
}

void Autonomoose::doNothing() {
	dont();
}

void Autonomoose::crossAutoLine() {
	if(_currentStep == 0){
		_timer.Reset();
		_currentStep++;
	}
	if(_currentStep == 1){
		if(_timer.Get() > _waitTime){
			_drive->autoDriveStraight(150, .8);
			_currentStep++;
		}
	}
	if(_currentStep == 2){
		if(_drive->isAutoStraightFinished()) {
			_currentStep++;
		}
	}
	if(_currentStep == 3) {
		printf("auto done \n");
	}
}

void Autonomoose::goToSwitch(){	//need to add in checking for which side is ours
	if(_currentStep == 0){
		_timer.Reset();
		_currentStep++;
	}
	switch (_startPos){
		case kLeft:
			if(_currentStep == 1){
				if(_timer.Get() > _waitTime){
					_cubeSystem->setArmPosition(Arm::REST);
					_drive->autoDriveStraight(138, .3);
					_currentStep++;
				}
			}
			if(_currentStep == 2){
				if(_drive->isAutoStraightFinished()){
					if(_fmsData[0] == 'L' ){
						_cubeSystem->setArmPosition(Arm::SWITCHFRONT);
						_drive->autoTurn(90);
						_currentStep++;
					}
					else{
						_currentStep = 99;
					}
				}
			}
			if(_currentStep == 3){
				if(_drive->isAutoTurnFinished() && _cubeSystem->wasArmAtPosition()) {
					_drive->autoDriveStraight(20, .2);
					_timer.Reset();
					_currentStep++;
				}
			}
			if(_currentStep == 4){
				if(_drive->isAutoStraightFinished() || _timer.Get() > 1.25){
					_cubeSystem->setCubeOperation(CubeSystem::OUT);
					_timer.Reset();
					_currentStep++;
				}
			}
			if(_currentStep == 5){
				if(_timer.Get() > 3){
					_cubeSystem->setCubeOperation(CubeSystem::STOP);
				}
			}
			break;
		case kCenter:
			if(_currentStep == 1){
				_drive->autoDriveStraight(25, 1);
				_timer.Reset();
				_currentStep++;
			}
			if(_currentStep == 2){
				if(_drive->isAutoStraightFinished() || _timer.Get() > .5){
					if(_fmsData[0] == 'R'){
						_drive->autoTurn(50);
					}
					else {
						_drive->autoTurn(-50);
					}
					_timer.Reset();
					_currentStep++;
				}

			}
			if(_currentStep == 3){
				if( _drive->isAutoTurnFinished() && _timer.Get()> .5){
					_drive->autoDriveStraight(35, 1);
					_timer.Reset();
					_currentStep++;
				}
			}
			if(_currentStep == 4){
				_cubeSystem->setArmPosition(Arm::SWITCHFRONT);
				if(_drive->isAutoStraightFinished() || _timer.Get() > 2){
					if(_fmsData[0] == 'R'){
						_drive->autoTurn(-50);
					}
					else {
						_drive->autoTurn(50);
					}
					_currentStep++;
					_timer.Reset();
				}
			}
			if(_currentStep == 5){
				_cubeSystem->setArmPosition(Arm::SWITCHFRONT);
				if(_drive->isAutoTurnFinished() && _timer.Get() > .5){
					_drive->autoDriveStraight(30, .45);
					_currentStep++;
					_timer.Get();
				}
			}
			if(_currentStep == 6 ){
				if(_drive->isAutoStraightFinished()  || _timer.Get() > 2){
					_timer.Reset();
					_currentStep++;
				}
			}
			if(_currentStep == 7 ){
				_cubeSystem->setCubeOperation(CubeSystem::OUT);
				if(_timer.Get() > kTimeOuttake){
					_cubeSystem->setCubeOperation(CubeSystem::STOP);
					_cubeSystem->setArmPosition(Arm::REST);
					_currentStep++;
				}
			}
			break;
		case kRightIn:
			if(_currentStep == 1){
				_drive->autoDriveStraight(115, .3);
				_cubeSystem->setArmPosition(Arm::SWITCHFRONT);
				_currentStep++;
			}
			if(_currentStep == 2){
				if(_drive->isAutoStraightFinished() && _cubeSystem->wasArmAtPosition()) {

					_cubeSystem->setCubeOperation(CubeSystem::OUT);
					_timer.Reset();
					_currentStep++;
				}
			}
			if(_currentStep == 3){
				if(_timer.Get() > kTimeOuttake){
					_cubeSystem->setCubeOperation(CubeSystem::STOP);
				}
			}
			break;
		case kRightOut:
			if(_currentStep == 1){
				_drive->autoDriveStraight(138, .3);
				_cubeSystem->setArmPosition(Arm::REST);
				_currentStep++;
			}
			if(_currentStep == 2){
				if(_drive->isAutoStraightFinished()){
					if(_fmsData[0] == 'R' ){
						_cubeSystem->setArmPosition(Arm::SWITCHFRONT);
						_drive->autoTurn(-90);
						_currentStep++;
					}
					else{
						_currentStep = 99;
					}
				}
			}
			if(_currentStep == 3){
				if(_drive->isAutoTurnFinished() && _cubeSystem->wasArmAtPosition()) {
					_drive->autoDriveStraight(20, .2);
					_cubeSystem->setArmPosition(Arm::SWITCHFRONT);
					_timer.Reset();
					_currentStep++;
				}
			}
			if(_currentStep == 4){
				if(_drive->isAutoStraightFinished() || _timer.Get() > 1.25){
					_cubeSystem->setCubeOperation(CubeSystem::OUT);
					_timer.Reset();
					_currentStep++;
				}
			}
			if(_currentStep == 5){
				if(_timer.Get() > 3){
					_cubeSystem->setCubeOperation(CubeSystem::STOP);
				}
			}
			break;
	}
}

void Autonomoose::switchToExchange(){	//THIS WONT PUT CUBE IN OR ARM TO BACK, CHANGE WHEN WE CAN DRIVE
	if(_currentStep < 8){
		_startPos = kCenter;
		goToSwitch();
	}
	else if(_currentStep == 8){
		if(_fmsData[0] == 'L'){
			_drive->autoArcTurn(-.65, -.35, 75);
		}
		else{
			_drive->autoArcTurn(-.35, -.65, -75);
		}
		_timer.Reset();
		_currentStep++;
	}
	else if(_currentStep == 9){
		if(_timer.Get() > .25){
			_cubeSystem->setArmPosition(Arm::FRONT);
		}
		if(_drive->isAutoTurnFinished() && _timer.Get() > .5){
			if(_fmsData[0] == 'L'){
				_drive->autoTurn(65);
			}
			else{
				_drive->autoTurn(-65);
			}
			_currentStep++;
			_timer.Reset();
		}
	}
	else if(_currentStep == 10){
		_cubeSystem->setArmPosition(Arm::FRONT);
		if(_drive->isAutoTurnFinished() && _timer.Get() > .5){
			_cubeSystem->setCubeOperation(CubeSystem::IN);
			_drive->autoDriveStraight(45, .4);
			_timer.Reset();
			_currentStep++;
		}
	}
	else if(_currentStep == 11){
		if(_drive->isAutoStraightFinished() || _cubeSystem->isCubePresent() || _timer.Get() > 3){
			_cubeSystem->setCubeOperation(CubeSystem::STOP);
			_drive->autoDriveStraight(20, -.35);
			_currentStep++;
		}
	}
	else if(_currentStep == 12){
		if(_drive->isAutoStraightFinished()){
			_cubeSystem->setArmPosition(Arm::REST);	//FIX THIS PLSSSSS
			_currentStep++;
			if(_fmsData[0] == 'L'){
				_drive->autoTurn(-10);
			}
			else{
				_drive->autoTurn(30);
			}
		}
	}
	else if(_currentStep == 13){
		if(_drive->isAutoTurnFinished()){
				_drive->autoVisionGoTo(false, kExchangeArea, Vision::EXCHANGE, true);
				_currentStep = 15;
		}
	}
	else if(_currentStep == 14){
		if(_drive->isAutoStraightFinished()){
			//_cubeSystem->setArmPosition(Arm::BACK);	//FIX THIS TOOOOOOO
			_drive->autoVisionGoTo(true, kExchangeArea, Vision::EXCHANGE, false);
			_currentStep++;
		}
	}
	else if(_currentStep == 15){
		if(_drive->isAutoVisionGoToFinished()){
			_drive->autoDriveStraight(20, -.3);
			_currentStep++;
		}
	}
	else if(_currentStep == 16){
		if(_drive->isAutoStraightFinished()){
			_timer.Reset();
			//_cubeSystem->setCubeOperation(CubeSystem::OUT);	//ADD BACK IN
			_currentStep++;
		}
	}
	else if(_currentStep== 17){
		if(_timer.Get() > kTimeOuttake){
#ifndef rb2017
			_cubeSystem->setCubeOperation(CubeSystem::STOP);
#endif
			_currentStep++;
		}
	}
}

void Autonomoose::switchAgain(){
	if(_currentStep < 8){
#ifdef rb2
		_currentStep=11;
#else
		_startPos = kCenter;
		goToSwitch();
#endif
	}
	else if(_currentStep == 8){
		if(_fmsData[0] == 'L'){
			_drive->autoArcTurn(-.65, -.35, 70);
		}
		else{
			_drive->autoArcTurn(-.35, -.65, -70);
		}
		_timer.Reset();

		_currentStep++;
	}
	else if(_currentStep == 9){
		if(_timer.Get() > .25){
			_cubeSystem->setArmPosition(Arm::FRONT);
		}
		if(_drive->isAutoTurnFinished() && _timer.Get() > .5){
			if(_fmsData[0] == 'L'){
				_drive->autoTurn(65);
			}
			else{
				_drive->autoTurn(-65);
			}
			_currentStep++;
			_timer.Reset();
		}
	}
	else if(_currentStep == 10){
		if(_drive->isAutoTurnFinished() && _timer.Get() > .5){
			_cubeSystem->setArmPosition(Arm::FRONT);
			_cubeSystem->setCubeOperation(CubeSystem::IN);
			_drive->autoDriveStraight(45, .4);
			_timer.Reset();
			_currentStep++;
		}
	}
	else if(_currentStep == 11){
		if(_drive->isAutoStraightFinished() || _cubeSystem->isCubePresent() || _timer.Get() > 3){
			_cubeSystem->setCubeOperation(CubeSystem::STOP);
			_drive->autoDriveStraight(20, -.35);
			_currentStep++;
		}
	}
	else if(_currentStep == 12){
		_cubeSystem->setArmPosition(Arm::REST);
		if(_drive->isAutoStraightFinished()){
#ifndef rb2
			if(_fmsData[0] == 'L'){
				_drive->autoTurn(-65);
			}
			else{
				_drive->autoTurn(65);
			}
#else
			if(_fmsData[0] == 'L'){
				_drive->autoTurn(-45);
			}
			else{
				_drive->autoTurn(45);
			}
#endif
			_currentStep++;
			_timer.Reset();
		}
	}
	else if(_currentStep == 13){
		if(_drive->isAutoTurnFinished()){
			_cubeSystem->setArmPosition(Arm::SWITCHFRONT);
			if(_fmsData[0] == 'L'){
				_drive->autoArcTurn(.65, .47, -75);
			}
			else{
				_drive->autoArcTurn(.47, .65, 75);
			}
			_timer.Reset();
			_currentStep++;
		}
	}
	else if(_currentStep == 14){
		if(_drive->isAutoTurnFinished() || _timer.Get() > 2){
			_cubeSystem->setArmPosition(Arm::SWITCHFRONT);
			_drive->autoDriveStraight(20, .4);
			_timer.Reset();
			_currentStep++;
		}
	}
	else if(_currentStep == 15){
		if(_drive->isAutoStraightFinished() || _timer.Get()>1){
			_timer.Reset();
			_cubeSystem->setCubeOperation(CubeSystem::OUT);
			_currentStep++;
		}
	}
	else if(_currentStep == 16){
		if(_timer.Get() > kTimeOuttake){
			_cubeSystem->setCubeOperation(CubeSystem::STOP);
			_currentStep = 8;	//MIGHT NEED TO FIX
		}
	}
}

void Autonomoose::exchange(){
	if(_currentStep == 0){
		_timer.Reset();
		_currentStep++;
	}
	if(_currentStep == 1){
		if(_timer.Get() > _waitTime){	//THIS ENABLES A WAIT TIME IF NEEDED
			_drive->autoArcTurn(.4, .3, 50);
			_currentStep++;
		}
	}
	if(_currentStep == 2){
		if(_drive->isAutoTurnFinished()){
			_drive->autoDriveStraight(20,.3);
			_currentStep++;
		}
	}
	if(_currentStep == 3){
		if(_drive->isAutoStraightFinished()){
			_drive->autoTurn(-45);
			_cubeSystem->setArmPosition(Arm::BACK);
			_timer.Reset();
			_currentStep++;
		}
	}
	if(_currentStep == 4){
		if(_drive->isAutoTurnFinished() && _cubeSystem->wasArmAtPosition() && _timer.Get() > 4){
			_drive->autoDriveStraight(50, -.1);
			_currentStep++;
		}
	}
	if(_currentStep == 5){
		if(_drive->isAutoStraightFinished()){
			_cubeSystem->setCubeOperation(CubeSystem::OUT);
			_timer.Reset();
			_currentStep++;
		}
	}
	if(_currentStep == 6){
		if(_timer.Get()>1.5){
			_cubeSystem->setCubeOperation(CubeSystem::STOP);
			_currentStep++;
		}
	}
}

void Autonomoose::nullZone(){
	if(_currentStep== 0){
		_timer.Reset();
	}
	if(_currentStep == 1){
		if(_timer.Get() > _waitTime){
			_drive->autoDriveStraight(325, .8);
			_currentStep++;
		}
	}
	if(_currentStep == 2){
		if(_drive->isAutoStraightFinished()) {
			_currentStep++;
		}
	}
	if(_currentStep == 3) {
		printf("auto done \n");
	}
}

void Autonomoose::visionFollowPrism(){
	if(_currentStep==0){
		_drive->autoVisionGoTo(true, kExchangeArea + 10000, Vision::CUBE, true);
		_currentStep++;
	}
}
