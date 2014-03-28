using Microsoft.Win32;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Reflection;
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
using InteractiveTV;

namespace InteractiveTV.PageContext
{
    /// <summary>
    /// Interaction logic for BrowserControl.xaml
    /// </summary>
    public partial class BrowserControl : UserControl
    {
        [ComImport, Guid("6D5140C1-7436-11CE-8034-00AA006009FA")]
        [InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
        private interface IOleServiceProvider
        {
            [PreserveSig]
            int QueryService(
                [In] ref Guid guidService, 
                [In] ref Guid riid, 
                [MarshalAs(UnmanagedType.IDispatch)] out object ppvObject
                );
        }

        public BrowserControl()
        {
            InitializeComponent();

            SetSilent(ITVView, true);
            dynamic activeX = ITVView.GetType().InvokeMember("ActiveXInstance",
                BindingFlags.GetProperty | BindingFlags.Instance | BindingFlags.NonPublic,
                null, ITVView, new object[] { }); activeX.Silent = true;

            address.Text = @"D:\Development\VisualStudio\Aktuelle\Nena\Demo\InteractiveTVClient\itv\index.html";
            //address.Text = @"http://www.codeproject.com";
            ///D:\Development\VisualStudio\Aktuelle\Nena\Demo\InteractiveTVClient\itv

            ITVView.Unloaded += ITVView_Unloaded;
            ITVView.LoadCompleted += ITVView_LoadCompleted;
            ITVView.Navigate(address.Text);
        }

        public bool IsValid { get; private set; }
        public RowDefinition UriBar { get { return uriBar; } }

        void ITVView_Unloaded(object sender, RoutedEventArgs e)
        {
            IsValid = false;
        }

        void ITVView_LoadCompleted(object sender, NavigationEventArgs e)
        {
            IsValid = true;
        }

        private void OnLoaded(object sender, RoutedEventArgs e)
        {
        }

        public static void SetSilent(WebBrowser browser, bool silent)
        {
            IOleServiceProvider sp = browser.Document as IOleServiceProvider;
            if (sp != null)
            {
                Guid IID_IWebBrowserApp = new Guid("0002DF05-0000-0000-C000-000000000046");
                Guid IID_IWebBrowser2 = new Guid("D30C1661-CDAF-11d0-8A3E-00C04FC9E26E");

                object webBrowser;
                sp.QueryService(
                    ref IID_IWebBrowserApp, 
                    ref IID_IWebBrowser2, 
                    out webBrowser
                    );

                if (webBrowser != null)
                {
                    webBrowser.GetType().InvokeMember("Silent", 
                        BindingFlags.Instance | BindingFlags.Public | BindingFlags.PutDispProperty, 
                        null, webBrowser, new object[] { silent }
                        );
                }
            }
        }

        private void OnGoButtonClicked(object sender, RoutedEventArgs e)
        {
            ITVView.Navigate(address.Text);
        }

        public WebBrowser Control
        {
            get { return ITVView; }
        }
    }
}
