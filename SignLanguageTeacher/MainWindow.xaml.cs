/**
 * @file MainWindow.xaml.cs
 * @brief Main routine for tracking body and color images from the Kinect, as well as gestures.
 * This program tracks up to 6 people simultaneously.
 * If a person is tracked, the associated gesture detector will determine if that person is seated or not.
 * If any of the 6 positions are not in use, the corresponding gesture detector(s) will be paused
 * and the 'Not Tracked' image will be displayed in the UI.
 * @author Microsoft, modified by Vivek Sridhar (viveksri@andrew.cmu.edu)
 * @since 28 February 2015
 */

namespace Microsoft.Samples.Kinect.DiscreteGestureBasics
{
    using System;
    using System.Collections.Generic;
    using System.Collections.ObjectModel;
    using System.ComponentModel;
    using System.Windows;
    using System.Windows.Controls;
    using System.Drawing;

    using Microsoft.Kinect;
    using Microsoft.Kinect.VisualGestureBuilder;

    using OpenCvSharp;
    using OpenCvSharp.UserInterface;
    using OpenCvSharp.CPlusPlus;

    /// Interaction logic for the MainWindow
    public partial class MainWindow : System.Windows.Window, INotifyPropertyChanged
    {
        /// Active Kinect sensor
        private KinectSensor kinectSensor = null;

        /// Array for the bodies (Kinect will track up to 6 people simultaneously)
        private Body[] bodies = null;

        /// Reader for multiple sources
        private MultiSourceFrameReader multiSourceFrameReader = null;

        /// Current status text to display
        private string statusText = null;

        /// KinectBodyView object which handles drawing the Kinect bodies to a View box in the UI
        private KinectBodyView kinectBodyView = null;

        /// List of gesture detectors, there will be one detector created for each potential body (max of 6)
        private List<GestureDetector> gestureDetectorList = null;

        /// Important global coordinates and variables
        private int skeletonAvailable = 0;
        private CameraSpacePoint baseBodyPosition;
        private CameraSpacePoint leftHandPosition;
        private CameraSpacePoint rightHandPosition;
        private ColorSpacePoint leftHandPixelPosition;
        private ColorSpacePoint rightHandPixelPosition;

        /// UI stuff
        private int screenWidth = 1280;

        /// OpenCV Windows
        private CvWindow videoWindow = null;
        private CvWindow instructionWindow = null;
        //private CvWindow leftHandWindow = new CvWindow("Left hand window");
        //private CvWindow rightHandWindow = new CvWindow("Right hand window");

        /// Initializes a new instance of the MainWindow class
        public MainWindow()
        {
            // Only one sensor is currently supported
            this.kinectSensor = KinectSensor.GetDefault();

            // Set IsAvailableChanged event notifier
            this.kinectSensor.IsAvailableChanged += this.Sensor_IsAvailableChanged;

            // Open the sensor
            this.kinectSensor.Open();

            // Set the status text
            this.StatusText = this.kinectSensor.IsAvailable ? Properties.Resources.RunningStatusText
                                                            : Properties.Resources.NoSensorStatusText;

            // Multi Source Frame Reader
            this.multiSourceFrameReader = this.kinectSensor.OpenMultiSourceFrameReader(FrameSourceTypes.Color | FrameSourceTypes.Body);
            this.multiSourceFrameReader.MultiSourceFrameArrived += this.Reader_MultiSourceFrameArrived;

            // Initialize the BodyViewer object for displaying tracked bodies in the UI
            this.kinectBodyView = new KinectBodyView(this.kinectSensor);

            // Initialize the gesture detection objects for our gestures
            this.gestureDetectorList = new List<GestureDetector>();

            // Initialize the MainWindow
            this.InitializeComponent();

            // Set our data context objects for display in UI
            this.DataContext = this;
            this.kinectBodyViewbox.DataContext = this.kinectBodyView;

            // Create a gesture detector for each body (6 bodies => 6 detectors) and create content controls to display results in the UI
            int col0Row = 0;
            int col1Row = 0;
            int maxBodies = this.kinectSensor.BodyFrameSource.BodyCount;
            for (int i = 0; i < maxBodies; ++i)
            {
                GestureResultView result = new GestureResultView(i, false, false, 0.0f);
                GestureDetector detector = new GestureDetector(this.kinectSensor, result);
                this.gestureDetectorList.Add(detector);

                // Split gesture results across the first two columns of the content grid
                ContentControl contentControl = new ContentControl();
                contentControl.Content = this.gestureDetectorList[i].GestureResultView;

                if (i % 2 == 0)
                {
                    // Gesture results for bodies: 0, 2, 4
                    Grid.SetColumn(contentControl, 0);
                    Grid.SetRow(contentControl, col0Row);
                    ++col0Row;
                }
                else
                {
                    // Gesture results for bodies: 1, 3, 5
                    Grid.SetColumn(contentControl, 1);
                    Grid.SetRow(contentControl, col1Row);
                    ++col1Row;
                }

                this.contentGrid.Children.Add(contentControl);
            }

            // User interface values
            // Window names and values
	        string welcome_window = "Welcome";
	        string calibration_window = "Calibration";
	        int width = 1280;
	        int height = 720;
	        int kinect_distance = 0; // note need to replace, with actual value;
	        int kinect_optimal_distance = 20;

            // Strings for UI
            string text_1 = "KINECT BASED SIGN LANGUAGE INSTRUCTION";
            string text_2 = "Calibration is needed plese step into the corect location infront of the Kinect";
            string text_3 = "Plese step closer to the Kinect";
            string text_4 = "plese step away from the Kinect";
            string text_5 = "calibration Complete";
            string text_6 = "press any key to continue";
            string text_7 = "MADE BY: Vivek Sridhar, Sean Reidy, Swetlika Tripathy, Jerry Chee";

            Mat welcome_image = new Mat(height, width, MatType.CV_8UC3);
            Mat calibration_image = Mat.Zeros(height, width, MatType.CV_8UC3);

            CvPoint point1 = new CvPoint(275, 30);
            CvPoint point2 = new CvPoint(400, 90);
            CvPoint point3 = new CvPoint(50, 60);

            MyText(welcome_image.ToCvMat(), point1, text_1);
            MyText(welcome_image.ToCvMat(), point2, text_6);
            MyText(welcome_image.ToCvMat(), point3, text_7);

            MyEllipse(welcome_image.ToCvMat(), 90);
            MyEllipse(welcome_image.ToCvMat(), 0);
            MyEllipse(welcome_image.ToCvMat(), 45);
            MyEllipse(welcome_image.ToCvMat(), -45);

            CvMat welcome_imagecv = welcome_image.ToCvMat();
            Cv.ShowImage(welcome_window, welcome_imagecv);
            Cv.WaitKey(0);
            Cv.DestroyWindow(welcome_window);
            this.videoWindow = new CvWindow("Video Window");
            Cv.MoveWindow("Video Window", 0, 0);
            this.instructionWindow = new CvWindow("Hello");
            Cv.MoveWindow("Hello", 1280, 0);
            Mat instructionMat = new Mat();
            instructionMat = Cv2.ImRead("Images/Hello.jpg");
            CvMat instruction = instructionMat.ToCvMat();
            Cv.ShowImage("Hello", instruction);
        }

        /// UI Functions
        void MyLine(CvMat img, CvPoint start, CvPoint end)
        {
            int thickness = 2;
            CvScalar myColor = new CvScalar(0, 0, 0);
            Cv.Line(img, start, end, myColor, thickness, LineType.Link8);
        }
        void MyEllipse(CvMat img, double angle)
        {
            int thickness = 2;
            CvScalar myColor = new CvScalar(255, 0, 0);
            Cv.Ellipse(img, Cv.Point(this.screenWidth / 2, this.screenWidth / 2),
                       Cv.Size(this.screenWidth / 4, this.screenWidth / 16), angle,
                        0, 360, myColor, thickness, LineType.Link8);
        }
        void MyFilledCircle(CvMat img, CvPoint center, int r, int g, int b)
        {
            int thickness = -1;
            CvScalar myColor = new CvScalar(r, g, b);
            Cv.Circle(img, center, this.screenWidth / 32, myColor, thickness, LineType.Link8);
        }
        void MyRectangle(CvMat img, CvPoint center, int width, int height)
        {
            int delta_w = width / 2;
            int delta_h = height / 2;
            CvScalar myColor = new CvScalar(0, 255, 255);
            Cv.Rectangle(img, Cv.Point(center.X - delta_w, center.Y - delta_h),
                Cv.Point(center.X + delta_w, center.Y + delta_h),
                myColor, -1, LineType.Link8);
        }
        void MyText(CvMat img, CvPoint bottom_L, string text)
        {
            CvScalar myColor = new CvScalar(200, 200, 250);
            CvFont font = new CvFont(FontFace.HersheyComplex, 1, 1);
            Cv.PutText(img, text, bottom_L, font, myColor);
        }

        /// INotifyPropertyChangedPropertyChanged event to allow window controls to bind to changeable data
        public event PropertyChangedEventHandler PropertyChanged;

        /// Gets or sets the current status text to display
        public string StatusText
        {
            get
            {
                return this.statusText;
            }

            set
            {
                if (this.statusText != value)
                {
                    this.statusText = value;

                    // Notify any bound elements that the text has changed
                    if (this.PropertyChanged != null)
                    {
                        this.PropertyChanged(this, new PropertyChangedEventArgs("StatusText"));
                    }
                }
            }
        }

        /// Execute shutdown tasks
        private void MainWindow_Closing(object sender, CancelEventArgs e)
        {
            if (this.gestureDetectorList != null)
            {
                // The GestureDetector contains disposable members (VisualGestureBuilderFrameSource and VisualGestureBuilderFrameReader)
                foreach (GestureDetector detector in this.gestureDetectorList)
                {
                    detector.Dispose();
                }

                this.gestureDetectorList.Clear();
                this.gestureDetectorList = null;
            }

            if (this.kinectSensor != null)
            {
                this.kinectSensor.IsAvailableChanged -= this.Sensor_IsAvailableChanged;
                this.kinectSensor.Close();
                this.kinectSensor = null;
            }
        }

        /// Handles the event when the sensor becomes unavailable (e.g. paused, closed, unplugged).
        private void Sensor_IsAvailableChanged(object sender, IsAvailableChangedEventArgs e)
        {
            // On failure, set the status text
            this.StatusText = this.kinectSensor.IsAvailable ? Properties.Resources.RunningStatusText
                                                            : Properties.Resources.SensorNotAvailableStatusText;
        }

        /// Handles the multiple frame data
        private void Reader_MultiSourceFrameArrived(object sender, MultiSourceFrameArrivedEventArgs e)
        {
            var reference = e.FrameReference.AcquireFrame();

            // Body
            using (var frame = reference.BodyFrameReference.AcquireFrame())
            {
                if (frame != null)
                {
                    if (this.bodies == null)
                    {
                        // Creates an array of 6 bodies, which is the max number of bodies that Kinect can track simultaneously
                        this.bodies = new Body[frame.BodyCount];
                    }

                    // The first time GetAndRefreshBodyData is called, Kinect will allocate each Body in the array.
                    // As long as those body objects are not disposed and not set to null in the array,
                    // those body objects will be re-used.
                    frame.GetAndRefreshBodyData(this.bodies);
                }

                if (this.bodies != null)
                {
                    foreach (var body in this.bodies)
                    {
                        if (body.IsTracked)
                        {
                            ulong trackingId = body.TrackingId;
                            // if the current body TrackingId changed, update the corresponding gesture detector with the new value
                            if (trackingId != this.gestureDetectorList[0].TrackingId)
                            {
                                this.gestureDetectorList[0].TrackingId = trackingId;

                                // if the current body is tracked, unpause its detector to get VisualGestureBuilderFrameArrived events
                                // if the current body is not tracked, pause its detector so we don't waste resources trying to get invalid gesture results
                                this.gestureDetectorList[0].IsPaused = trackingId == 0;
                            }

                            this.baseBodyPosition = body.Joints[JointType.SpineMid].Position;
                            this.leftHandPosition = body.Joints[JointType.HandLeft].Position;
                            this.rightHandPosition = body.Joints[JointType.HandRight].Position;

                            //Console.WriteLine("Body Root: ({0, 1}, {1, 1}, {2, 1})", this.baseBodyPosition.X, this.baseBodyPosition.Y, this.baseBodyPosition.Z);
                            //Console.WriteLine("Left Hand: ({0, 1}, {1, 1}, {2, 1})", this.leftHandPosition.X, this.leftHandPosition.Y, this.leftHandPosition.Z);
                            //Console.WriteLine("Right Hand: ({0, 1}, {1, 1}, {2, 1})", this.rightHandPosition.X, this.rightHandPosition.Y, this.rightHandPosition.Z);
                        }
                    }
                }
            }

            // Color
            using (var frame = reference.ColorFrameReference.AcquireFrame())
            {
                if (frame != null)
                {
                    FrameDescription colorFrameDesc = frame.FrameDescription;
                    int height = colorFrameDesc.Height;
                    int width = colorFrameDesc.Width;
                    byte[] colorBuffer = new byte[4 * height * width];
                    frame.CopyConvertedFrameDataToArray(colorBuffer, ColorImageFormat.Bgra);

                    Mat videoFeedMat = new Mat(height, width, MatType.CV_8UC4, colorBuffer);
                    CvMat videoFeed = videoFeedMat.ToCvMat();
                    OpenCvSharp.CPlusPlus.Size size = new OpenCvSharp.CPlusPlus.Size(1280, 720);
                    Mat resizedVideoFeedMat = new Mat();
                    Cv2.Resize(videoFeedMat, resizedVideoFeedMat, size);
                    CvMat resizedVideoFeed = resizedVideoFeedMat.ToCvMat();
                    if (this.videoWindow != null)
                    {
                        Cv.ShowImage("Video Window", resizedVideoFeed);
                    }

                    // Transform hand coordinates to pixels
                    ColorSpacePoint[] pixelPositions = new ColorSpacePoint[2];
                    pixelPositions[0] = this.leftHandPixelPosition;
                    pixelPositions[1] = this.rightHandPixelPosition;
                    CameraSpacePoint[] cameraPositions = new CameraSpacePoint[2];
                    cameraPositions[0] = this.leftHandPosition;
                    cameraPositions[1] = this.rightHandPosition;
                    this.kinectSensor.CoordinateMapper.MapCameraPointsToColorSpace(cameraPositions, pixelPositions);
                    // Update global values
                    this.leftHandPixelPosition = pixelPositions[0];
                    this.rightHandPixelPosition = pixelPositions[1];

                    //Console.WriteLine("Left Hand Pixels: ({0}, {1})", this.leftHandPixelPosition.X, this.leftHandPixelPosition.Y);
                    //Console.WriteLine("Right Hand Pixels: ({0}, {1})", this.rightHandPixelPosition.X, this.rightHandPixelPosition.Y);

                    // Construct region of interest around the hands
                    /*
                    int handDelta = 100;
                    OpenCvSharp.CPlusPlus.Rect leftHandROI = new OpenCvSharp.CPlusPlus.Rect((int)this.leftHandPixelPosition.X - handDelta, (int)this.leftHandPixelPosition.Y - handDelta, 2 * handDelta, 2 * handDelta);
                    Mat videoFeedCopy1 = videoFeedMat.Clone();
                    Mat leftHandMat = new Mat(videoFeedCopy1, leftHandROI);
                    CvMat leftHand = leftHandMat.ToCvMat();
                    if (leftHand != null)
                    {
                        Cv.ShowImage("Left hand window", leftHand);
                    }

                    OpenCvSharp.CPlusPlus.Rect rightHandROI = new OpenCvSharp.CPlusPlus.Rect((int)this.rightHandPixelPosition.X - handDelta, (int)this.rightHandPixelPosition.Y - handDelta, 2 * handDelta, 2 * handDelta);
                    Mat videoFeedCopy2 = videoFeedMat.Clone();
                    Mat rightHandMat = new Mat(videoFeedCopy2, rightHandROI);
                    CvMat rightHand = rightHandMat.ToCvMat();
                    if (rightHand != null)
                    {
                        //Cv.ShowImage("Right hand window", rightHand);
                    }
                     */
                }
            }
        }
    }
}
