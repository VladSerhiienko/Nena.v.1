using System;
using System.Net.Sockets;
using System.Net;
using System.IO;
using System.Text;
using System.Threading.Tasks;
using System.Threading;
using System.Collections.Generic;
using InteractiveTV;

namespace InteractiveTV.PageContext
{
    public class AppTalk
    {
        public class ChatEvent
        {

            public static readonly String CeEvent = "event:";
            public static readonly String CeRequest = "request:";
            public static readonly String CeEventSubscribe = "event-subscribe:";
            public static readonly String CeEventUnsubscribe = "event-unsubscribe:";
            public static readonly String CeRequestName = "name";
            public static readonly String CeRequestNext = "next";
            public static readonly String CeEventGestureNavigation = "gesture-navigation";
            public static readonly String CeEventGestureSwipeRight = "gesture-swipe-right";
            public static readonly String CeEventGestureSwipeLeft = "gesture-swipe-left";
            public static readonly String CeEventGestureSwipeDown = "gesture-swipe-down";
            public static readonly String CeEventGestureSwipeUp = "gesture-swipe-up";
            public static readonly String CeEventFaceDetected = "face-detected";
            public static readonly String CeEventFaceRecognized = "face-recognized";

            public enum MessageType
            {
                Event,
                Request,
                EventSubscribe,
                EventUnsubscribe,
                Unknown
            }

            public enum MessageArgs
            {
                Name,
                Next,
                GestureNavigation,
                GestureSwipeRight,
                GestureSwipeLeft,
                GestureSwipeDown,
                GestureSwipeUp,
                FaceDetected,
                FaceRecognized,
                Unknown
            }

            public class Entry
            {
                public MessageType Type;
                public MessageArgs Args;
            }

            public static string ToString(MessageType type)
            {
                switch (type)
                {
                    case MessageType.Event: return CeEvent;
                    case MessageType.Request: return CeRequest;
                    case MessageType.EventSubscribe: return CeEventSubscribe;
                    case MessageType.EventUnsubscribe: return CeEventUnsubscribe;
                    default: return null;
                }
            }

            public static MessageType ToMessageType(string type)
            {
                if (type == CeEventUnsubscribe) return MessageType.EventUnsubscribe;
                if (type == CeEventSubscribe) return MessageType.EventSubscribe;
                if (type == CeRequest) return MessageType.Request;
                if (type == CeEvent) return MessageType.Event;
                return MessageType.Unknown;
            }

            public static MessageArgs ToMessageArgs(string type)
            {
                if (type == CeEventGestureNavigation) return MessageArgs.GestureNavigation;
                if (type == CeEventGestureSwipeRight) return MessageArgs.GestureSwipeRight;
                if (type == CeEventGestureSwipeLeft) return MessageArgs.GestureSwipeLeft;
                if (type == CeEventGestureSwipeDown) return MessageArgs.GestureSwipeDown;
                if (type == CeEventGestureSwipeUp) return MessageArgs.GestureSwipeUp;
                return MessageArgs.Unknown;
            }

            public static string ToString(MessageArgs args)
            {
                switch (args)
                {
                    case MessageArgs.GestureNavigation: return CeEventGestureNavigation;
                    case MessageArgs.GestureSwipeRight: return CeEventGestureSwipeRight;
                    case MessageArgs.GestureSwipeLeft: return CeEventGestureSwipeLeft;
                    case MessageArgs.GestureSwipeDown: return CeEventGestureSwipeDown;
                    case MessageArgs.GestureSwipeUp: return CeEventGestureSwipeUp;
                    case MessageArgs.FaceRecognized: return CeEventFaceRecognized;
                    case MessageArgs.FaceDetected: return CeEventFaceDetected;
                    case MessageArgs.Name: return CeRequestName;
                    case MessageArgs.Next: return CeRequestNext;
                    default: return null;
                }
            }

            public string Txt;
            public List<Entry> Msg = new List<Entry>();

            public static string ComposeMessage(MessageType type, MessageArgs args)
            {
                return ComposeMessage(ToString(type), ToString(args));
            }

            public static string ComposeMessage(string type, string args)
            {
                if (type != null && args != null) 
                    return string.Format("{0} {1};", type, args);
                else return null;
            }

            public bool TryParseText()
            {
                string[] entries = Txt.Split(new char[] { ';' }, StringSplitOptions.RemoveEmptyEntries);
                if (entries.Length != 0) Msg.Clear();

                foreach(var entry in entries)
                {
                    string[] type_args = entry.Split(new char[] { ' ' }, StringSplitOptions.RemoveEmptyEntries);
                    if (type_args.Length < 2) continue;

                    Entry parsed = new Entry();
                    parsed.Type = ToMessageType(type_args[0]);
                    parsed.Args = ToMessageArgs(type_args[1]);
                    Msg.Add(parsed);
                }

                return true;
            }

        }

        public delegate void ChatEventHandler(object sender, ChatEvent e);

        const int DefaultPort = 27015;

        private TcpClient Callbox = null;
        public event ChatEventHandler Send;
        public event ChatEventHandler Received;

        public CancellationTokenSource ChatToken { get; private set; }
        public Task ChatRoutine { get; private set; }
        public bool IsConnected { get { return Callbox.Connected; } }
        public int Port { get; set; }

        public AppTalk(int port = DefaultPort)
        {
            Callbox = new TcpClient();
            Callbox.Client.Blocking = true;
            Port = port;
            Connect();
        }

        public bool Connect()
        {
            if (IsConnected) return true;
            else try
            {
                "connecting to server...".Print();
                Callbox.Connect("localhost", Port);
            }
            catch (Exception e)
            {
                "connection failed".Print();
                "message: {0} ".Print(e.Message);
                "target site: {0}".Print(e.TargetSite.Name);
                return IsConnected;
            }

            "connected".Print();
            "local end point: {0}".Print(Callbox.Client.LocalEndPoint.ToString());
            return IsConnected;
        }

        public void Close()
        {
            try
            {
                if (ChatToken != null) ChatToken.Cancel();
                if (ChatRoutine != null) ChatRoutine.Wait();
                if (Callbox != null) Callbox.Close();
                ChatRoutine = null;
                Callbox = null;
                "chat closed".Print();
            }
            catch
            {
                "errors in close".Print();
            }
            finally
            {
                "closed".Print();
            }
        }

        public void Launch()
        {
            try
            {
                Connect();

                if (!IsConnected) return;

                ChatToken = new CancellationTokenSource();
                CancellationToken token = ChatToken.Token;

                ChatEvent eventArgs = new ChatEvent();

                ChatRoutine = Task.Run(() =>
                {
                    Stream stm = Callbox.GetStream();

                    try
                    {
                        while (!token.IsCancellationRequested)
                        {
                            Send(this, eventArgs);
                            //"send:  {0}".Print(eventArgs.Txt);

                            ASCIIEncoding asen = new ASCIIEncoding();
                            byte[] ba = asen.GetBytes(eventArgs.Txt);
                            stm.Write(ba, 0, ba.Length);

                            byte[] bb = new byte[100];
                            int k = stm.Read(bb, 0, 100);

                            eventArgs.Txt = string.Empty;
                            for (int i = 0; i < k; i++)
                                eventArgs.Txt += Convert.ToChar(bb[i]);

                            //"recv:  {0}".Print(eventArgs.Txt);
                            Received(this, eventArgs);

                            //
                            //Thread.Sleep(200);
                        }
                    }
                    catch (Exception e)
                    {
                        "in-in-exception: mssg '{0}' ".Print(e.Message);
                        "in-in-exception: site '{0}' ".Print(e.TargetSite.ToString());
                    }

                    "chat cancelled".Print();

                }, token);

            }
            catch (Exception e)
            {
                "in-exception: mssg {0} ".Print(e.Message);
                "in-exception: site {0} ".Print(e.TargetSite.ToString());
            }
        }

    }
}
