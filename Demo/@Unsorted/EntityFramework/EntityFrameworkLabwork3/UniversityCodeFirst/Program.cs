using System;
using System.Data.Entity;
using System.Diagnostics;

namespace UniversityCodeFirst
{
    using Migrations;
    using UniversityDatabase.Context;

    internal class Program
    {
        private static void Main(String[] args)
        {
            try
            {
                //Database.SetInitializer(new MigrateDatabaseToLatestVersion<University, Configuration>());
                //Database.SetInitializer(new DropCreateDatabaseAlways<University>());
                //Database.SetInitializer(new UniversityInitializer<University>());

                using (var context = new University())
                {
                    var handler = new UniversityTester<University>
                    {
                        //DataSupplyCallback = Samples.SupplyData,
                        AuditCallback = Samples.Audit,
                        ValidationCallback = Samples.Validation,
                        ConcurrencyDbWinCallback = Samples.FirstWins,
                        ConcurrencyMemoryWinCallback = Samples.LastWins,
                    };

                    handler.RunTests(context);
                    Samples.DataMaintenance(context);
                }
            }
            catch (System.Exception ex)
            {
                Console.WriteLine(ex.Message);
                //Console.ReadKey();
            }

            Exit();
        }

        private static void Exit()
        {
            Console.WriteLine("Labwork #03: Press any key to exit...");
            Console.ReadKey();
        }
    }

    public class Air
    {
        private static Action<string> OutputWindow;

        static Air()
        {
            ConfigTrace();
            OutputWindow = s => { Trace.WriteLine(s); };
            Broadcast("Labwork #03: CodeFirst.");
        }

        private static void ConfigTrace()
        {
            try
            {
                Trace.Listeners.Clear();
                TextWriterTraceListener twtl =
                    new TextWriterTraceListener(
                        new System.IO.StreamWriter("UniversityCodeFirstOutput.txt", false), "Air"
                        );
                twtl.TraceOutputOptions = TraceOptions.DateTime | TraceOptions.ThreadId;
                ConsoleTraceListener ctl = new ConsoleTraceListener(false);
                ctl.TraceOutputOptions = TraceOptions.DateTime;
                Trace.Listeners.Add(ctl);
                Trace.Listeners.Add(twtl);
                Trace.AutoFlush = true;
            }
            catch (System.Exception ex)
            {
                Console.WriteLine(ex);
            }
        }

        public static void Broadcast(string s)
        {
            OutputWindow(s);
        }

        public static void Broadcast(string format, params object[] args)
        {
            Broadcast(string.Format(format, args));
        }
    }
}