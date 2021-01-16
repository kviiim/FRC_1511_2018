#ifndef SRC_FEEDBACK_H_
#define SRC_FEEDBACK_H_

#include <stdio.h>
#include <string.h>
#include <WPILib.h>
#include <stdarg.h>


class Feedback {
public:
	Feedback();
	/*
	 * sends editable string to dashboard
	 */
	void send_Editable_String(const char *subsystem, const char *name, const char *format,...);
	/*
	 * sends editable double to dashboard
	 */
	void send_Editable_Double(const char *subsystem, const char *name, double value);
	/*
	 * reads editable double from dashboard
	 */
	double get_Editable_Double(const char *subsystem, const char *name, double defaultValue);
	/*
	 * reads editable string from dashboard
	 */
	std::string get_Editable_String(const char *subsystem, const char *name, llvm::StringRef defaultValue);
	/*
	 * sends string to dashboard
	 */
	void send_Debug_String(const char *subsystem, const char *name, const char *format,...);
	/*
	 * sends double to dashboard
	 */
	void send_Debug_Double(const char *subsystem, const char *name, double value);
};

#endif /* SRC_FEEDBACK_H_ */
