using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Diagnostics;

namespace Common
{
    static class Supervisor
    {
        public static bool IS_CONSOLE_WINDOW_ENABLED = false;

        static Supervisor()
        {
            Trace.Listeners.Clear();

            TextWriterTraceListener twtl = new TextWriterTraceListener("output.txt");
            twtl.Name = "Air";
            twtl.TraceOutputOptions = TraceOptions.DateTime | TraceOptions.ThreadId;

            ConsoleTraceListener ctl = new ConsoleTraceListener(false);
            ctl.TraceOutputOptions = TraceOptions.DateTime;

            Trace.Listeners.Add(ctl);
            Trace.Listeners.Add(twtl);
            Trace.AutoFlush = true;
        }

        public static void WriteMode()
        {
#if (DEBUG)
            Debug.WriteLine("Supervisor: Mode = Debugging.");

            if (IS_CONSOLE_WINDOW_ENABLED)
            {
                Console.WriteLine("Supervisor: Mode = Debugging.");
                Console.WriteLine("Supervisor: I.e.: Program output is streamed to the Visual Studio Output Window.");
            }
#else
            if (IS_CONSOLE_WINDOW_ENABLED)
            {
                Console.WriteLine("Supervisor: Mode = Released");
                Console.WriteLine("Supervisor: I.e.: Program output is streamed to the Application Console Window.");
            }
#endif
        }

        public static void Write(string message)
        {
            message = "Supervisor: " + message;
#if (DEBUG)
            Debug.WriteLine(message);
#else
            if (IS_CONSOLE_WINDOW_ENABLED)
            {
                Console.WriteLine(message);
            }
#endif
        }

        public static void Write(object obj)
        {
            string message = "Supervisor: " + obj.ToString();
#if (DEBUG)
            Debug.WriteLine(message);
#else
            if (IS_CONSOLE_WINDOW_ENABLED)
            {
                Console.WriteLine(message);
            }
#endif
        }

        public static void Write(string format, params object[] args)
        {
            format = "Supervisor: " + format;
#if (DEBUG)
            Debug.WriteLine(string.Format(format, args));
#else
            if (IS_CONSOLE_WINDOW_ENABLED)
            {
                Console.WriteLine(format, args);
            }
#endif
        }
    }
}
