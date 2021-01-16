#include <math.h>

#include "StallCheck.h"

StallCheck::StallCheck(float currentThreshold, unsigned int measurementWindowSamples) :
	_currentThreshold(currentThreshold),
	_measurementWindowSamples(measurementWindowSamples),
	_numMeasurements(0)
{
	_currentMeasurements = new float[_measurementWindowSamples];
	reset();
}

StallCheck::~StallCheck() {
	// free memory on destruct
	delete _currentMeasurements;
}

void StallCheck::feedCurrentMeasurement(float currentMeasurement) {
	_currentMeasurements[_numMeasurements % _measurementWindowSamples] = currentMeasurement;
	_numMeasurements++;
}

float StallCheck::averageCurrent(){
	float sum = 0;

	if(_numMeasurements == 0){
		return 0;
	} else {
		for(unsigned int ix = 0; ix < _measurementWindowSamples; ix++){
			sum += _currentMeasurements[ix];
		}

		return sum / ((float) _measurementWindowSamples);
	}
}

bool StallCheck::shouldStall() {
	return fabs(averageCurrent() >= _currentThreshold);
}

void StallCheck::reset(){
	for(unsigned int ix = 0; ix < _measurementWindowSamples; ix++){
		_currentMeasurements[ix] = 0;
	}
	_numMeasurements = 0;
}
