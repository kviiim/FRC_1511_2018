#ifndef SRC_DRIVECOUNTER_H_
#define SRC_DRIVECOUNTER_H_

#include "WPILib.h"
#include "Feedback.h"

class DriveCounter {
public:
	DriveCounter(int id);

	/*
	 * Resets all values and buffers to zero
	 */
	void Reset();
	/*
	 * Updates speed values
	 */
	void process();
	/*
	 * Gets raw counter value
	 */
	int Get();
	/*
	 * Returns calculated speed
	 */
	int GetSpeed();

private:
	Counter _counter;
	Timer _timer;

	float _buffer[5];
	float _timeBuffer[5];

	float _speed;
};

#endif /* SRC_DRIVECOUNTER_H_ */
