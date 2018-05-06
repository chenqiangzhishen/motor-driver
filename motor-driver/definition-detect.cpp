#pragma once
#include "definition-detect.h"

double DefinitionDetect(Mat &frame) {
	Mat imageSobel;
	//mean grey value
	double meanValue = 0.0;
	std::string meanValueString;

	Sobel(frame, imageSobel, CV_16U, 1, 1);
	meanValue = mean(imageSobel)[0];

	//TODO: debug code
	meanValueString = std::to_string(meanValue);
	meanValueString = "definition detect value: " + meanValueString;
	putText(frame, meanValueString, Point(10, 60), CV_FONT_HERSHEY_COMPLEX, 0.8, Scalar(255, 255, 25), 2);

	return meanValue;
}