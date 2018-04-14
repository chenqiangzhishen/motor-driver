#include "stdafx.h"
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;

int DisplayVideo()
{
    //store image at here
	Mat image = imread("..\\photoes\\test.jpg");
	imshow("show image", image);
	waitKey(0);
	return 0;
}

int OpenCamera()
{
    // read camera
	VideoCapture capture(0);

	while (true)
	{
		Mat frame;
		capture >> frame;

		putText(frame, "nce-8900k", Point(10, 30), FONT_HERSHEY_PLAIN, 2.0, Scalar(0, 0, 255), 2, LINE_AA);

		cv::rectangle(
			frame,
			cv::Point(100, 50),
			cv::Point(200, 200),
			cv::Scalar(0, 255, 0),
			5, 8
		);

		imshow("read video", frame);
		//rectangle(frame, (384, 0), (510, 128), (0, 255, 0), 3);
		waitKey(30);    //delay for 30
	}
	return 0;
}
