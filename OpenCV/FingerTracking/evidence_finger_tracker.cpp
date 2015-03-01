// evidence_finger_tracker.cpp

#include <iostream>
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/photo.hpp"

#include "finger_tracker.h"

using namespace cv;
using namespace std;

int main()
{
	int glen = 3;
	Point **gestures = new Point *[glen];
	Point *high_five = new Point [5];
	gestures[0] = high_five;
	high_five[0] = Point(-82,-4);
	high_five[1] = Point(-63, 84);
	high_five[2] = Point(-37, 103);
	high_five[3] = Point(1, 112);
	high_five[4] = Point(32, 76);

	Point *b = new Point [5];
	gestures[1] = b;
	b[0] = Point(-45, 39);
	b[1] = Point(-41, 94);
	b[2] = Point(-30, 103);
	b[3] = Point(-8, 100);
	b[4] = Point(15, 68);

	Point *c = new Point [5];
	gestures[2] = c;
	c[0] = Point(-40, 37);
	c[1] = Point(-35, 49);
	c[2] = Point(-14, 41);
	c[3] = Point(-121, 69);
	c[4] = Point(29, 40);


	VideoCapture cap(1);
	if (!cap.isOpened()) {
		cout<<"Cannot open the web cam"<<endl;
		return -1;
	}

	namedWindow("Control");

	int H_gr_low = 34;
	int H_gr_high = 69;
	int S_gr_low = 85;
	int S_gr_high = 255;
	int V_gr_low = 105;
	int V_gr_high = 255;

	Scalar gr_low  = Scalar(H_gr_low, S_gr_low, V_gr_low);
	Scalar gr_high = Scalar(H_gr_high, S_gr_high, V_gr_high);

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

	/*  create trackbars in "control" window */
	createTrackbar("LowH_green", "Control", &H_gr_low, 179);
	createTrackbar("HighH_green", "Control", &H_gr_high, 179);

	createTrackbar("LowS_green", "Control", &S_gr_low, 255);
	createTrackbar("HighS_green", "Control", &S_gr_high, 255);

	createTrackbar("LowV_green", "Control", &V_gr_low, 255);
	createTrackbar("HighV_green", "Control", &V_gr_high, 255);

	/*
	   createTrackbar("LowH_orange", "Control", &iLowH_or, 179);
	   createTrackbar("HighH_orange", "Control", &iHighH_or, 179);

	   createTrackbar("LowH_yellow", "Control", &iLowH_ye, 179);
	   createTrackbar("HighH_yellow", "Control", &iHighH_ye, 179);


	   createTrackbar("LowS", "Control", &iLowS, 255);
	   createTrackbar("HighS", "Control", &iHighS, 255);

	   createTrackbar("LowV", "Control", &iLowV, 255);
	   createTrackbar("HighV", "Control", &iHighV, 255);
	   */ 

	/* capture temp image from camera */
	Mat imgTmp;
	cap.read(imgTmp);

	/* create black image with size as camera output */
	//Mat imgLines = Mat::zeros(imgTmp.size(), CV_8UC3);;

	// -----------------
	// 5 second delay
	int timer = 5000;
	while (timer > 0) {
		Mat imgOriginal;
		bool bSuccess = cap.read(imgOriginal);
		if (!bSuccess) {
			cout <<"cant read frame from video stream"<<endl;
			break;
		}

		imshow("Original", imgOriginal);

		waitKey(100);	
		timer -= 100;
	}


	// Point holding array
	int alen = 100;
	int counter = 0;
	Point thumb_holding[alen];
	Point index_holding[alen];
	Point middle_holding[alen];
	Point ring_holding[alen];
	Point pinky_holding[alen];
	Point palm_holding[alen];	

	while (true) {
		/* create black image with size as camera output */
		Mat imgLines = Mat::zeros(imgTmp.size(), CV_8UC3);;

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


		imshow("Thresholded Image Green", imgThresholded[0]);
		imshow("Thresholded Image Yellow", imgThresholded[1]);
		imshow("Thresholded Image Orange", imgThresholded[2]);
		imshow("Thresholded Image Blue", imgThresholded[3]);
		imshow("Thresholded Image Pink", imgThresholded[4]);
		imshow("Thresholded Image Purple", imgThresholded[5]);
		
		Point *p_curr = new Point [6];
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

		/*	
			for (i = 0; i < 6; i++) {
			printf("(%d, %d) ", p_curr[i].x, p_curr[i].y);
			}
			putchar('\n');
			*/

		/*
		// SAVE GESTURE
		// write to array
		if (counter < alen/2) {
			thumb_holding[counter] = p_curr[0];
			index_holding[counter] = p_curr[1];
			middle_holding[counter] = p_curr[2];
			ring_holding[counter] = p_curr[3];
			pinky_holding[counter] = p_curr[4];
			palm_holding[counter] = p_curr[5];

			counter++;
		} else {
			Point *gesture = avgGesture(palm_holding, thumb_holding,
					index_holding, middle_holding, ring_holding,
					pinky_holding, alen);
			for (i = 0; i < 5; i++) {
				printf("(%d, %d)\n", gesture[i].x, gesture[i].y);
			}
			break;
		}
		*/

			
		// TEST GESTURE

		// create array to hold transformed coords 
		Point *transcoord = new Point [5];
		transcoord =  transCoord(p_curr);

		//int test;
		if (compareGesture(gestures, glen, transcoord) == 0) {
		printf("Correct! this is a high-five!\n");
		} 
		if (compareGesture(gestures, glen, transcoord) == 1) {
		printf("Correct! this is the letter b in ASL!\n");
		}
		if (compareGesture(gestures, glen, transcoord) == 2) {
		printf("Correct! this is the letter e in ASL!\n");
		}

		if (waitKey(20) == 27) {
			cout <<"esc key is pressed by user"<< endl;
			break;
		}
	}

	return 0;
}
