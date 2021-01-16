/*
 * StallCheck.h
 *
 * keeps track of motor current measurements and gives us a way to check for
 * stalling
 */

#ifndef SRC_STALL_CHECK_H_
#define SRC_STALL_CHECK_H_

class StallCheck {
public:
	StallCheck(float currentThreshold, unsigned int measurementWindowSamples);
	~StallCheck();

	/* feed a current measurement to the checker */
	void feedCurrentMeasurement(float currentMeasurement);

	/*
	 * returns the average of available current measurements
	 * returns 0 if there have been no measurements yet
	 */
	float averageCurrent();

	/* returns true if averageCurrent() is at or over threshold */
	bool shouldStall();

	/* zero out all current measurements and the measurement counter */
	void reset();

private:
	float _currentThreshold;
	unsigned int _measurementWindowSamples;
	unsigned int _numMeasurements;
	float* _currentMeasurements;
};

#endif /* SRC_STALL_CHECK_H_ */
