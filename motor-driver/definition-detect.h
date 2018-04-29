#pragma once
#include "stdafx.h"
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
using namespace cv;

#define DEFINITION_THRESHOLD 1.4
double DefinitionDetect(Mat &frame);