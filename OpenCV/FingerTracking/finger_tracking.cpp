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

	Scalar gr_low  = Scalar(34, 85, 105);
	Scalar gr_high = Scalar(69, 255, 255);

	Scalar ye_low  = Scalar(19, 117, 213);
	Scalar ye_high = Scalar(51, 204, 255);

	Scalar or_low  = Scalar(0, 193, 216);
	Scalar or_high = Scalar(34, 255, 255);
		
	Scalar bl_low  = Scalar(91, 58, 216);
	Scalar bl_high = Scalar(120, 111, 255);

	Scalar pk_low  = Scalar(146, 83, 207);
	Scalar pk_high = Scalar(175, 157, 255);

	Scalar pr_low  = Scalar(118, 73, 84);
	Scalar pr_high = Scalar(157, 148, 174);

	/*
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
	*/

	/* capture temp image from camera */
	Mat imgTmp;
	cap.read(imgTmp);

	/* create black image with size as camera output */
	Mat imgLines = Mat::zeros(imgTmp.size(), CV_8UC3);;
	

	while (true) {
		Mat imgOriginal;
		bool bSuccess = cap.read(imgOriginal);

		if (!bSuccess) {
			cout <<"Cannot read a frame from video stream" <<endl;
			break;
		}
		/* create our 6 layers of threshed images */
		Mat imgThresholded[6];
		imgThresholded[0] = getThresholdedImage(imgOriginal, gr_low, gr_high);
		imgThresholded[1] = getThresholdedImage(imgOriginal, ye_low, ye_high);
		imgThresholded[2] = getThresholdedImage(imgOriginal, or_low, or_high);
		imgThresholded[3] = getThresholdedImage(imgOriginal, bl_low, bl_high);
		imgThresholded[4] = getThresholdedImage(imgOriginal, pk_low, pk_high);
		imgThresholded[5] = getThresholdedImage(imgOriginal, pr_low, pr_high);

		/*
		imshow("Thresholded Image Green", imgThresholded[0]);
		imshow("Thresholded Image Yellow", imgThresholded[1]);
		imshow("Thresholded Image Orange", imgThresholded[2]);
		imshow("Thresholded Image Blue", imgThresholded[3]);
		imshow("Thresholded Image Pink", imgThresholded[4]);
		imshow("Thresholded Image Purple", imgThresholded[5]);
		*/

		Point p_curr[6];
		int i;
		for (i = 0; i < 6; i++) {
		 p_curr[i] = getPoint(imgThresholded[i]);
		}	 
		
		circle(imgLines, p_curr[0], 10, Scalar(0, 153, 0), -1, 8);
		circle(imgLines, p_curr[1], 10, Scalar(255, 255, 0), -1, 8);
		circle(imgLines, p_curr[2], 10, Scalar(255, 128, 0), -1, 8);
		circle(imgLines, p_curr[3], 10, Scalar(0, 0, 255), -1, 8);
		circle(imgLines, p_curr[4], 10, Scalar(255, 0, 127), -1, 8);
		circle(imgLines, p_curr[5], 10, Scalar(102, 0, 102), -1, 8);

		imgOriginal = imgOriginal + imgLines;
		imshow("Original", imgOriginal);
		/* printf("O(%d, %d), Y(%d, %d), G(%d, %d)\n", p_curr[0].x, p_curr[0].y
				, p_curr[1].x, p_curr[1].y, p_curr[2].x, p_curr[2].y);
		*/

		if (waitKey(20) == 27) {
			cout <<"esk key is pressed by user"<< endl;
			break;
		}
	}
	
	return 0;
}



