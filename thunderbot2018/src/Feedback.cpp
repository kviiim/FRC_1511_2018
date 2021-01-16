/*
 * Feedback.cpp
 *
 *  Created on: Jan 20, 2018
 *      Author: Robotics
 */

#include "Feedback.h"

Feedback::Feedback() {
	// TODO Auto-generated constructor stub

}


void Feedback::send_Debug_String(const char *subsystemName, const char *dataName, const char *format, ...) {//

	std::string keyName;
		keyName = subsystemName;
		keyName += "_";
		keyName += dataName; //adds a _ after your subsytem name, the name of your specific value after that
	  char buffer[256]; //sets the maximum character count to 256
	  va_list args; // args is the "..."
	  va_start (args, format); //marks the start of the list of arguments(args)
	  vsnprintf (buffer,256,format, args); //printing the args after the keyName
	  va_end (args);//marks end of arguments (this allows us to change the arguments and not the keyname)
	  SmartDashboard::PutString(keyName.c_str(), buffer);//this prints keyName into the table, then the arguments
	}

void Feedback::send_Debug_Double(const char *subsystemName, const char *dataName, double format){

	std::string keyName;
	keyName = subsystemName;
	keyName += "_";
	keyName += dataName; // a _ after the subsystem name, and adds the name of the value after that
	SmartDashboard::PutNumber(keyName.c_str(), format); //this prints keyName and the data into the table
}

void Feedback::send_Editable_String(const char *subsystemName, const char *dataName, const char *format, ...) {//

	std::string keyName;
		keyName = subsystemName;
		keyName += "_";
		keyName += dataName; //adds a _ after your subsytem name, the name of your specific value after that
		keyName += "_e"; // End in _e to make editable
	  char buffer[256]; //sets the maximum character count to 256
	  va_list args; // args is the "..."
	  va_start (args, format); //marks the start of the list of arguments(args)
	  vsnprintf (buffer,256,format, args); //printing the args after the keyName
	  va_end (args);//marks end of arguments (this allows us to change the arguments and not the keyname)
	  SmartDashboard::PutString(keyName.c_str(), buffer);//this prints keyName into the table, then the arguments
	}

void Feedback::send_Editable_Double(const char *subsystemName, const char *dataName, double format){

	std::string keyName;
	keyName = subsystemName;
	keyName += "_";
	keyName += dataName; // a _ after the subsystem name, and adds the name of the value after that
	keyName += "_e"; // End in _e to make editable
	SmartDashboard::PutNumber(keyName.c_str(), format); //this prints keyName and the data into the table
}


std::string Feedback::get_Editable_String(const char *subsystemName, const char *dataName, llvm::StringRef defaultVal) {//

	std::string keyName;
		keyName = subsystemName;
		keyName += "_";
		keyName += dataName; //adds a _ after your subsytem name, the name of your specific value after that
		keyName += "_e"; // End in _e to make editable
	  return SmartDashboard::GetString(keyName.c_str(), defaultVal);//this prints keyName into the table, then the arguments
	}

double Feedback::get_Editable_Double(const char *subsystemName, const char *dataName, double defaultVal){

	std::string keyName;
	keyName = subsystemName;
	keyName += "_";
	keyName += dataName; // a _ after the subsystem name, and adds the name of the value after that
	keyName += "_e"; // End in _e to make editable
	return SmartDashboard::GetNumber(keyName.c_str(), defaultVal);
}

