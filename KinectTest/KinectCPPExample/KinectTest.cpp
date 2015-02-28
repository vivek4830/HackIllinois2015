/**
 * @file KinectTest.cpp
 * @brief Implements the Kinect/OpenCV test
 * @author Vivek Sridhar (viveksri@andrew.cmu.edu)
 * @since 28 February 2015
 */

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include "KinectTest.h"

using namespace cv;
using namespace std;

/// Main function of the program - starts the Kinect processes
int main(int argc, char** argv)
{
	// Creates window for the color frame data
	namedWindow("Display window", WINDOW_NORMAL);

	// Starts receiving and processing data from the Kinect
	KinectTest application;
	application.Run();
}


/// Constructor
KinectTest::KinectTest() :
	m_pKinectSensor(NULL),
	m_pMultiSourceFrameReader(NULL) {
	InitializeDefaultSensor();
}

/// Destructor
KinectTest::~KinectTest() {
	if (m_pKinectSensor) {
		m_pKinectSensor->Close();
	}
	SafeRelease(m_pKinectSensor);
}

/// Main run function
int KinectTest::Run() {
	MSG msg = { 0 };

	while (WM_QUIT != msg.message) {
		Update();
	}

	return static_cast<int>(msg.wParam);
}

/// Main processing function
void KinectTest::Update() {
	HRESULT hr;

	if (!m_pMultiSourceFrameReader) {
		return;
	}

	// Multisource frame information
	IMultiSourceFrame* pMultiSourceFrame = NULL;
	
	// Body frame information
	IBodyFrameReference* pBodyFrameReference = NULL;
	IBodyFrame* pBodyFrame = NULL;
	IBody* ppBodies[BODY_COUNT] = { 0 };

	// Color frame information
	IColorFrameReference* pColorFrameReference = NULL;
	IColorFrame* pColorFrame = NULL;
	IFrameDescription* frameDesc = NULL;
	RGBQUAD* colorBuffer = NULL;

	// Receives the latest Multisource frame
	hr = m_pMultiSourceFrameReader->AcquireLatestFrame(&pMultiSourceFrame);

	if (SUCCEEDED(hr)) {
		// Gets body frame via references from MultiSource frame
		hr = pMultiSourceFrame->get_BodyFrameReference(&pBodyFrameReference);
		if (SUCCEEDED(hr)) {
			hr = pBodyFrameReference->AcquireFrame(&pBodyFrame);
			if (SUCCEEDED(hr)) {
				// Stores data from body frame into ppBodies
				hr = pBodyFrame->GetAndRefreshBodyData(_countof(ppBodies), ppBodies);
				if (SUCCEEDED(hr)) {
					// Processes data in current body frame
					ProcessBody(BODY_COUNT, ppBodies);
				}
				// Cleans up stored body frame data
				for (int i = 0; i < _countof(ppBodies); ++i) {
					SafeRelease(ppBodies[i]);
				}
			}
		}
		// Get color frame via references from Multisource frame
		hr = pMultiSourceFrame->get_ColorFrameReference(&pColorFrameReference);
		if (SUCCEEDED(hr)) {
			hr = pColorFrameReference->AcquireFrame(&pColorFrame);
		}
		if (SUCCEEDED(hr)) {
			// Gets color frame information and constructs the image
			pColorFrame->get_FrameDescription(&frameDesc);
			if (SUCCEEDED(hr)) {
				int height = 0, width = 0;
				if (SUCCEEDED(frameDesc->get_Height(&height)) &&
					SUCCEEDED(frameDesc->get_Width(&width))) {
					colorBuffer = new RGBQUAD[height * width];
					hr = pColorFrame->CopyConvertedFrameDataToArray(height * width * sizeof(RGBQUAD),
						reinterpret_cast<BYTE*>(colorBuffer), ColorImageFormat_Bgra);
					if (SUCCEEDED(hr)) {
						// Creates an OpenCV image from the processed color frame
						Mat image(height, width, CV_8UC4,
							reinterpret_cast<void*>(colorBuffer));
						Mat resizedImage;
						// Displays the image
						imshow("Display window", image);
						resizeWindow("Display window", 1280, 720);
						moveWindow("Display window", 0, 0);
						waitKey(1);
					}
				}
			}
		}
		// Frees the last color frame
		if (colorBuffer != NULL) {
			delete[] colorBuffer;
			colorBuffer = NULL;
		}
	}
	
	// Cleans up all the frame data
	SafeRelease(pMultiSourceFrame);
	SafeRelease(pBodyFrameReference);
	SafeRelease(pColorFrameReference);
	SafeRelease(pBodyFrame);
	SafeRelease(pColorFrame);
}

/// Initializes the default Kinect sensor
HRESULT KinectTest::InitializeDefaultSensor() {
	HRESULT hr;

	hr = GetDefaultKinectSensor(&m_pKinectSensor);
	if (FAILED(hr)) {
		return hr;
	}

	if (m_pKinectSensor) {
		// Initialize the Kinect and get the multi-source frame reader (for Body and Color frames)
		hr = m_pKinectSensor->Open();

		if (SUCCEEDED(hr)) {
			hr = m_pKinectSensor->OpenMultiSourceFrameReader(FrameSourceTypes_Color | 
				FrameSourceTypes_Body, &m_pMultiSourceFrameReader);
		}
	}

	if (!m_pKinectSensor || FAILED(hr)) {
		cout << "No ready kinect found!\n";
		return E_FAIL;
	}

	return hr;
}

/// Handle new body data - Detects and prints out the positions of the centers of each hand
void KinectTest::ProcessBody(int nBodyCount, IBody** ppBodies) {
	HRESULT hr;

	for (int i = 0; i < nBodyCount; ++i) {
		IBody* pBody = ppBodies[i];
		Joint joints[JointType_Count];

		hr = pBody->GetJoints(_countof(joints), joints);
		if (SUCCEEDED(hr)) {
			for (int j = 0; j < _countof(joints); ++j) {
				if ((joints[j].JointType == JointType_HandLeft) &&
					(joints[j].TrackingState == TrackingState_Tracked)) {
					float x = joints[j].Position.X;
					float y = joints[j].Position.Y;
					float z = joints[j].Position.Z;
					cout << "Left Hand Coordinates: (" << x << "," << y << "," << z << ")\n";
				}
				if ((joints[j].JointType == JointType_HandRight) &&
					(joints[j].TrackingState == TrackingState_Tracked)) {
					float x = joints[j].Position.X;
					float y = joints[j].Position.Y;
					float z = joints[j].Position.Z;
					cout << "Right Hand Coordinates: (" << x << "," << y << "," << z << ")\n";
				}
			}
		}
	}
}