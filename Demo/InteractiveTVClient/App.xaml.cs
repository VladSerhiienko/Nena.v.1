using InteractiveTV.PageContext;
using Microsoft.Win32.SafeHandles;
using System;
using System.Collections.Generic;
using System.Configuration;
using System.Data;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;

namespace InteractiveTV
{
    /// <summary>
    /// Interaction logic for App.xaml
    /// </summary>
    public partial class App : Application
    {
        [DllImport("kernel32.dll",
            EntryPoint = "GetStdHandle",
            SetLastError = true,
            CharSet = CharSet.Auto,
            CallingConvention = CallingConvention.StdCall)]
        private static extern IntPtr GetStdHandle(int nStdHandle);
        [DllImport("kernel32.dll",
            EntryPoint = "AllocConsole",
            SetLastError = true,
            CharSet = CharSet.Auto,
            CallingConvention = CallingConvention.StdCall)]
        private static extern int AllocConsole();
        private const int STD_OUTPUT_HANDLE = -11;
        private const int MY_CODE_PAGE = 437;   

        static void OpenConsoleWindow()
        {
            AllocConsole();

            IntPtr stdHandle = GetStdHandle(STD_OUTPUT_HANDLE);
            SafeFileHandle safeFileHandle = new SafeFileHandle(stdHandle, true);
            FileStream fileStream = new FileStream(safeFileHandle, FileAccess.Write);
            Encoding encoding = System.Text.Encoding.GetEncoding(MY_CODE_PAGE);
            StreamWriter standardOutput = new StreamWriter(fileStream, encoding);
            standardOutput.AutoFlush = true;
            Console.SetOut(standardOutput);

            "console allocated".Print();

        }

        public AppTalk chat;
        public InlineServer.GestureServiceInline gestureService; 

        public App() : base()
        {
        }

        private void OnApplicationStartup(object sender, StartupEventArgs e)
        {
            OpenConsoleWindow();

            "hook web dialogs".Print();
            WebBrowserControlDialogs.WindowsInterop.Hook();

            "launching chat...".Print();

            chat = new AppTalk();
            chat.Send += OnSendingMessage;
            chat.Received += OnReceivedMessage;

            "launching inline server...".Print();

            gestureService = new InlineServer.GestureServiceInline();

        }

        void OnReceivedMessage(object sender, AppTalk.ChatEvent e)
        {
            if (e.TryParseText())
                foreach(var entry in e.Msg)
                    "{0} -> {1}".Print(
                        entry.Type.ToString(), 
                        entry.Args.ToString()
                        );
            if (e.Msg.Count == 0)
                Thread.Sleep(300);
        }

        void OnSendingMessage(object sender, AppTalk.ChatEvent e)
        {
            e.Txt = AppTalk.ChatEvent.ComposeMessage(
                AppTalk.ChatEvent.MessageType.Request,
                AppTalk.ChatEvent.MessageArgs.Next
                );
        }

        private void OnExit(object sender, ExitEventArgs e)
        {
            //chat.Close();

            "quit inline server".Print();
            if (gestureService != null) gestureService.Stop();
            "quit chat".Print();
            if (chat != null) chat.Close();

            "unhook web dialogs".Print();
            WebBrowserControlDialogs.WindowsInterop.Unhook();
        }

        private void OnActivated(object sender, EventArgs e)
        {
            "app activated".Print();
        }

        private void OnDeactivated(object sender, EventArgs e)
        {
            "app deactivated".Print();
        }

    }
}
