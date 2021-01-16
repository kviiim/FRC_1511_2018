/*
 * Vision.cpp
 *
 *  Created on: Jan 18, 2018
 *      Author: Robotics
 */

#include <Vision.h>

//color limits for yellow
Scalar cubeLowerLimit(15, 125, 50);
Scalar cubeUpperLimit(21, 225, 150);

Scalar exchangeUpperLimit(10, 200, 150);
Scalar exchangeLowerLimit(5, 100, 60);

const float width = 320;
const float height = 240;

Vision::Vision():
	_target(NONE),
	_inputMat(),
	_outputMat(),
	_xCoordinate(0),
	_yCoordinate(0),
	_area(0),
	_targetsFound(false),
	_targetUpperLimit(0, 0, 0),
	_targetLowerLimit(0, 0, 0),
	_auto(true)

{
#ifndef NOVISION

	initCameras();

#endif
}

Vision::~Vision() {
}

void Vision::Process(){
	while (_auto){Iteration();}
}

void Vision::inAuto(bool inAuto){
	_auto = inAuto;
}

void Vision::Iteration(){
	if(cvSink.GrabFrame(_inputMat) == 0) {
		// Send the output the error.
		outputStream.NotifyError(cvSink.GetError());
		// skip the rest of the current iteration
		return;
	}

	cvtColor(_inputMat, _inputMat, COLOR_BGR2HSV);	//converts from bgr to hsv

	inRange(_inputMat, _targetLowerLimit, _targetUpperLimit, _inputMat);

	medianBlur(_inputMat, _inputMat, 3); //blurs it

	cvtColor(_inputMat, _outputMat, COLOR_GRAY2BGR); // converts from black n white to bgr

	std::vector<std::vector<Point>> contours;
	std::vector<Vec4i> hierarchy;

	findContours(_inputMat, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0,0)); //finds contours....

	Mat drawing = Mat::zeros(_inputMat.size(), CV_8UC3);
	for( size_t i = 0; i< contours.size(); i++ )				//and then draws them
	{
	      Scalar color = Scalar(255, 0, 0);
	      drawContours(_outputMat, contours, (int)i, color, 2, 8, hierarchy, 0, Point() );
	}

	//gets moments
	std::vector<Moments> mu(contours.size());
	for(unsigned int i = 0; i < contours.size(); i++){
		mu[i] = moments(contours[i], false);
	}

	//gets the mass centers
	std::vector<Point2f> mc( contours.size());
	for( unsigned int i = 0; i < contours.size(); i++ ){
		mc[i] = Point2f( mu[i].m10/mu[i].m00, mu[i].m01/mu[i].m00);
	}

	//finds the largest of the contours
	if(contours.size() > 0) {
		_targetsFound = true;
		int largest = 0;
		for(unsigned int i = 0; i < contours.size(); i++){
			if(contourArea(contours[i]) > contourArea(contours[largest])) {
				largest = i;
			}
		}
		circle(_outputMat, mc[largest], 4, Scalar(0,0,255), -1, 8, 0 );
		_xCoordinate = mc[largest].x;
		_yCoordinate = mc[largest].y;
		_area = contourArea(contours[largest]);
	}
	else{
		_targetsFound = false;
	}


	outputStream.PutFrame(_outputMat);	//sends image to dashboard
}

float Vision::getXPosition(){
	float position = (_xCoordinate - (width/2))/(width/2);
	if(_target == EXCHANGE) {
		position -= .5;
	}
	return position;
}

float Vision::getYPosition(){
	float position = (_yCoordinate - (height/2))/(height/2);
	return position;
}

float Vision::getArea(){
	double area = (_area/(width*height))*100;
	return area;
}

bool Vision::targetsFound()	{
	if(getArea() > .35){
		return _targetsFound;
	}
	else{
		return false;
	}
}

void Vision::setTarget(VisionTarget target) {
	_target = target;
	switch(_target) {
		case NONE:

		case CUBE:
			_targetLowerLimit = cubeLowerLimit;
			_targetUpperLimit = cubeUpperLimit;
			break;
		case EXCHANGE:
			_targetLowerLimit = exchangeLowerLimit;
			_targetUpperLimit = exchangeUpperLimit;
			break;
		}

}

void Vision::initCameras(){
	std::vector<cs::UsbCameraInfo>  camInfos = cs::UsbCamera::EnumerateUsbCameras();

	for (size_t i = 0; i < camInfos.size(); ++i) {
		printf("Found camera #%d named %s at path %s\n", camInfos[i].dev, camInfos[i].name.c_str(), camInfos[i].path.c_str());
		if (camInfos[i].dev == 1) {
			driverCamera = CameraServer::GetInstance()->StartAutomaticCapture(1);
			driverCamera.SetResolution(320, 240);
			driverCamera.SetFPS(30);
			driverCamera.SetExposureManual(40);
		}

	}
	for (size_t i = 0; i < camInfos.size(); ++i) {
		printf("Found camera #%d named %s at path %s\n", camInfos[i].dev, camInfos[i].name.c_str(), camInfos[i].path.c_str());
		if (camInfos[i].dev == 0) {
			camera = CameraServer::GetInstance()->StartAutomaticCapture(0);
			camera.SetResolution(320, 240);
			camera.SetFPS(30);
			camera.SetExposureManual(40);
			cvSink = CameraServer::GetInstance()->GetVideo();
			outputStream = CameraServer::GetInstance()->PutVideo("Rectangle", width, height);
		}

	}

}
//pizza party
