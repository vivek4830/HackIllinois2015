// finger_tracker.cpp

#include <iostream>
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/photo.hpp"

#include "finger_tracker.h"

using namespace cv;
using namespace std;

Mat getThresholdedImage(Mat imgOriginal, Scalar low, Scalar high)
{
	// convert image into HSV
	Mat imgHSV;
	cvtColor(imgOriginal, imgHSV, COLOR_BGR2HSV);

	Mat imgThresholded;
	inRange(imgHSV, low, high, imgThresholded);

	// filter out noise
	/*
	erode(imgThresholded, imgThresholded, 
			getStructuringElement(MORPH_ELLIPSE, Size(5,5)));
	dilate(imgThresholded, imgThresholded,
			getStructuringElement(MORPH_ELLIPSE, Size(5,5)));

	dilate(imgThresholded, imgThresholded,
			getStructuringElement(MORPH_ELLIPSE, Size(5,5)));
	erode(imgThresholded, imgThresholded, 
			getStructuringElement(MORPH_ELLIPSE, Size(5,5)));
	*/
	
	
	//fastNlMeansDenoising(imgThresholded, imgThresholded, 3, 7, 21);

	return imgThresholded;
}

Point getPoint(Mat imgThresholded)
{
	// calculate moments of thresholded image
	Moments oMoments = moments(imgThresholded);

	double dM01 = oMoments.m01;
	double dM10 = oMoments.m10;
	double dArea = oMoments.m00;

	// calculate pos of ball
	int posX, posY;
	if (dArea > 1000) {
		posX = dM10/dArea;
		posY = dM01/dArea;
	} else {
		posX = -1;
		posY = -1;
	}

	Point fingertip = Point(posX, posY);
	return fingertip;
}




