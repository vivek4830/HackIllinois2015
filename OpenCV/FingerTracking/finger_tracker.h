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


