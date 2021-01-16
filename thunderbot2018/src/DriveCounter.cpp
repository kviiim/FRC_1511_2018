/*
 * DriveCounter.cpp
 *
 *  Created on: Feb 14, 2018
 *      Author: Robotics
 */

#include <DriveCounter.h>

DriveCounter::DriveCounter(int id):
	_counter(id),
	_timer(),
	_speed(0)
{

	for(int i = 0; i<5; i++) {
		_buffer[i] = 0;
		_timeBuffer[i] = 0;
	}
}

void DriveCounter::Reset() {
	_counter.Reset();
	_timer.Reset();
	_timer.Start();

	for(int i = 0; i<5; i++) {
			_buffer[i] = 0;
			_timeBuffer[i] = 0;
		}
}

void DriveCounter::process() {
	_buffer[0] = _buffer[1];
	_buffer[1] = _buffer[2];
	_buffer[2] = _buffer[3];
	_buffer[3] = _buffer[4];
	_buffer[4] = _counter.Get();

	_timeBuffer[0] = _timeBuffer[1];
	_timeBuffer[1] = _timeBuffer[2];
	_timeBuffer[2] = _timeBuffer[3];
	_timeBuffer[3] = _timeBuffer[4];
	_timeBuffer[4] = _timer.Get();

	float deltaTime = _timeBuffer[4] - _timeBuffer[0];
	if(deltaTime < 0) {
		_speed = 0;
	}
	else {
		_speed = (_buffer[4] - _buffer[0]) / deltaTime;
	}

	if(_speed < 0) {
		_speed = 0;
	}
}

int DriveCounter::Get() {
	return _counter.Get();
}

int DriveCounter::GetSpeed() {
	return _speed;
}
