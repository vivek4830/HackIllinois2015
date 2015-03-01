// 2evidence_finger_tracker.cpp

#include <iostream>
#include <fstream>
#include <stdio.h>
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/photo.hpp"

#include "finger_tracker.h"

using namespace cv; 
using namespace std; 

void evidence_saveGesture()
{
	FILE *F = fopen("~/Desktop/HackIllinois2015/OpenCV/FingerTracking/test1.txt", "w+");
	Point *points = new Point [5];
	
	// populate points with zeros
	int i;
	for (i = 0; i < 5; i++) {
		points[i].x = 0;
		points[i].y = 0;
	}

	saveGesture(points, F);
	//fclose(F);
	//free(points);
}


int main()
{
	// tests here
	evidence_saveGesture();

	return 0;
}


