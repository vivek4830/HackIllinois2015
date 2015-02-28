// finger_tracking.cpp

#include <iostream>
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/photo.hpp"

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

int main()
{
	VideoCapture cap(1);
	if (!cap.isOpened()) {
		cout<<"Cannot open the web cam"<<endl;
		return -1;
	}

	namedWindow("Control");

	int iLowH_or = 0;
	int iHighH_or = 22;

	int iLowH_ye = 22;
	int iHighH_ye = 38;

	int iLowH_gr = 38;
	int iHighH_gr = 75;

	int iLowH_bl = 75;
	int iHighH_bl = 130;
	
	int iLowH_vi = 130;
	int iHighH_vi = 160;

	int iLowH_rd = 160;
	int iHighH_rd = 179;

	int iLowS = 150;
	int iHighS = 255;

	int iLowV = 60;
	int iHighV = 255;

	
	// create trackbars in "control" window
	createTrackbar("LowH_orange", "Control", &iLowH_or, 179);
	createTrackbar("HighH_orange", "Control", &iHighH_or, 179);

	createTrackbar("LowH_yellow", "Control", &iLowH_ye, 179);
	createTrackbar("HighH_yellow", "Control", &iHighH_ye, 179);

	createTrackbar("LowH_green", "Control", &iLowH_gr, 179);
	createTrackbar("HighH_green", "Control", &iHighH_gr, 179);

	createTrackbar("LowS", "Control", &iLowS, 255);
	createTrackbar("HighS", "Control", &iHighS, 255);

	createTrackbar("LowV", "Control", &iLowV, 255);
	createTrackbar("HighV", "Control", &iHighV, 255);
	

	// capture temp image from camera
	Mat imgTmp;
	cap.read(imgTmp);

	// create black image with size as camera output
	Mat imgLines = Mat::zeros(imgTmp.size(), CV_8UC3);;
	

	while (true) {
		Mat imgOriginal;
		bool bSuccess = cap.read(imgOriginal);

		if (!bSuccess) {
			cout <<"Cannot read a frame from video stream" <<endl;
			break;
		}

		Mat imgThresholded[3];
		imgThresholded[0] = getThresholdedImage(imgOriginal,
			Scalar(iLowH_or, iLowS, iLowV), Scalar(iHighH_or, iHighS, iHighV));
	   	imgThresholded[1] = getThresholdedImage(imgOriginal,
			Scalar(iLowH_ye, iLowS, iLowV), Scalar(iHighH_ye, iHighS, iHighV));
		imgThresholded[2] = getThresholdedImage(imgOriginal,
			Scalar(iLowH_gr, iLowS, iLowV), Scalar(iHighH_gr, iHighS, iHighV));

		imshow("Thresholded Image Orange", imgThresholded[0]);
		imshow("Thresholded Image Yellow", imgThresholded[1]);
		imshow("Thresholded Image Green", imgThresholded[2]);

		Point p_curr[3];
		int i;
		for (i = 0; i < 3; i++) {
		 p_curr[i] = getPoint(imgThresholded[i]);
		}	 
		
		circle(imgLines, p_curr[0], 15, Scalar(236, 92, 20), -1, 8);
		circle(imgLines, p_curr[1], 15, Scalar(222, 236, 20), -1, 8);
		circle(imgLines, p_curr[2], 15, Scalar(26, 166, 16), -1, 8);

		imgOriginal = imgOriginal + imgLines;
		imshow("Original", imgOriginal);
		printf("O(%d, %d), Y(%d, %d), G(%d, %d)\n", p_curr[0].x, p_curr[0].y
				, p_curr[1].x, p_curr[1].y, p_curr[2].x, p_curr[2].y);


		if (waitKey(20) == 27) {
			cout <<"esk key is pressed by user"<< endl;
			break;
		}
	}
	
	return 0;
}



