#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include<iostream>
using namespace cv;
using namespace std;


int g_nMedianBlurValue = 2;
Mat element = getStructuringElement(MORPH_RECT, Size(3, 3));
vector<Point> point_seq;

Mat selectChannel(Mat src, int channel);
bool objectDetection(Mat  src, int threshold_vlaue, int areasize, int channel)
{/*
 @param[out] success or fail.
 @param[in]  threshold  threshold for segmentation.
 @param[in]  areasize   threshold for selecting large-enough object.
 @param[in]  channel 1(B), 2(G), 3(R), 4(H), 5(S), 6(V), other(GRAY)
 */
	int i;
	//cvCopy(src,displayImage,NULL);
	Mat displayImage = src.clone();
	//cvClearSeq(point_seq);
	//cvClearSeq(contour);
	//cvClearMemStorage(storage);

	Mat imageSC = selectChannel(src, channel);
	//smooth(imageSC,imageSC,CV_MEDIAN);//图像中值滤波
	medianBlur(imageSC, imageSC, g_nMedianBlurValue * 2 + 1);//中值滤波
															 //cvAdaptiveThreshold( gray, gray, 255, CV_ADAPTIVE_THRESH_MEAN_C,CV_THRESH_BINARY, 7, 0);
	threshold(imageSC, imageSC, threshold_vlaue, 255, CV_THRESH_BINARY);
	if (1)
		threshold(imageSC, imageSC, threshold_vlaue, 255, CV_THRESH_BINARY_INV);              //cvNot(imageSC,imageSC);//把元素的每一位取反
																							  //imageSC->origin = 0;
	dilate(imageSC, imageSC, element);//膨胀
									  //CvScalar color = CV_RGB( 155, 155,155 );//灰度图像
	Scalar color = Scalar(155, 155, 155);
	vector<vector<Point>> Contours;
	vector<Vec4i> Hierarchy;

	findContours(imageSC, Contours, Hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
	vector<Moments> mu(Contours.size());
	vector<Point2f> mc(Contours.size());
	Mat drawing = Mat::zeros(src.size(), CV_8UC3);
	for (int i = 0; i< Contours.size(); i++) {
		mu[i] = moments(Contours[i], false);
	}
	for (int i = 0; i< Contours.size(); i++) {
		mc[i] = Point2d(mu[i].m10 / mu[i].m00, mu[i].m01 / mu[i].m00);
	}

	for (int i = 0; i< Contours.size(); i++) {
		double tmparea = fabs(contourArea(Contours[i]));
		if (tmparea>areasize) {
			drawContours(displayImage, Contours, i, color, 2, 8, Hierarchy, 0, Point());//you can change 1 to CV_FILLED
			if (1) {
				//Mat region=Contour[i];
				//CvMoments moments;
				//cvMoments( region, &moments,0 );
				//cvMoments( &contour, &moments,0 );
				// cvDrawContours( cnt_img, _contours, CV_RGB(255,0,0), CV_RGB(0,255,0), _levels, 3, CV_AA, cvPoint(0,0) ); CV_FILLED
				////////////////////////////////////////////////	
				/*float xd,yd;
				int xc,yc;
				float m00, m10, m01, inv_m00;
				Point point;
				m00 = moments.m00;
				m10 = moments.m10;
				m01 = moments.m01;
				inv_m00 = 1. / m00;
				xd =m10 * inv_m00;//一阶矩
				yd =m01 * inv_m00;
				xc=cvRound(xd);//返回和参数最接近的整数
				yc=cvRound(yd);
				i++;
				point.x=xd;
				point.y=yd;
				cvSeqPush(point_seq, &point );
				circle(displayImage,point,5,cvScalar(255,0,0));*/
				//mu=moments(Contours[i],false);
				//mc=Point2d(mu.m10/mu.m00,mu.m01/mu.m00);
				rectangle(drawing, boundingRect(Contours.at(i)), cvScalar(0, 255, 0));

				char tam[10000];
				sprintf(tam, "(%0.0f,%0.0f,%0.0d,%0.0d)", mc[i].x, mc[i].y, boundingRect(Contours.at(i)).height, boundingRect(Contours.at(i)).width);
				//tam[0]=mc[i].x;

				cout << "质心x " << mc[i].x << " 质心y  " << mc[i].y << " height=" << boundingRect(Contours.at(i)).height << " width=" << boundingRect(Contours.at(i)).width << endl;
				circle(drawing, mc[i], 5, cvScalar(255, 0, 0));
				putText(drawing, tam, Point(mc[i].x, mc[i].y), FONT_HERSHEY_SIMPLEX, 0.4, Scalar(255, 0, 255), 1);
			}
		}
	}
	// 	cvNamedWindow("Result",0);//创建窗口，设置窗口属性标志，不能手动改变窗口大小
	// 	cvMoveWindow("Result",750,0);
	// 	cvResizeWindow("Result",300,200); //缩放窗口
	//     cvShowImage("Result",tempImage);
	// 	cvWaitKey();
	// 	cvDestroyAllWindows();
	imshow("bianyuan", drawing);
	imshow("imageSC", imageSC);
	imshow("src", src);
	waitKey(0);
	return false;
}

Mat selectChannel(Mat src, int channel)
{///select image channel
	Mat image, gray, hsv;

	image = src.clone(); //not directly operate on source image
						 //gray = cvCreateImage( cvGetSize(image), 8, 1 );
						 //hsv = cvCreateImage( cvGetSize(image), 8, 3 );
	cvtColor(image, gray, CV_BGR2GRAY);
	cvtColor(image, hsv, CV_BGR2HSV);
	vector<Mat> imageRGBORHSV;
	Mat imageSC;
	switch (channel)
	{
	case 1:
		//cvSplit(image,imageSC,0,0,0);
		split(image, imageRGBORHSV);
		imageSC = imageRGBORHSV[0];
		break;
	case 2:
		//cvSplit(image,0,imageSC,0,0);
		split(image, imageRGBORHSV);
		imageSC = imageRGBORHSV[1];
		break;
	case 3:
		//cvSplit(image,0,0,imageSC,0);
		split(image, imageRGBORHSV);
		imageSC = imageRGBORHSV[2];
		break;
	case 4:
		//cvSplit(hsv,imageSC,0,0,0);
		split(hsv, imageRGBORHSV);
		imageSC = imageRGBORHSV[0];
		break;
	case 5:
		//cvSplit(hsv,0,imageSC,0,0);
		split(hsv, imageRGBORHSV);
		imageSC = imageRGBORHSV[1];
		break;
	case 6:
		//cvSplit(hsv,0,0,imageSC,0);
		split(hsv, imageRGBORHSV);
		imageSC = imageRGBORHSV[2];
		break;
	default:
		//cvCopy( gray, imageSC, 0 );
		imageSC = gray;
	}
	//cvReleaseImage(&image);
	//cvReleaseImage(&hsv);
	//cvReleaseImage(&gray);
	return imageSC;
}

int getFocus() {
	Mat src = imread("front.jpg", 1);

	//Mat imageSC=selectChannel(src, 1);
	objectDetection(src, 100, 100, 1);
	return 0;
}
