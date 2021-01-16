/*
 * Lights.cpp
 *
 *  Created on: Mar 31, 2018
 *      Author: Robotics
 */

#include <Lights.h>

#define OFF 0x00
#define RAINBOW 0x01
#define TETRIS 0x02
#define PACMAN 0x03
#define RED 0x04

Lights::Lights() : serial(9600, frc::SerialPort::kMXP), data(0x00) {
	serialWriter = std::thread(&Lights::writeToStream, this);
}

void Lights::writeToStream() {
	while(true) {
		serial.Write(&data, 1);
		serial.Flush();
		Wait(1.0);
	}
}

void Lights::process(bool autoOn, bool enabled) {
	if(autoOn) {
		data = OFF;
	}
	else if(!autoOn) {
		if(enabled) {
			data = PACMAN;
		}
		else {
			data = RED;
		}
	}
}

void Lights::reset() {
	serial.Reset();
}
