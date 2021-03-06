#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <thread>


#define w 1280

using namespace cv;
using namespace std;

void MyLine(Mat img, Point start, Point end)
{
	int thickness = 2;
	int lineType = 8;
	line(img,
		start,
		end,
		Scalar(0, 0, 0),
		thickness,
		lineType);
}

void MyEllipse(Mat img, double angle)
{
	int thickness = 2;
	int lineType = 8;

	ellipse(img,
		Point(w / 2.0, w / 2.0),
		Size(w / 4.0, w / 16.0),
		angle,
		0,
		360,
		Scalar(255, 0, 0),
		thickness,
		lineType);
}

void MyFilledCircle(Mat img, Point center,int r, int g, int b)
{
	int thickness = -1;
	int lineType = 8;

	circle(img,
		center,
		w / 32.0,
		Scalar(r, g, b),
		thickness,
		lineType);
}

void MyRectangel(Mat img, Point center, int width, int height )
{
	int thickness = 3;
	int lineType = 8;
	int delta_w = width / 2;
	int delta_h = height / 2;


	rectangle(img,
		Point(center.x - delta_w , center.y - delta_h),
		Point(center.x +delta_w, center.y + delta_h),
		Scalar(0, 255, 255),
		-1,
		8);
	

}

void MyText(Mat img, Point bottom_L,string text )
{
	putText(img, text, bottom_L, CV_FONT_HERSHEY_TRIPLEX, 0.8, cvScalar(200, 200, 250), 1, CV_AA);

}

//old test code ignore 
/*
void MyPolygon(Mat img)
{
	int lineType = 8;

	
	Point rook_points[1][20];
	rook_points[0][0] = Point(w / 4.0, 7 * w / 8.0);
	rook_points[0][1] = Point(3 * w / 4.0, 7 * w / 8.0);
	rook_points[0][2] = Point(3 * w / 4.0, 13 * w / 16.0);
	rook_points[0][3] = Point(11 * w / 16.0, 13 * w / 16.0);
	rook_points[0][4] = Point(19 * w / 32.0, 3 * w / 8.0);
	rook_points[0][5] = Point(3 * w / 4.0, 3 * w / 8.0);
	rook_points[0][6] = Point(3 * w / 4.0, w / 8.0);
	rook_points[0][7] = Point(26 * w / 40.0, w / 8.0);
	rook_points[0][8] = Point(26 * w / 40.0, w / 4.0);
	rook_points[0][9] = Point(22 * w / 40.0, w / 4.0);
	rook_points[0][10] = Point(22 * w / 40.0, w / 8.0);
	rook_points[0][11] = Point(18 * w / 40.0, w / 8.0);
	rook_points[0][12] = Point(18 * w / 40.0, w / 4.0);
	rook_points[0][13] = Point(14 * w / 40.0, w / 4.0);
	rook_points[0][14] = Point(14 * w / 40.0, w / 8.0);
	rook_points[0][15] = Point(w / 4.0, w / 8.0);
	rook_points[0][16] = Point(w / 4.0, 3 * w / 8.0);
	rook_points[0][17] = Point(13 * w / 32.0, 3 * w / 8.0);
	rook_points[0][18] = Point(5 * w / 16.0, 13 * w / 16.0);
	rook_points[0][19] = Point(w / 4.0, 13 * w / 16.0);

	const Point* ppt[1] = { rook_points[0] };
	int npt[] = { 20 };

	fillPoly(img,
		ppt,
		npt,
		1,
		Scalar(255, 255, 255),
		lineType);
}
*/
/*
int main()
{
	//window name 
	char atom_window[] = "Drawing 1: Atom";
	char rook_window[] = "Drawing 2: Rook";

	//Creation of mat emty images
	Mat atom_image = Mat::zeros(w, w, CV_8UC3);
	Mat rook_image = Mat::zeros(w, w, CV_8UC3);

	string test = "this is a test";
	string test_b = " this is also a test";

	MyText(atom_image, Point(w / 2, w / 2), test);
	MyText(rook_image, Point((w / 2) + 10, (w / 2) + 10 ), test_b);



	/*attom drawing 
	MyEllipse(atom_image, 90);
	MyEllipse(atom_image, 0);
	MyEllipse(atom_image, 45);
	MyEllipse(atom_image, -45);
	MyFilledCircle(atom_image, Point(w / 2.0, w / 2.0));

	//Rook
	//MyPolygon(rook_image);

	/// 2.b. Creating rectangles
	rectangle(rook_image,
		Point(0, 7 * w / 8.0),
		Point(w, w),
		Scalar(0, 255, 255),
		-1,
		8);

	/// 2.c. Create a few lines
	MyLine(rook_image, Point(0, 15 * w / 16), Point(w, 15 * w / 16));
	MyLine(rook_image, Point(w / 4, 7 * w / 8), Point(w / 4, w));
	MyLine(rook_image, Point(w / 2, 7 * w / 8), Point(w / 2, w));
	MyLine(rook_image, Point(3 * w / 4, 7 * w / 8), Point(3 * w / 4, w));

	imshow(atom_window, atom_image);
	moveWindow(atom_window, 0, 200);
	imshow(rook_window, rook_image);
	moveWindow(rook_window, w, 200);

	waitKey(0);
	return(0);


}
*/

int main()
{
	//window names and values
	char welcome_window[] = "Welcome";
	char calibration_window[] = "Calibration";
	int width = 1280;
	int height = 720;
	int kinect_distance = 0; // note need to replace, with actual value;
	int kinect_optimal_distance = 20;

	//Strings for UI
	string text_1 = "KINECT BASED SIGN LANGUAGE INSTRUCTION";
	string text_2 = "Calibration is needed plese step into the corect location infront of the Kinect";
	string text_3 = "Plese step closer to the Kinect";
	string text_4 = "plese step away from the Kinect";
	string text_5 = "calibration Complete";
	string text_6 = "press any key to continue";
	string text_7 = "MADE BY: Vivek Sridhar, Sean Reidy, Swetlika Tripathy, Jerry Chee";
	


	// create window, and image constuction
	Mat welcome_image = Mat::zeros(height, width, CV_8UC3);
	//EVIL LINES 

	//cv::Mat welcome_image = cv::imread("C:\\Users\\Sean\\Documents\\GitHub\\HackIllinois2015\\OpenCV\\UserInterface\\one.jpeg");
	Mat calibration_image = Mat::zeros(height, width, CV_8UC3);
	MyText(welcome_image, Point(height/2 ,50), text_1);
	MyText(welcome_image, Point(height /2, 70), text_6);
	MyText(welcome_image, Point(height / 2-100, 100), text_7);
	MyText(calibration_image, Point(30, 30),text_5);
	MyText(calibration_image, Point(30, 45), text_6);

	MyEllipse(welcome_image, 90);
	MyEllipse(welcome_image, 0);
	MyEllipse(welcome_image, 45);
	MyEllipse(welcome_image, -45);
	//MyFilledCircle(welcome_image, Point(width/ 2.0, height / 2.0));



	//UI FLOW
	//imshow(welcome_window, welcome_image);
	for (int i = 0; i <= 1000; i++){
		rectangle(welcome_image, Point(i * 5, i*10), Point((i * 5) + 5, 5 *i), Scalar(0, 55, 255), CV_FILLED, 4);
		imshow(welcome_window, welcome_image);
		std::this_thread::sleep_for(chrono::seconds(1/100));
	}
	waitKey(0);
	destroyWindow(welcome_window);
	printf("Calibration is needed plese step into the corect location infront of the Kinect\n");
	std::this_thread::sleep_for(chrono::seconds(5));
	printf("Calibration Start\n");

	// Calibration change values later 
	bool Calibration_complete = false;
	while (!Calibration_complete){
		std::this_thread::sleep_for(chrono::seconds(2));
		kinect_distance++;
		if (kinect_distance < kinect_optimal_distance - 10){
			printf("plese step away from the Kinect\n");
		}
		else if (kinect_distance > kinect_optimal_distance + 10){
			printf("Plese step closer to the Kinect\n");
		}
		else{
			printf("Stand still!");
			std::this_thread::sleep_for(chrono::seconds(4));
			Calibration_complete = true;
		}
	}
	imshow(calibration_window, calibration_image);
	waitKey(0);
	

	//Add program fuctions here 
	
	return(0);

}