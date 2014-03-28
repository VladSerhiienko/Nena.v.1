using InteractiveTV.Capture;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Interop;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace InteractiveTV
{
    using InteractiveTV.PageContext;
    using InteractiveTV.PageContext.Talks;
    using System.Runtime.InteropServices;

    public partial class MainWindow : Window, IChatMessageDriven, InlineServer.IConsumeGestureService
    {
        private bool isFullscreen = false;
        private PageContext.BrowserControl BrowserView;
        private PageContext.AppTalk.ChatEvent chatProxy;

        public MainWindow()
        {
            InitializeComponent();

            BrowserView = new PageContext.BrowserControl();
            pageTransitionControl.ShowPage(BrowserView);

            var app = App.Current as App;

            if (app.chat != null)
            {
                app.chat.Received += OnReceivedMessage;
                app.chat.Send += OnSendingMessage;
                app.chat.Connect();

                if (!app.chat.IsConnected)
                {
                    app.chat.Received -= OnReceivedMessage;
                    app.chat.Send -= OnSendingMessage;
                    app.chat.Close();
                    app.chat = null;
                }
                else
                {
                    app.gestureService = null;
                    app.chat.Launch();
                }
            }

            Requirements = InlineServer.Requirement.Geonodes |
                           InlineServer.Requirement.Gestures;

            if (app.chat == null)
            {
                chatProxy = new AppTalk.ChatEvent();
                chatProxy.Msg = new List<AppTalk.ChatEvent.Entry>();
                chatProxy.Msg.Add(new AppTalk.ChatEvent.Entry()
                {
                    Type = AppTalk.ChatEvent.MessageType.Event,
                    Args = AppTalk.ChatEvent.MessageArgs.Unknown
                });

                app.gestureService.StatusChanged += OnStatusChanged;
                app.gestureService.Updated += OnUpdated;
                app.gestureService.Launch();
            }
        }

        private void OnShowButtonClicked(object sender, RoutedEventArgs e)
        {
            BrowserView = new PageContext.BrowserControl();
            pageTransitionControl.ShowPage(BrowserView);
        }

        private void OnWindowKeyPressed(object sender, KeyEventArgs e)
        {
            if ((e.Key == Key.F && (Keyboard.Modifiers == ModifierKeys.Control)) || e.Key == Key.F11) SwitchWindowMode();
            else if (e.Key == Key.Escape) if (MessageBox.Show("Please, confirm exiting InteractiveTV Client...",
                    "InteractiveTV::Client::View", MessageBoxButton.YesNo) == MessageBoxResult.Yes) Close();
        }

        void SwitchWindowMode()
        {
            if (isFullscreen)
            {
                WindowStyle = WindowStyle.SingleBorderWindow;
                WindowState = WindowState.Normal;
                isFullscreen = false;
                Topmost = false;
                BrowserView.UriBar.Height = (GridLength) new GridLengthConverter().ConvertFrom("30");

                "running in fullscreen".Print();
            }
            else
            {
                Topmost = true;
                WindowStyle = WindowStyle.None;
                WindowState = WindowState.Maximized;
                isFullscreen = true;
                BrowserView.UriBar.Height = (GridLength)new GridLengthConverter().ConvertFrom("0");

                "running windowed".Print();
            }
        }

        public void OnReceivedMessage(object sender, PageContext.AppTalk.ChatEvent e)
        {
            if (!BrowserView.IsValid) return;
            try
            {
                foreach (var entry in e.Msg)
                    switch (entry.Type)
                    {
                        case AppTalk.ChatEvent.MessageType.Event:
                            switch ((uint)entry.Args)
                            {
                                case (uint)AppTalk.ChatEvent.MessageArgs.GestureSwipeLeft:
                                case (uint)PXCMGesture.Gesture.Label.LABEL_NAV_SWIPE_LEFT:
                                    "event: {0}".Print("gesture-swipe-left");
                                    BrowserView.Control.Dispatcher.BeginInvoke(new Action(delegate()
                                    { BrowserView.Control.InvokeScript("rightArrowPressed"); }));
                                    break;
                                case (uint)AppTalk.ChatEvent.MessageArgs.GestureSwipeRight:
                                case (uint)PXCMGesture.Gesture.Label.LABEL_NAV_SWIPE_RIGHT:
                                    "event: {0}".Print("gesture-swipe-left");
                                    BrowserView.Control.Dispatcher.BeginInvoke(new Action(delegate()
                                    { BrowserView.Control.InvokeScript("leftArrowPressed"); }));
                                    break;
                                case (uint)AppTalk.ChatEvent.MessageArgs.GestureSwipeUp:
                                case (uint)PXCMGesture.Gesture.Label.LABEL_NAV_SWIPE_UP:
                                    "event: {0}".Print("gesture-swipe-up");
                                    BrowserView.Control.Dispatcher.BeginInvoke(new Action(delegate()
                                    { BrowserView.Control.InvokeScript("downArrowPressed"); }));
                                    break;
                                case (uint)AppTalk.ChatEvent.MessageArgs.GestureSwipeDown:
                                case (uint)PXCMGesture.Gesture.Label.LABEL_NAV_SWIPE_DOWN:
                                    "event: {0}".Print("gesture-swipe-down");
                                    BrowserView.Control.Dispatcher.BeginInvoke(new Action(delegate()
                                    { BrowserView.Control.InvokeScript("topArrowPressed"); }));
                                    break;
                            }
                            break;
                        default:
                            "non-event-msg: ignored".Print();
                            break;
                    }
            }
            catch (COMException ex)
            {
                "com-error: chat msg recv: \n\twhat[{0}] \n\twhere[{1}]".Print(
                    ex.Message,
                    ex.Source
                    );
            }
            catch (Exception ex)
            {
                "error: chat msg recv: \n\twhat[{0}] \n\twhere[{1}]".Print(
                    ex.Message,
                    ex.Source
                    );
            }
        }

        public void OnSendingMessage(object sender, PageContext.AppTalk.ChatEvent e)
        {
        }

        public InlineServer.Requirement Requirements { get; set; }

        public void OnUpdated(InlineServer.IGestureServiceInline sender, InlineServer.UpdateEvent e)
        {
            //"inline-update ".Print();

            if (e.Gestures != null && e.Gestures[0].label > 0)
            {
                chatProxy.Msg[0].Args = (AppTalk.ChatEvent.MessageArgs)e.Gestures[0].label;
                OnReceivedMessage(this, chatProxy);
            }
        }

        public void OnStatusChanged(InlineServer.IGestureServiceInline sender, InlineServer.StatusChangedEvent e)
        {
            "inline-status {0} {1}".Print(e.Token.ToString(), e.Description);
        }
    }
}
