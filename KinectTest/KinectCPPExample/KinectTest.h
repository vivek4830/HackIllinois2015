/**
 * @file KinectTest.h
 * @brief Defines a class for testing the Kinect and OpenCV
 * @author Vivek Sridhar (viveksri@andrew.cmu.edu)
 * @since 28 February 2015
 */

#include <Windows.h>
#include <Kinect.h>

/// Class KinectTest
/// Holds the information necessary to stream and process data
/// from the Kinect device.
class KinectTest {
public:
	// Constructor
	KinectTest();
	
	// Destructor
	~KinectTest();

	// Starts the program and begins processing
	int Run();

private:
	// Current Kinect
	IKinectSensor* m_pKinectSensor;

	// Global frame reader
	IMultiSourceFrameReader* m_pMultiSourceFrameReader;

	// Main processing function
	void Update();

	// Initializes the default Kinect sensor
	HRESULT InitializeDefaultSensor();

	// Handle new body data
	void ProcessBody(int nBodyCount, IBody** ppbodies);
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