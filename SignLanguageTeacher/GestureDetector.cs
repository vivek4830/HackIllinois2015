/**
 * @file GestureDetector.cs
 * @brief This file implements the gesture recognition and processing
 * from the Kinect.
 * @author Microsoft, modified by Vivek Sridhar (viveksri@andrew.cmu.edu)
 * @since 28 February 2015
 */

namespace Microsoft.Samples.Kinect.DiscreteGestureBasics
{
    using System;
    using System.Windows.Forms;
    using System.Threading;
    using System.Collections.Generic;
    using Microsoft.Kinect;
    using Microsoft.Kinect.VisualGestureBuilder;

    using OpenCvSharp;
    using OpenCvSharp.UserInterface;
    using OpenCvSharp.CPlusPlus;

    /// Gesture Detector class which listens for VisualGestureBuilderFrame events from the service
    /// and updates the associated GestureResultView object with the latest results for the 'Seated' gesture
    public class GestureDetector : IDisposable
    {
        /// Path to the gesture database that was trained with VGB
        private readonly string gestureDatabase = "Database\\SignLanguage.gbd";

        /// Name of the discrete gesture in the database that we want to track
        private readonly string helloGestureName = "Hello_Right";
        private readonly string goodbyeGestureName = "Goodbye_Right";
        private readonly string thankYouGestureName = "ThankYou";
        private readonly string momGestureName = "Mom";

        private int order = 0;

        /// Gesture frame source which should be tied to a body tracking ID
        private VisualGestureBuilderFrameSource vgbFrameSource = null; 

        /// Gesture frame reader which will handle gesture events coming from the sensor
        private VisualGestureBuilderFrameReader vgbFrameReader = null;

        /// Initializes a new instance of the GestureDetector class along with the gesture frame source and reader
        public GestureDetector(KinectSensor kinectSensor, GestureResultView gestureResultView)
        {
            if (kinectSensor == null)
            {
                throw new ArgumentNullException("kinectSensor");
            }

            if (gestureResultView == null)
            {
                throw new ArgumentNullException("gestureResultView");
            }

            this.GestureResultView = gestureResultView;

            // Create the vgb source. The associated body tracking ID will be set when a valid body frame arrives from the sensor.
            this.vgbFrameSource = new VisualGestureBuilderFrameSource(kinectSensor, 0);
            this.vgbFrameSource.TrackingIdLost += this.Source_TrackingIdLost;

            // Open the reader for the vgb frames
            this.vgbFrameReader = this.vgbFrameSource.OpenReader();
            if (this.vgbFrameReader != null)
            {
                this.vgbFrameReader.IsPaused = true;
                this.vgbFrameReader.FrameArrived += this.Reader_GestureFrameArrived;
            }

            // Load the 'Hello' gesture from the gesture database
            using (VisualGestureBuilderDatabase database = new VisualGestureBuilderDatabase(this.gestureDatabase))
            {
                // We could load all available gestures in the database with a call to vgbFrameSource.AddGestures(database.AvailableGestures)
                foreach (Gesture gesture in database.AvailableGestures)
                {
                    if (gesture.Name.Equals(this.helloGestureName))
                    {
                        this.vgbFrameSource.AddGesture(gesture);
                    }
                    if (gesture.Name.Equals(this.goodbyeGestureName))
                    {
                        this.vgbFrameSource.AddGesture(gesture);
                    }
                    if (gesture.Name.Equals(this.thankYouGestureName))
                    {
                        this.vgbFrameSource.AddGesture(gesture);
                    }
                    if (gesture.Name.Equals(this.momGestureName)) {
                        this.vgbFrameSource.AddGesture(gesture);
                    }
                }
            }
        }

        /// Gets the GestureResultView object which stores the detector results for display in the UI
        public GestureResultView GestureResultView { get; private set; }

        /// Gets or sets the body tracking ID associated with the current detector
        /// The tracking ID can change whenever a body comes in/out of scope
        public ulong TrackingId
        {
            get
            {
                return this.vgbFrameSource.TrackingId;
            }

            set
            {
                if (this.vgbFrameSource.TrackingId != value)
                {
                    this.vgbFrameSource.TrackingId = value;
                }
            }
        }

        /// Gets or sets a value indicating whether or not the detector is currently paused
        /// If the body tracking ID associated with the detector is not valid, then the detector should be paused
        public bool IsPaused
        {
            get
            {
                return this.vgbFrameReader.IsPaused;
            }

            set
            {
                if (this.vgbFrameReader.IsPaused != value)
                {
                    this.vgbFrameReader.IsPaused = value;
                }
            }
        }

        /// Disposes all unmanaged resources for the class
        public void Dispose()
        {
            this.Dispose(true);
            GC.SuppressFinalize(this);
        }

        /// Disposes the VisualGestureBuilderFrameSource and VisualGestureBuilderFrameReader objects
        protected virtual void Dispose(bool disposing)
        {
            if (disposing)
            {
                if (this.vgbFrameReader != null)
                {
                    this.vgbFrameReader.FrameArrived -= this.Reader_GestureFrameArrived;
                    this.vgbFrameReader.Dispose();
                    this.vgbFrameReader = null;
                }

                if (this.vgbFrameSource != null)
                {
                    this.vgbFrameSource.TrackingIdLost -= this.Source_TrackingIdLost;
                    this.vgbFrameSource.Dispose();
                    this.vgbFrameSource = null;
                }
            }
        }

        /// Handles gesture detection results arriving from the sensor for the associated body tracking ID
        private void Reader_GestureFrameArrived(object sender, VisualGestureBuilderFrameArrivedEventArgs e)
        {
            VisualGestureBuilderFrameReference frameReference = e.FrameReference;
            using (VisualGestureBuilderFrame frame = frameReference.AcquireFrame())
            {
                if (frame != null)
                {
                    // Get the discrete gesture results which arrived with the latest frame
                    IReadOnlyDictionary<Gesture, DiscreteGestureResult> discreteResults = frame.DiscreteGestureResults;

                    if (discreteResults != null)
                    {
                        // We only have one gesture in this source object, but you can get multiple gestures
                        foreach (Gesture gesture in this.vgbFrameSource.Gestures)
                        {
                            if (gesture.Name.Equals(this.helloGestureName) && gesture.GestureType == GestureType.Discrete && this.order == 0)
                            {
                                DiscreteGestureResult result = null;
                                discreteResults.TryGetValue(gesture, out result);

                                if (result != null)
                                {
                                    // Update the GestureResultView object with new gesture result values
                                    this.GestureResultView.UpdateGestureResult(true, result.Detected, result.Confidence);
                                    if (result.Detected)
                                    {
                                        Cv.DestroyWindow("Hello");
                                        Cv.WaitKey(0);
                                        CvWindow goodbyeWindow = new CvWindow("Goodbye");
                                        Cv.MoveWindow("Goodbye", 1280, 0);
                                        Mat instructionMat = new Mat();
                                        instructionMat = Cv2.ImRead("Images/Goodbye.jpg");
                                        CvMat instruction = instructionMat.ToCvMat();
                                        Cv.ShowImage("Goodbye", instruction);
                                        this.order = 1;
                                    }
                                }
                            }
                            if (gesture.Name.Equals(this.goodbyeGestureName) && gesture.GestureType == GestureType.Discrete && this.order == 1)
                            {
                                DiscreteGestureResult result = null;
                                discreteResults.TryGetValue(gesture, out result);

                                if (result != null)
                                {
                                    // Update the GestureResultView object with new gesture result values
                                    this.GestureResultView.UpdateGestureResult(true, result.Detected, result.Confidence);
                                    if (result.Detected)
                                    {
                                        Cv.DestroyWindow("Goodbye");
                                        Cv.WaitKey(0);
                                        CvWindow thankYouWindow = new CvWindow("Thank You");
                                        Cv.MoveWindow("Thank You", 1280, 0);
                                        Mat instructionMat = new Mat();
                                        instructionMat = Cv2.ImRead("Images/ThankYou.jpg");
                                        CvMat instruction = instructionMat.ToCvMat();
                                        Cv.ShowImage("Thank You", instruction);
                                        this.order = 2;
                                    }
                                }
                            }
                            if (gesture.Name.Equals(this.thankYouGestureName) && gesture.GestureType == GestureType.Discrete && this.order == 2)
                            {
                                DiscreteGestureResult result = null;
                                discreteResults.TryGetValue(gesture, out result);

                                if (result != null)
                                {
                                    // Update the GestureResultView object with new gesture result values
                                    this.GestureResultView.UpdateGestureResult(true, result.Detected, result.Confidence);
                                    if (result.Detected)
                                    {
                                        Cv.DestroyWindow("Thank You");
                                        Cv.WaitKey(0);
                                        CvWindow momWindow = new CvWindow("Mom");
                                        Cv.MoveWindow("Mom", 1280, 0);
                                        Mat instructionMat = new Mat();
                                        instructionMat = Cv2.ImRead("Images/Mom.jpg");
                                        CvMat instruction = instructionMat.ToCvMat();
                                        Cv.ShowImage("Mom", instruction);
                                        this.order = 3;
                                    }
                                }
                            }
                            if (gesture.Name.Equals(this.momGestureName) && gesture.GestureType == GestureType.Discrete && this.order == 3)
                            {
                                DiscreteGestureResult result = null;
                                discreteResults.TryGetValue(gesture, out result);

                                if (result != null)
                                {
                                    // Update the GestureResultView object with new gesture result values
                                    this.GestureResultView.UpdateGestureResult(true, result.Detected, result.Confidence);
                                    if (result.Detected)
                                    {
                                        Cv.DestroyWindow("Mom");
                                        Cv.WaitKey(0);
                                        CvWindow helloWindow = new CvWindow("Hello");
                                        Cv.MoveWindow("Hello", 1280, 0);
                                        Mat instructionMat = new Mat();
                                        instructionMat = Cv2.ImRead("Images/Hello.jpg");
                                        CvMat instruction = instructionMat.ToCvMat();
                                        Cv.ShowImage("Hello", instruction);
                                        this.order = 0;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        /// Handles the TrackingIdLost event for the VisualGestureBuilderSource object
        private void Source_TrackingIdLost(object sender, TrackingIdLostEventArgs e)
        {
            // Update the GestureResultView object to show the 'Not Tracked' image in the UI
            this.GestureResultView.UpdateGestureResult(false, false, 0.0f);
        }
    }
}
