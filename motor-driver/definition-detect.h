#pragma once
#include "stdafx.h"
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#define DEFINITION_THRESHOLD 1
#define BIN_DIV 120

using namespace std;
using namespace cv;

double DefinitionDetect(Mat &frame);