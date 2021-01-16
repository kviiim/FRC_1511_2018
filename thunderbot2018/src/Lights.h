/*
 * Lights.h
 *
 *  Created on: Mar 31, 2018
 *      Author: Robotics
 */

#ifndef SRC_LIGHTS_H_
#define SRC_LIGHTS_H_

#include "WPIlib.h"
#include "SerialPort.h"
#include <thread>

class Lights {
public:
	Lights();
	void process(bool autoOn, bool enabled);
	void reset();
private:
	SerialPort serial;
	std::thread serialWriter;
	void writeToStream();
	char data;
};

#endif /* SRC_LIGHTS_H_ */

/*
LEDs are
  _____.__
_/ ____\__| ____   ____ _____
\   __\|  |/    \ /    \\__  \
 |  |  |  |   |  \   |  \/ __ \_
 |__|  |__|___|  /___|  (____  /
               \/     \/     \/
               __
__  _  ______ |  | __ ____
\ \/ \/ /  _ \|  |/ // __ \
 \     (  <_> )    <\  ___/
  \/\_/ \____/|__|_ \\___  >
                   \/    \/
 */
