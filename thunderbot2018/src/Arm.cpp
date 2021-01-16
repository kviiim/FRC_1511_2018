/*
 * Arm.cpp
 *
 *  Created on: Jan 17, 2018
 *      Author: Robotics
 */

#include <Arm.h>
#ifndef rb2
const float kPotMinVal = 510;
const float kPotMaxVal = 3847;
#else
const float kPotMinVal = 240;
const float kPotMaxVal = 3480;
#endif
const float kArmFront = .7;
const float kIntakeSpeed = 1;
const float kOuttakeSpeedSwitch = -.7;
const float kOuttakeSpeedExchange = -1;
const float kSlowIntake = .15;

const float kFrontMin = .99;
const float kSwitchFrontMin = .62;
const float kSwitchFrontMax = .69;
const float kSwitchBackMax = .35;
const float kSwitchBackMin = .3;
const float kBackMax = .05;
const float kRestMin = .47;
const float kRestMax = .53;
const float kSecondMin = .86;
const float kSecondMax = .89;

const float kArmSpringArea = .3;
const float kMaxSpeed = 1;
//ALL THINGS THAT DEAL WITH SPEED ARE HERE

#ifndef rb2
const float kArmSpeed = .31;	//any commented out speeds r from competition, should return to those for offseasons
//normal speeds without cube
const float kNormalUpSlow = 1;
const float kNormalUpFast = 1.4;
const float kNormalDownFast = .4;
const float kNormalDownSlow = .35;
//speeds with cube
const float kCubeUpSlow = 1; //should always be > 1
const float kCubeUpFast = 1.8; //should always be > 1
const float kCubeDownSlow = .25;
const float kCubeDownFast = .4;
//speeds if moving front/back no cube
const float kFullMotionUp = 2.3;
const float kFullMotionDownFast = 0;
const float kFullMotionDownSlow = -3;
//speeds if moving front/back with cube
const float kFullMotionCubeUp = 3;
const float kFullMotionCubeDownFast = .1;
const float kFullMotionCubeDownSlow = -3;

const float kPotValOutOfIntake = .91;
const float kOutOfIntakeSpeed = 1.5; // always greater than 0
#else
const float kArmSpeed = .31;
//normal speeds without cube
const float kNormalUpSlow = .85;
const float kNormalUpFast = 1.2;
const float kNormalDownFast = .45;
const float kNormalDownSlow = .35;
//speeds with cube
const float kCubeUpSlow = .9; //should always be > 1
const float kCubeUpFast = 1.9; //should always be > 1
const float kCubeDownSlow = .25;
const float kCubeDownFast = .4;
//speeds if moving front/back no cube
const float kFullMotionUp = 2.6;
const float kFullMotionDownFast = .8;
const float kFullMotionDownSlow = -1.6;
//speeds if moving front/back with cube
const float kFullMotionCubeUp = 3.5;
const float kFullMotionCubeDownFast = .7;
const float kFullMotionCubeDownSlow = -2.5;

const float kPotValOutOfIntake = .91;
const float kOutOfIntakeSpeed = 1; // always greater than 0
#endif

const float kArmStallLimit = 90.0;
const double kStallTime = 1.5;

Arm::Arm():
	_manualMode(true),
	_armPosition(REST),
	_armState(STOP),
	arm(CAN_OUT_CUBE_ARM),
	intake(CAN_OUT_CUBE_HAND),
	potentiometer(ANALOG_IN_ARM_POT),
	cubeSensor(DIGITAL_IN_HAND_CUBE_PRESENT),
	_potBroken(false),
	_cubeSensorBroken(false),
	_armSpeed(0),
	_frontLimitBroken(false),
	_backLimitBroken(false),
	_brakePnu(CAN_PNUEMATICS,PCM_OUT_ARM_BRAKE_DISENGAGE, PCM_OUT_ARM_BRAKE_ENGAGE),
	_handPnu(CAN_PNUEMATICS, PCM_OUT_ARM_HAND_RELEASE, PCM_OUT_ARM_HAND_TIGHTEN),
	_frontLimit(DIGITAL_IN_ARM_FRONT_LIMIT),
	_backLimit(DIGITAL_IN_ARM_BACK_LIMIT),
	_isBrakeSet(false),
	_atPosition(false),
	_hasBeenAtPosition(true),
	_armStall(NOTSTALL),
	_stallIdx(),
	_armIntakeInterfere(false),
	_haveHadCube(false),
	_brakeCounter(0),
	_armFullMotion(false),
	_hadCubeLostIt(false),
	_outtakeSpeed(-1),
	_handPressureOff(false)

{
	_handPnu.Set(DoubleSolenoid::kReverse);
	_brakePnu.Set(DoubleSolenoid::kForward);
	arm.SetNeutralMode(NeutralMode::Brake);
	intake.SetNeutralMode(NeutralMode::Brake);
	_timer.Start();
}

void Arm::process(){
	if(!_potBroken && !_manualMode){
		switch (_armPosition){
		case FRONT:
			goToPosition(kFrontMin, 10);
			_outtakeSpeed = kOuttakeSpeedExchange;
			break;
		case SWITCHFRONT:
			goToPosition(kSwitchFrontMin, kSwitchFrontMax);
			_outtakeSpeed = kOuttakeSpeedSwitch;
			break;
#ifndef ARMNOWORK
		case BACK:
			goToPosition(-10, kBackMax);
			_outtakeSpeed = kOuttakeSpeedExchange;
			break;
		case SWITCHBACK:
			goToPosition(kSwitchBackMin, kSwitchBackMax);
			_outtakeSpeed = kOuttakeSpeedSwitch;
			break;
		case REST:
			goToPosition(kRestMin, kRestMax);
			_outtakeSpeed = kOuttakeSpeedSwitch;
			break;
		case SECONDLEVEL:
			goToPosition(kSecondMin, kSecondMax);
			_outtakeSpeed = kOuttakeSpeedExchange;
			break;
#endif
		}
	}
	else{
		setArmMotorSpeed(_armSpeed);
	}

	if(getPotValue() > .8 || _handPressureOff){
		setHandPnu(false);
	}
	else{
		setHandPnu(true);
	}

	if(_haveHadCube == true && !cubeSensor.Get()){
		_hadCubeLostIt = true;
	}
	else _hadCubeLostIt = false;

	switch (_armState){
	case STOP:
		setIntakeSpeed(0);
		if(_hadCubeLostIt){
			setIntakeSpeed(kSlowIntake);
		}
		else{
			setIntakeSpeed(0);
		}
		break;
	case INTAKE:
		setIntakeSpeed(kIntakeSpeed);
		break;
	case OUTTAKE:
		setIntakeSpeed(_outtakeSpeed);
		break;
	}

	//----------------HOW 2 NOT STALL WEW---------------------

	if(_armStall != STALL){
		float avg = 0;
		_currentValues[_stallIdx] = arm.GetOutputCurrent();		//sets a value in the array to the current output
		_stallIdx++;
		if(_stallIdx >= STALLIDXNUM){				//if the number in the array isn't the last one...
			_stallIdx = 0;					//start over
		}

		for(int i=0; i<STALLIDXNUM; i++){			//adds up all the numbers in the array
			avg += _currentValues[i];
		}
		avg = avg/STALLIDXNUM;					//averages all the numbers


		if(avg > kArmStallLimit){			//if the average is more than the stall limit...
			_armStall = STALL;		//set it to stall up
			_timer.Reset();			//and reset the _timer
			_timer.Start();
		}
	}
}

void Arm::setArmSpeed(float speed){
	_armSpeed = calculateArmSpeed(speed * kArmSpeed);
}

void Arm::setArmPosition(armPosition position){
	_hasBeenAtPosition = false;
	_armPosition = position;
}

void Arm::setArmState(armState state){
	_armState = state;
}


void Arm::reset(){	//brake?
	setIntakeSpeed(0);
	setArmMotorSpeed(0);
	setArmState(STOP);
	_hasBeenAtPosition = true;
	setBrake(true);
}

float Arm::getPotValue(){	//higher numbers more forwards
	return 1 - (((float)potentiometer.GetValue() - kPotMinVal)/(kPotMaxVal - kPotMinVal));
}

void Arm::setBrake(bool lock){
	_isBrakeSet = lock;
	if(lock){
		_brakePnu.Set(DoubleSolenoid::kForward);
	}
	else{
		_brakePnu.Set(DoubleSolenoid::kReverse);
	}
}

bool Arm::isArmInFront(){
	if(getPotValue() > kArmFront){	//gets ignored in cube system if pot broken
		return true;
	}
	else{
		return false;
	}
}
bool Arm::isFrontLimitFlipped() {
	return !_frontLimit.Get();
}

bool Arm::isBackLimitFlipped() {
	return !_backLimit.Get();
}

void Arm::setArmBroken(bool broken){	//HOW TO DO BROKEN MODE??? idk.... lots of thinking required
	_potBroken = broken;
}

void Arm::setCubeSensorBroken(bool broken){
	_cubeSensorBroken = broken;
}

bool Arm::isCubePresent(){//gets ignored by cubesystem if broken
	if(cubeSensor.Get()){
		_haveHadCube = true;
	}
	if(_armState == OUTTAKE){
		_haveHadCube = false;
	}
	return _haveHadCube;
}

bool Arm::isCubePresentSensor(){
	return cubeSensor.Get();
}
bool Arm::isCubeSensorBroken(){
	return _cubeSensorBroken;
}

void Arm::goToPosition(float minVal, float maxVal){
	if(!_hasBeenAtPosition){			//if since the last time the goal position has changed we haven't been there
		if(getPotValue() < minVal){		//then adjust the motor speed to go towards that position
			_armSpeed = calculateArmSpeed(kArmSpeed);
			_atPosition = false;
		}			//NEED TO ADD IN SOMEWHERE TO GIVE MORE POWER UP WHEN WE HAVE A CUBE
		else if (getPotValue() > maxVal){
			_armSpeed = calculateArmSpeed(-kArmSpeed);
			_atPosition = false;
		}
		else{							//unless we are actually now at the position and then we stop
			_armSpeed = 0;
			_atPosition = true;
			_hasBeenAtPosition = true;
		}
		setArmMotorSpeed(_armSpeed);
	}
	//This is literally just for debug... should be ignored
	else if(getPotValue() < minVal || getPotValue() > maxVal){
		_atPosition = false;
	}
	else{
		_atPosition = true;
	}
}

void Arm::setArmMotorSpeed(float speed){
	switch(_armStall){		//Checks if the motor is stalling and only lets it go after a certain amount of time
	case(STALL):	//not moving if we stall sounds like a good idea
			printf("Has Stalled");
			if(_timer.Get() < kStallTime){
				arm.Set(ControlMode::PercentOutput, 0);
			}
			else{
				Stall_Zero();
				_armStall = NOTSTALL;
			}
		break;
	case(NOTSTALL):	//if we havent stalled yet then move the motor
		float actualSpeed = speed;
			if(canMoveArm()){	// if we arent at limit switches n moving the wrong way
				if(abs(actualSpeed) > kMaxSpeed){	//LIMITS MAX SPEED PROBABLY SHOULD BE TAKEN OUT EVENTUALLY
					if(actualSpeed < 0){
						actualSpeed = -kMaxSpeed;
					}
					else{
						actualSpeed = kMaxSpeed;
					}
				}
				_armSpeed = actualSpeed;
				//sets break based on if were moving or not
				if(actualSpeed != 0){
					setBrake(false);
				}
				else{
					setBrake(true);
				}
				//Part where we actually set the speed
				if(_brakeCounter < 10){
					_brakeCounter++;
				}
				else{
					arm.Set(ControlMode::PercentOutput, actualSpeed);
				}
			}

			else{	//if at limits n stuff then dont move n set the brake
				_brakeCounter = 0;
				arm.Set(ControlMode::PercentOutput, 0);
				setBrake(true);
			}
		break;
	}
}

float Arm::calculateArmSpeed(float speed){
	float speedPosBased = speed;
	if((speed > 0 && getPotValue() < .5) || (speed < 0 && getPotValue() > .5)){	// if moving forwards starting in back, or moving back and are in front...
		if(!isCubePresent()){
			if(!_armFullMotion){
				if(getPotValue() < kArmSpringArea || getPotValue() > 1 - kArmSpringArea){	//if were more to the front or more to the back than a specific point...
					speedPosBased = speed *kNormalUpFast;	//then go speed*multiplier, dont know what that should be yet .05
				}
				else {
					speedPosBased = speed *kNormalUpSlow;	//also dont know multiplier
				}
				/*if(getPotValue() > kPotValOutOfIntake){
					speedPosBased = kOutOfIntakeSpeed*speedPosBased;
					if(speedPosBased > 1){
						speedPosBased = 1;
					}
				}*/

			}
			else{
				speedPosBased = speed*kFullMotionUp;
			}
		}
		else{	//if we have a cube
			if(!_armFullMotion){
				if(getPotValue() < kArmSpringArea || getPotValue() > 1 - kArmSpringArea){	//if were more to the front or more to the back than a specific point...
					speedPosBased = speed *kCubeUpFast;	//then go speed*multiplier, dont know what that should be yet .05
				}
				else {
					speedPosBased = speed *kCubeUpSlow;	//also dont know multiplier
				}
			}
			else{
				speedPosBased = speed *kFullMotionCubeUp;
			}
		}

	}

	else if((speed > 0 && getPotValue() > .5) || (speed < 0 && getPotValue() < .5)){// if moving forwards and are in front, or moving back starting in back...
		if(!isCubePresent()){
			if((getPotValue() > 1 - kArmSpringArea) || (getPotValue() < kArmSpringArea)){	//if were a certain distance away from center either front or back...
				speedPosBased = speed*kNormalDownSlow;	//moves the motor slower if were going down .25
			}
			else{
				speedPosBased = speed*kNormalDownFast;	//moves it + multiplier it
			}
		}
		else{	//we have cube
			if((getPotValue() > 1 - kArmSpringArea) || (getPotValue() < kArmSpringArea)){	//if were a certain distance away from center either front or back...
				speedPosBased = speed*-kCubeDownSlow;	//moves the motor slower if were going down .25
			}
			else{
				speedPosBased = speed*kNormalDownFast;	//moves it + multiplier it
			}
		}
	}
	//if were exactly at the top then just go that speed
	//printf("speed %f", speedPosBased);
	if(speedPosBased > 1){
		speedPosBased = 1;
	}
	return speedPosBased;
}

void Arm::Stall_Zero(){
	for(int i=0; i<STALLIDXNUM; i++){
		_currentValues[i]=0;
	}
	_stallIdx = 0;
}

void Arm::setIntakeSpeed(float speed){
	intake.Set(ControlMode::PercentOutput,speed);
}

void Arm::setFrontLimitBroken(bool broken) {
	_frontLimitBroken = broken;
}
void Arm::setBackLimitBroken(bool broken) {
	_backLimitBroken = broken;
}

void Arm::armIntake(bool interfere){
	_armIntakeInterfere = interfere;
}

void Arm::armFullMotion(bool full){
	_armFullMotion = full;
}

bool Arm::isArmAtPosition(){
	if(!_potBroken){
		return _hasBeenAtPosition;
	}
	else{
		return true;
	}
}

void Arm::setHandPnu(bool clamp){
	if(clamp){
		_handPnu.Set(DoubleSolenoid::kReverse);
	}
	else{
		_handPnu.Set(DoubleSolenoid::kForward);

	}
}

bool Arm::isFullMotion(){
	return _armFullMotion;
}

void Arm::turnHandPressureOff(bool off){
	_handPressureOff = off;
}

bool Arm::canMoveArm(){
	if(((!isFrontLimitFlipped() || _frontLimitBroken) && _armSpeed > 0) || ((!isBackLimitFlipped() || _backLimitBroken) && _armSpeed < 0)) {	//if not at limits n moving down
		if(((_armIntakeInterfere && _armSpeed < 0) || !_armIntakeInterfere) || _potBroken){	//if the arm isnt in the way of the intake n moving down
			return true;	// then move
		}	//if intake interfere then cant move
		return false;
	}
	else if (_armSpeed == 0){	//returns true if not moving just for debug
		return true;
	}
	else{	//otherwise cant move
		return false;
	}

	/*
	 * returns true if....
	 * 		we want to move forwards and the arm from limit isnt tripped
	 * 		we want to move back and the back limit isnt tripped
	 * 		the speed were going is 0
	 * 		the arm and intake dont interfere or were moving away from the intake if they do
	 * 		the arm pot is broken
	 */
	//what this used to return just in case my slightly easier to read version doesnt work
	//	return ((((!isFrontLimitFlipped() || _frontLimitBroken) && _armSpeed > 0) || ((!isBackLimitFlipped() || _backLimitBroken) && _armSpeed < 0)) && (((_armIntakeInterfere && _armSpeed < 0) || !_armIntakeInterfere) || _potBroken)) || _armSpeed == 0;
}

void Arm::Debug(Feedback *feedback){
	feedback->send_Debug_String("Arm", "arm brake locked?", _isBrakeSet ? "True":"False");
	feedback->send_Debug_String("Arm", "arm can move?", canMoveArm() ? "True":"False");
	feedback->send_Debug_String("Arm", "arm is at position?", _atPosition ? "True":"False");
	feedback->send_Debug_String("Arm", "arm in front?", isArmInFront() ? "True":"False");
	feedback->send_Debug_String("Arm", "arm was at position", _hasBeenAtPosition ? "true":"false");
	feedback->send_Debug_String("Arm", "arm interefere intake", _armIntakeInterfere ? "True":"False");
	feedback->send_Debug_String("Arm", "broken limit back", _backLimitBroken ? "True":"False");
	feedback->send_Debug_String("Arm", "broken limit front", _frontLimitBroken ? "True":"False");
	feedback->send_Debug_String("Arm", "broken pot", _potBroken ? "True":"False");
	feedback->send_Debug_String("Arm", "full motion", _armFullMotion ? "true":"false");
	feedback->send_Debug_String("Arm", "limit front", isFrontLimitFlipped() ? "True":"False");
	feedback->send_Debug_String("Arm", "limit back", isBackLimitFlipped() ? "True":"False");
	feedback->send_Debug_String("Arm", "manual mode", _manualMode ? "True":"False");
	feedback->send_Debug_Double("Arm", "pot value not normalized", potentiometer.GetValue());
	feedback->send_Debug_Double("Arm", "pot value 0-1", getPotValue());
	feedback->send_Debug_Double("Arm", "speed", _armSpeed);

	feedback->send_Debug_String("Hand", "broken cube sensor", _cubeSensorBroken ? "True":"False");
	feedback->send_Debug_String("Hand", "cube was/is present?", isCubePresent() ? "true":"false");
	feedback->send_Debug_String("Hand", "cube present? sensor", cubeSensor.Get() ? "true":"false");

	switch (_armPosition){
			case FRONT:
				feedback->send_Debug_String("Arm", "Position", "Front");
				break;
			case SWITCHFRONT:
				feedback->send_Debug_String("Arm", "Position", "Switch Front");
				break;
#ifndef ARMNOWORK
			case BACK:
				feedback->send_Debug_String("Arm", "Position", "Back");
				break;
			case SWITCHBACK:
				feedback->send_Debug_String("Arm", "Position", "Switch Back");
				break;
			case REST:
				feedback->send_Debug_String("Arm", "Position", "Rest");
				break;
			case SECONDLEVEL:
				feedback->send_Debug_String("Arm", "Position", "Second Level");
				break;
#endif
	}
	switch (_armState){
		case STOP:
			feedback->send_Debug_String("Hand", "State", "Stop");
			break;
		case INTAKE:
			feedback->send_Debug_String("Hand", "State", "Intake");
			break;
		case OUTTAKE:
			feedback->send_Debug_String("", "State", "Outtake");
			break;
	}
	switch(_armStall){
		case STALL:
			feedback->send_Debug_String("Arm", "Stall?", "Stalled");
			break;
		case NOTSTALL:
			feedback->send_Debug_String("Arm", "Stall?", "Not stalled");

			break;
	}
}

double Arm::getDashboardArmPos() {
	float scaledPotValue = getPotValue() * 100;
	return scaledPotValue;
}

