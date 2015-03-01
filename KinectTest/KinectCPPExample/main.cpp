/**
 * @file main.cp
 * @brief Implements the main routine for the Kinect test program.
 * @author Vivek Sridhar (viveksri@andrew.cmu.edu)
 * @since 28 February 2015
 */

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "KinectManager.h"

using namespace cv;

/// Main function of the program - starts the Kinect processes
int main(int argc, char** argv)
{
	// Creates window for the color frame data
	namedWindow("Display window", WINDOW_NORMAL);

	// Starts receiving and processing data from the Kinect
	KinectManager application;
	application.Run();
}