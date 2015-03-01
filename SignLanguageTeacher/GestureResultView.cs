/**
 * @file GestureResultView.cs
 * @brief Implements the view for the gestures and whether they are detected or not
 * @author Microsoft
 * @since 28 February 2015
 */

namespace Microsoft.Samples.Kinect.DiscreteGestureBasics
{
    using System;
    using System.ComponentModel;
    using System.Runtime.CompilerServices;
    using System.Windows.Media;
    using System.Windows.Media.Imaging;

    /// Stores discrete gesture results for the GestureDetector.
    /// Properties are stored/updated for display in the UI.
    public sealed class GestureResultView : INotifyPropertyChanged
    {
        /// Image to show when the 'detected' property is true for a tracked body
        private readonly ImageSource seatedImage = new BitmapImage(new Uri(@"Images\Seated.png", UriKind.Relative));

        /// Image to show when the 'detected' property is false for a tracked body
        private readonly ImageSource notSeatedImage = new BitmapImage(new Uri(@"Images\NotSeated.png", UriKind.Relative));

        /// Image to show when the body associated with the GestureResultView object is not being tracked
        private readonly ImageSource notTrackedImage = new BitmapImage(new Uri(@"Images\NotTracked.png", UriKind.Relative));

        /// Array of brush colors to use for a tracked body; array position corresponds to the body colors used in the KinectBodyView class
        private readonly Brush[] trackedColors = new Brush[] { Brushes.Red, Brushes.Orange, Brushes.Green, Brushes.Blue, Brushes.Indigo, Brushes.Violet };

        /// Brush color to use as background in the UI
        private Brush bodyColor = Brushes.Gray;

        /// The body index (0-5) associated with the current gesture detector
        private int bodyIndex = 0;

        /// Current confidence value reported by the discrete gesture
        private float confidence = 0.0f;

        /// True, if the discrete gesture is currently being detected
        private bool detected = false;

        /// Image to display in UI which corresponds to tracking/detection state
        private ImageSource imageSource = null;

        /// True, if the body is currently being tracked
        private bool isTracked = false;

        /// Initializes a new instance of the GestureResultView class and sets initial property values
        public GestureResultView(int bodyIndex, bool isTracked, bool detected, float confidence)
        {
            this.BodyIndex = bodyIndex;
            this.IsTracked = isTracked;
            this.Detected = detected;
            this.Confidence = confidence;
            this.ImageSource = this.notTrackedImage;
        }

        /// INotifyPropertyChangedPropertyChanged event to allow window controls to bind to changeable data
        public event PropertyChangedEventHandler PropertyChanged;

        /// Gets the body index associated with the current gesture detector result 
        public int BodyIndex
        {
            get
            {
                return this.bodyIndex;
            }

            private set
            {
                if (this.bodyIndex != value)
                {
                    this.bodyIndex = value;
                    this.NotifyPropertyChanged();
                }
            }
        }

        /// Gets the body color corresponding to the body index for the result
        public Brush BodyColor
        {
            get
            {
                return this.bodyColor;
            }

            private set
            {
                if (this.bodyColor != value)
                {
                    this.bodyColor = value;
                    this.NotifyPropertyChanged();
                }
            }
        }

        /// Gets a value indicating whether or not the body associated with the gesture detector is currently being tracked 
        public bool IsTracked
        {
            get
            {
                return this.isTracked;
            }

            private set
            {
                if (this.IsTracked != value)
                {
                    this.isTracked = value;
                    this.NotifyPropertyChanged();
                }
            }
        }

        /// Gets a value indicating whether or not the discrete gesture has been detected
        public bool Detected
        {
            get
            {
                return this.detected;
            }

            private set
            {
                if (this.detected != value)
                {
                    this.detected = value;
                    this.NotifyPropertyChanged();
                }
            }
        }

        /// Gets a float value which indicates the detector's confidence that the gesture is occurring for the associated body 
        public float Confidence
        {
            get
            {
                return this.confidence;
            }

            private set
            {
                if (this.confidence != value)
                {
                    this.confidence = value;
                    this.NotifyPropertyChanged();
                }
            }
        }

        /// Gets an image for display in the UI which represents the current gesture result for the associated body 
        public ImageSource ImageSource
        {
            get
            {
                return this.imageSource;
            }

            private set
            {
                if (this.ImageSource != value)
                {
                    this.imageSource = value;
                    this.NotifyPropertyChanged();
                }
            }
        }

        /// Updates the values associated with the discrete gesture detection result
        public void UpdateGestureResult(bool isBodyTrackingIdValid, bool isGestureDetected, float detectionConfidence)
        {
            this.IsTracked = isBodyTrackingIdValid;
            this.Confidence = 0.0f;

            if (!this.IsTracked)
            {
                this.ImageSource = this.notTrackedImage;
                this.Detected = false;
                this.BodyColor = Brushes.Gray;
            }
            else
            {
                this.Detected = isGestureDetected;
                this.BodyColor = this.trackedColors[this.BodyIndex];

                if (this.Detected)
                {
                    this.Confidence = detectionConfidence;
                    this.ImageSource = this.seatedImage;
                }
                else
                {
                    this.ImageSource = this.notSeatedImage;
                }
            }
        }

        /// Notifies UI that a property has changed
        private void NotifyPropertyChanged([CallerMemberName] string propertyName = "")
        {
            if (this.PropertyChanged != null)
            {
                this.PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
            }
        }
    }
}
