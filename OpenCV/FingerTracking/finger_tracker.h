// finger_tracker.h

#include <iostream>
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/photo.hpp"

using namespace cv;
using namespace std;

// define gestures
//int glen = 1;
//Point **gestures;
//Point *high_five = new Point [5];
//gestures[0] = new Point [5];
/*
Point *high_five = new Point [5];
high_five[0] = Point(-82,-4);
high_five[1] = Point(-63, 84);
high_five[2] = Point(-37, 103);
high_five[3] = Point(1, 112);
high_five[4] = Point(32, 76);
*/

// filters or "threshes" input Mat pasted on HSV range to look into
Mat getThresholdedImage(Mat imgOriginal, Scalar low,Scalar high);

// get central point of threshed Mat
Point getPoint(Mat imgThresholded);

// saves 6 Points to file.
// Convention: palm, thumb, index, middle, ring, pinky
void printGesture(Point *gesture_coordinates);

// reads from text file, 
// gives 6 Points array
// Point *readGesture(FILE *F);

// finds average Point of palm
Point avgPalm(Point *palm_coordinates, int len);

// takes an array of Points relative to one
// finger and palm. Outputs the average.
Point avgFinger(Point *finger_coordinates, 
		Point palm_avg, int len);

// takes in arrays for palm and 5 digits,
// outputs array of 6 Points, averages
Point *avgGesture(Point *palm_coords, Point *thumb_coords, 
		Point *index_coords, Point *middle_coords, 
		Point *ring_coords, Point *pinky_coords, int alen);

// given 6 points from Mat, transforms the 
// finger coordinates to coordinate system relative
// to palm
// convention: thumb, index ... palm
Point *transCoord(Point *coords);

// gives error margin when between
// the string defines which gesture
int compareGesture(Point **gestures, int glen, Point *finger_coords);

