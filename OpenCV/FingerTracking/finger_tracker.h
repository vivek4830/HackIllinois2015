// finger_tracker.h

#include <iostream>
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/photo.hpp"

using namespace cv;
using namespace std;

// filters or "threshes" input Mat pasted on HSV range to look into
Mat getThresholdedImage(Mat imgOriginal, Scalar low,Scalar high);

// get central point of threshed Mat
Point getPoint(Mat imgThresholded);

// saves 6 Points to file.
// Convention: palm, thumb, index, middle, ring, pinky
void saveGesture(Point *gesture_coordinates, FILE *F);

// reads from text file, 
// gives 6 Points array
Point *readGesture(FILE *F);

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
		Point *ring_coords, Point *pinky_coords);
