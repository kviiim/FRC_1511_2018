#ifndef SRC_VISION_H_
#define SRC_VISION_H_

#include "WPILib.h"
#include <Timer.h>
#include <thread>
#include "IOMap.h"

#include <CameraServer.h>
#include <IterativeRobot.h>
#include <opencv2/core/core.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/features2d.hpp"
#include <vector>

using namespace cv;

class Vision {
public:
	enum VisionTarget {
		NONE,
		CUBE,
		EXCHANGE
	};

	Vision();
	virtual ~Vision();
	/*
	 * Where all data from the camera is interpreted
	 */
	void Process();
	/*
	 * Returns how far the center of the blob is in the x direction in relation to the center of the image, on a scale of -1 (far left) to 1 (far right)
	 */
	float getXPosition();
	/*
	 * Returns how far the center of the blob is in the y direction in relation to the center of the image, on a scale of -1 (far left) to 1 (far right)
	 */
	float getYPosition();
	/*
	 * Returns the area of the largest target found as a percentage of the full image size (0 - 100)
	 */
	float getArea();
	/*
	 * returns if a target is found or not
	 */
	bool targetsFound();
	/*
	 * Sets current target to track
	 */
	void setTarget(VisionTarget target);
	void inAuto(bool inAuto);
	void initCameras();
	VisionTarget _target;
private:
	void Iteration();
	cs::UsbCamera camera;
	cs::UsbCamera driverCamera;
	cs::CvSink cvSink;
	cs::CvSource outputStream;
	cv::Mat _inputMat; //formerly known as steve
	cv::Mat _outputMat; //formerly known as otherSteve
	float _xCoordinate;
	float _yCoordinate;
	double _area;
	bool _targetsFound;
	Scalar _targetUpperLimit;
	Scalar _targetLowerLimit;
	bool _auto;
};



#endif /* SRC_VISION_H_ */
