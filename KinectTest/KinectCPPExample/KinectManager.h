/**
 * @file KinectManager.h
 * @brief Defines a class for testing the Kinect and OpenCV
 * @author Vivek Sridhar (viveksri@andrew.cmu.edu)
 * @since 28 February 2015
 */

#ifndef KINECT_MANAGER_H_
#define KINECT_MANAGER_H_

#include <Windows.h>
#include <Kinect.h>
#include <Kinect.VisualGestureBuilder.h>

/// Class KinectManager
/// Holds the information necessary to stream and process data
/// from the Kinect device.
class KinectManager {
public:
	// Constructor
	KinectManager();
	
	// Destructor
	~KinectManager();

	// Starts the program and begins processing
	int Run();

private:
	// Current Kinect
	IKinectSensor* m_pKinectSensor;

	// Global frame reader
	IMultiSourceFrameReader* m_pMultiSourceFrameReader;

	// Gesture detector
	IVisualGestureBuilderFrameReader* m_pVisualGestureBuilderFrameReader;

	// Gesture information
	IVisualGestureBuilderDatabase* m_pGestureDatabase;
	//std::string m_pSeatedGestureName;

	// Main processing function
	void Update();

	// Initializes the default Kinect sensor
	HRESULT InitializeDefaultSensor();

	// Handle new body data
	UINT64 ProcessBody(int nBodyCount, IBody** ppbodies);
};

/// Safe release for interfaces
template<class Interface>
inline void SafeRelease(Interface *& pInterfaceToRelease)
{
	if (pInterfaceToRelease != NULL)
	{
		pInterfaceToRelease->Release();
		pInterfaceToRelease = NULL;
	}
}
#endif