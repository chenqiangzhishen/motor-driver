#pragma once
#include "definition-detect.h"


int DisplayImage()
{
    //store image at here
	Mat image = imread("..\\photoes\\test.jpg");
	imshow("show image 0", image);
	waitKey(0);
	return 0;
}

int WriteVideo() {
	// Create a VideoCapture object and use camera to capture the video
	VideoCapture capture(0);
	// Check if camera opened successfully
	if (!capture.isOpened())
	{
		cout << "Error opening video stream" << endl;
		return -1;
	}
	// Default resolution of the frame is obtained.The default resolution is system dependent.
	int frame_width = capture.get(CV_CAP_PROP_FRAME_WIDTH);
	int frame_height = capture.get(CV_CAP_PROP_FRAME_HEIGHT);
	// Define the codec and create VideoWriter object.The output is stored in 'nce-eye-capture.avi' file.
	VideoWriter video("nce-eye-capture.avi", CV_FOURCC('M', 'J', 'P', 'G'), 10, Size(frame_width, frame_height));
	while (1)
	{
		Mat frame;
		// Capture frame-by-frame
		capture >> frame;
		// If the frame is empty, break immediately
		if (frame.empty())
			break;
		// Write the frame into the file 'outcpp.avi'
		video.write(frame);
		// Display the resulting frame
		imshow("nce-8900k-frame", frame);
		// Press  ESC on keyboard to  exit
		char c = (char)waitKey(1);
		if (c == 27)
			break;
	}
	// When everything done, release the video capture and write object
	capture.release();
	video.release();
	// Closes all the windows
	destroyAllWindows();
	return 0;
}


int WriteVideo1() {
	// Create a VideoCapture object and use camera to capture the video
	VideoCapture capture(1);
	// Check if camera opened successfully
	if (!capture.isOpened())
	{
		cout << "Error opening video stream" << endl;
		return -1;
	}
	// Default resolution of the frame is obtained.The default resolution is system dependent.
	int frame_width = capture.get(CV_CAP_PROP_FRAME_WIDTH);
	int frame_height = capture.get(CV_CAP_PROP_FRAME_HEIGHT);
	// Define the codec and create VideoWriter object.The output is stored in 'nce-eye-capture.avi' file.
	VideoWriter video("nce-eye-capture-video1.avi", CV_FOURCC('M', 'J', 'P', 'G'), 10, Size(frame_width, frame_height));
	while (1)
	{
		Mat frame;
		// Capture frame-by-frame
		capture >> frame;
		// If the frame is empty, break immediately
		if (frame.empty())
			break;
		// Write the frame into the file 'outcpp.avi'
		video.write(frame);
		// Display the resulting frame
		imshow("nce-8900k-frame-vedio1", frame);
		// Press  ESC on keyboard to  exit
		char c = (char)waitKey(1);
		if (c == 27)
			break;
	}
	// When everything done, release the video capture and write object
	capture.release();
	video.release();
	// Closes all the windows
	destroyAllWindows();
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
	bool flag = true;
	while (true)
	{
		Mat frame;
		double meanValue = 0.0;
		std::string position;

		POINT pt = getLeftButtonPosition();
		position = "x=" + std::to_string(pt.x) + ",y=" + std::to_string(pt.y);
		capture >> frame;
		putText(frame, "nce-8900k", Point(10, 30), CV_FONT_HERSHEY_COMPLEX, 0.8, Scalar(0, 0, 255), 2);
		putText(frame, position, Point(10, 90), CV_FONT_HERSHEY_COMPLEX, 0.8, Scalar(0, 255, 0), 2);
		meanValue = DefinitionDetect(frame);

		if (flag) {
			Mat image = imread("..\\photoes\\focus.png");
			if (!image.data)
			{
				std::cout << "open image error" << endl;
				return -1;
			}
			Mat imROI;
			imROI = frame(Rect(200, 200, image.cols, image.rows));
			Mat mask = imread("..\\photoes\\focus.png", 0);
			image.copyTo(imROI, mask);
			flag = false;
		}
		else
		{
			flag = true;
			Sleep(100);
		}
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
