#include "definition-detect.h"

int DisplayVideo()
{
    //store image at here
	Mat image = imread("..\\photoes\\test.jpg");
	imshow("show image 0", image);
	waitKey(0);
	return 0;
}

int OpenCamera0()
{
    // read camera
	VideoCapture capture(0);
	Mat frame;

	// Check if camera opened successfully
	if (!capture.isOpened()) {
		cout << "camera 0 opened with failure, it doesn't work?" << endl;
		return -1;
	}

	while (true)
	{
		Mat frame;
		double meanValue = 0.0;
		capture >> frame;
		putText(frame, "nce-8900k", Point(10, 30), FONT_HERSHEY_PLAIN, 2.0, Scalar(0, 0, 255), 2, LINE_AA);
		meanValue = DefinitionDetect(frame);
		cv::rectangle(
			frame,
			cv::Point(150, 180),
			cv::Point(450, 380),
			meanValue > DEFINITION_THRESHOLD ? cv::Scalar(0, 255, 0): cv::Scalar(255, 0, 0),
			5, 8
		);
		imshow("read video 0", frame);
		//delay for 30
		waitKey(30);
	}
	return 0;
}

int OpenCamera1() {
	// read camera
	VideoCapture capture(1);
	Mat frame;

	// Check if camera opened successfully
	if (!capture.isOpened()) {
		cout << "camera 1 opened with failure, it doesn't work?" << endl;
		return -1;
	}

	while (true)
	{
		Mat frame;
		double meanValue = 0.0;
		capture >> frame;
		putText(frame, "nce-8900k", Point(10, 30), FONT_HERSHEY_PLAIN, 2.0, Scalar(0, 0, 255), 2, LINE_AA);
		meanValue = DefinitionDetect(frame);
		cv::rectangle(
			frame,
			cv::Point(150, 180),
			cv::Point(450, 380),
			meanValue > DEFINITION_THRESHOLD ? cv::Scalar(0, 255, 0) : cv::Scalar(255, 0, 0),
			5, 8
		);
		//creat different window
		imshow("read video 1", frame);
		//delay for 30
		waitKey(30);
	}
	return 0;
}