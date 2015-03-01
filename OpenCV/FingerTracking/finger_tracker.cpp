// finger_tracker.cpp

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <math.h>
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

void printGesture(Point *gesture_coords)
{
	int i;
	for (i = 0; i < 5; i++) {
		printf("%d,%d\n",
				gesture_coords[i].x, gesture_coords[i].y);
	}
}

	/*
	int i;
	for (i = 0; i < 5; i++) {
		outputFile << gesture_coords[i].x << ',' 
			<< gesture_coords[i].y << endl;
	}
	*/
/*
Point *readGesture(FILE *F)
{
	Point *points; 
	points = new Point [5];

	int i;
	for (i = 0; i < 5; i++) {
		fscanf(F, "%d,%d", &points[i].x, &points[i].y);
	}

	return points;
}	
*/

Point avgPalm(Point *palm_coordinates, int len)
{
	// use for loop to move over palm_coordinates
	// collect sum
	double sum_palm_x = 0;
	double sum_palm_y = 0;
	int i;
	for (i = 0; i < len; i++) {
		if (palm_coordinates[i].x != -1) {
			sum_palm_x += palm_coordinates[i].x;
		}
		if (palm_coordinates[i].y != -1) {
			sum_palm_y += palm_coordinates[i].y;
		}
	}

	Point palm_avg = Point(sum_palm_x/len, sum_palm_y/len);
	return palm_avg;
}

Point avgFinger(Point *finger_coordinates, 
		Point palm_avg, int len)
{
 	double sum_finger_x = 0;
	double sum_finger_y = 0;
	// use for loop to move over array
	int i;
	for (i = 0; i < len; i++) {
		if (finger_coordinates[i].x != -1) {
			sum_finger_x += finger_coordinates[i].x;
		}
		if (finger_coordinates[i].y != -1) {
			sum_finger_y += finger_coordinates[i].y;
		}
	}

	Point finger_avg = Point(sum_finger_x/(double)len, sum_finger_y/(double)len);
	return palm_avg - finger_avg;
}

Point *avgGesture(Point *palm_coords, Point *thumb_coords, 
		Point *index_coords, Point *middle_coords, 
		Point *ring_coords, Point *pinky_coords, int len)
{
	// first find palm_avg
	Point palm_avg = avgPalm(palm_coords, len);
	
	// create output array
	Point *avgGestures;
	avgGestures = new Point [5];

	avgGestures[0] = avgFinger(thumb_coords, palm_avg, len);
	avgGestures[1] = avgFinger(index_coords, palm_avg, len);
	avgGestures[2] = avgFinger(middle_coords, palm_avg, len);
	avgGestures[3] = avgFinger(ring_coords, palm_avg, len);
	avgGestures[4] = avgFinger(pinky_coords, palm_avg, len);

	return avgGestures;
}
int compareGesture(Point *gestures, int glen, Point *finger_coords)
{
	int delta = 50;
	double BIG = 10000000;

	// build array to hold differences
	double differences;

	// use for loop to check all gestures
	// and to find error deviations
	int i;
	double difference_x;
	double difference_y;
	// for loop to check all fingers in a gesture
		for (i = 0; i < 5; i++) {
			difference_x = abs(gestures[i].x - finger_coords[i].x);
		   	difference_y = abs(gestures[i].y - finger_coords[i].y);
			
			// now check if meet error tolerances
			if (difference_x > delta || difference_y > delta) {
				differences = BIG;
			} else {
				differences += (difference_x + difference_y);
			}
		}
		
	// second time we search for min;
	double min_error = differences;
	int index_min_error = 0;
	/*
	for (i = 0; i < glen; i++) {
		if (differences[i] < min_error) {
			min_error = differences[i];
			index_min_error = i;
		}
	}
	*/

	// now we check that min_error is not BIG
	if (min_error >= BIG) {
		return -1;
	} else {
		return index_min_error;
	}
}

// save for **gestures
/*
int compareGesture(Point **gestures, int glen, Point *finger_coords)
{
	int delta = 5;
	double BIG = 10000000;

	// build array to hold differences
	double differences[glen];

	// use for loop to check all gestures
	// and to find error deviations
	int i, j;
	double difference_x;
	double difference_y;
	for (i = 0; i < glen; i++) {
		// second for loop to check all fingers in a gesture
		for (j = 0; j < 5; j++) {
			difference_x = abs(gestures[i][j].x - finger_coords[j].x);
		   	difference_y = abs(gestures[i][j].y - finger_coords[j].y);
			
			// now check if meet error tolerances
			if (difference_x > delta || difference_y > delta) {
				differences[i] = BIG;
			} else {
				differences[i] += (difference_x + difference_y);
			}
		}
	}
	
	// second time we search for min;
	double min_error = differences[0];
	int index_min_error = -1;
	for (i = 0; i < glen; i++) {
		if (differences[i] < min_error) {
			min_error = differences[i];
			index_min_error = i;
		}
	}

	// now we check that min_error is not BIG
	if (min_error == BIG) {
		return -1;
	} else {
		return index_min_error;
	}
}
*/

Point *transCoord(Point *coords)
{
	Point *trans = new Point[5];
	// use for loop to move over input coords
	// palm - finger
	int i;
	for (i = 0; i < 5; i++) {
	   trans[i] = coords[5] - coords[i];
	}

	return trans;
}	
