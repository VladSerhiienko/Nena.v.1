using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using Luxand;
using System.Threading;
using System.Drawing;
using System.IO;

namespace InteractiveTV.PageContext
{
    public partial class CameraControl : UserControl
    {
        enum ProgramState { psRemember, psRecognize }
        ProgramState programState = ProgramState.psRecognize;

        String cameraName;
        bool needClose = false;
        bool waitingUserInput = false;
        string userName;
        String TrackerMemoryFile = "tracker.dat";
        int mouseX = 0;
        int mouseY = 0;
        int tracker = 0; 	// creating a Tracker
        long[] IDs;
        int i;

        public CancellationTokenSource capturingTokenSource { get; private set; }
        public Task capturingTask { get; private set; }

        // WinAPI procedure to release HBITMAP handles returned by FSDKCam.GrabFrame
        [DllImport("gdi32.dll")]
        static extern bool DeleteObject(IntPtr hObject);
        public static System.Windows.Controls.Image WFormsImageToWPFImage(System.Drawing.Image Old_School_Image)
        {
            MemoryStream ms = new MemoryStream();
            Old_School_Image.Save(ms, System.Drawing.Imaging.ImageFormat.Png);

            System.Windows.Media.Imaging.BitmapImage bImg = new System.Windows.Media.Imaging.BitmapImage();
            bImg.BeginInit();
            bImg.StreamSource = new MemoryStream(ms.ToArray());
            bImg.EndInit();

            System.Windows.Controls.Image WPFImage = new System.Windows.Controls.Image();
            WPFImage.Source = bImg;
            //WPFImage.Source.Dispatcher.InvokeAsync(() => WPFImage.Source = bImg, System.Windows.Threading.DispatcherPriority.Normal);
            return WPFImage;
        }


        public CameraControl()
        {
            InitializeComponent();
        }

        public void StartCapturing(CancellationToken token)
        {
            int cameraHandle = 0;

            int r = FSDKCam.OpenVideoCamera(ref cameraName, ref cameraHandle);
            if (r != FSDK.FSDKE_OK)
            {
                MessageBox.Show("Error opening the first camera", "Error", MessageBoxButton.OK);
                return;
            }

            if (FSDK.FSDKE_OK != FSDK.LoadTrackerMemoryFromFile(ref tracker, TrackerMemoryFile)) // try to load saved tracker state
                FSDK.CreateTracker(ref tracker); // if could not be loaded, create a new tracker

            int err = 0; // set realtime face detection parameters
            FSDK.SetTrackerMultipleParameters(tracker, "HandleArbitraryRotations=false; DetermineFaceRotationAngle=false; InternalResizeWidth=100; FaceDetectionThreshold=5;", ref err);

            while (!needClose)
            {
                Int32 imageHandle = 0;
                if (FSDK.FSDKE_OK != FSDKCam.GrabFrame(cameraHandle, ref imageHandle))
                {
                    continue;
                }

                FSDK.CImage image = new FSDK.CImage(imageHandle);

                long faceCount = 0;
                FSDK.FeedFrame(tracker, 0, image.ImageHandle, ref faceCount, out IDs, sizeof(long) * 256);
                Array.Resize(ref IDs, (int)faceCount);

                // make UI controls accessible (to find if the user clicked on a face)

                System.Drawing.Image frameImage = image.ToCLRImage();
                Graphics gr = Graphics.FromImage(frameImage);

                for (i = 0; i < IDs.Length; ++i)
                {
                    FSDK.TFacePosition facePosition = new FSDK.TFacePosition();
                    FSDK.GetTrackerFacePosition(tracker, 0, IDs[i], ref facePosition);

                    int left = facePosition.xc - (int)(facePosition.w * 0.6);
                    int top = facePosition.yc - (int)(facePosition.w * 0.5);
                    int w = (int)(facePosition.w * 1.2);

                    String name;
                    int res = FSDK.GetAllNames(tracker, IDs[i], out name, 65536); // maximum of 65536 characters

                    if (FSDK.FSDKE_OK == res && name.Length > 0)
                    { // draw name
                        StringFormat format = new StringFormat();
                        format.Alignment = StringAlignment.Center;

                        gr.DrawString(name, new System.Drawing.Font("Arial", 16),
                            new System.Drawing.SolidBrush(System.Drawing.Color.LightGreen),
                            facePosition.xc, top + w + 5, format);
                    }

                    System.Drawing.Pen pen = Pens.LightGreen;
                    if (mouseX >= left && mouseX <= left + w && mouseY >= top && mouseY <= top + w)
                    {
                        pen = Pens.Blue;
                        if (ProgramState.psRemember == programState)
                        {
                            if (FSDK.FSDKE_OK == FSDK.LockID(tracker, IDs[i]))
                            {
                                if (!personNamePopup.IsOpen)
                                { 
                                    personNamePopup.IsOpen = true;
                                    waitingUserInput = true;
                                }
                            }
                        }
                    }
                    gr.DrawRectangle(pen, left, top, w, w);
                }

                programState = ProgramState.psRecognize;

                // display current frame

                try
                {
                    capturedImage.Source.Dispatcher.BeginInvoke(
                        (Action)(() => capturedImage = WFormsImageToWPFImage(frameImage)))//, 
                        //System.Windows.Threading.DispatcherPriority.Normal)
                        //.Wait();
                        ;
                }
                catch (Exception)
                {
                }

                //capturedImage.Source.Dispatcher.Invoke(
                //    (Action)(() => capturedImage = WFormsImageToWPFImage(frameImage)))//, 
                //    //System.Windows.Threading.DispatcherPriority.Normal)
                //    //.Wait();
                //    ;

                //capturedImage = WFormsImageToWPFImage(frameImage);
                GC.Collect(); // collect the garbage after the deletion
            }

            FSDK.SaveTrackerMemoryToFile(tracker, TrackerMemoryFile);
            FSDK.FreeTracker(tracker);

            FSDKCam.CloseVideoCamera(cameraHandle);
            FSDKCam.FinalizeCapturing();   

        }

        private void OnLoaded(object sender, RoutedEventArgs e)
        {
            if (FSDK.FSDKE_OK != FSDK.ActivateLibrary("BDamqp/LmVRIFKBUehhOLhjFApxKwiISCu3sTBhksRWFAEpsHvuw7GG3Bs0Yf8QRnt7vo5MtOMhml0PD+ZPPm0G2UjVbfgCrm9Av448HWO0EROCkoGuSUJYnhguTluJcWwDAPo4n3zGlsmRHj3Aqo2Hmw9aK15ExvdQEoNNckYU="))
            {
                return;
            }

            FSDK.InitializeLibrary();
            FSDKCam.InitializeCapturing();


            string[] cameraList;
            int count;
            FSDKCam.GetCameraList(out cameraList, out count);

            if (0 == count)
            {
                MessageBox.Show("Please attach a camera", "Error", MessageBoxButton.OK);
                return;
            }
            cameraName = cameraList[0];

            FSDKCam.VideoFormatInfo[] formatList;
            FSDKCam.GetVideoFormatList(ref cameraName, out formatList, out count);

            int VideoFormat = 0; // choose a video format
            capturedImage.Width = formatList[VideoFormat].Width;
            capturedImage.Height = formatList[VideoFormat].Height;

            Width = formatList[VideoFormat].Width + 48;
            Height = formatList[VideoFormat].Height + 96;

            capturingTokenSource = new CancellationTokenSource();

            capturingTask = Task.Run(() => StartCapturing(capturingTokenSource.Token), capturingTokenSource.Token);
        }

        private void OnUnloaded(object sender, RoutedEventArgs e)
        {
            needClose = true;
        }

        private void OnImageMouseMove(object sender, MouseEventArgs e)
        {
            mouseX = (int)e.GetPosition(capturedImage).X;
            mouseY = (int)e.GetPosition(capturedImage).Y;
        }

        private void OnImageMouseLeave(object sender, MouseEventArgs e)
        {
            mouseX = 0;
            mouseY = 0;
        }

        private void OnImageMouseUp(object sender, MouseButtonEventArgs e)
        {
            programState = ProgramState.psRemember;
        }

        private void OnPopupOkPressed(object sender, RoutedEventArgs e)
        {
            userName = personName.Text;
            FSDK.SetName(tracker, IDs[i], userName);
            FSDK.UnlockID(tracker, IDs[i]);
            personNamePopup.IsOpen = false;
            waitingUserInput = false;
        }

        private void OnPopupCancelPressed(object sender, RoutedEventArgs e)
        {
            waitingUserInput = false;
        }
    }
}
