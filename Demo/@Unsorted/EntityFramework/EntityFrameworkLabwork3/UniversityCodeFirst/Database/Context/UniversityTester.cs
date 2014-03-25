using System;
using System.Diagnostics;

namespace UniversityCodeFirst.UniversityDatabase.Context
{
    internal class UniversityTester<T>
        where T : University, new()
    {
        public Action<T> DataSupplyCallback { protected get; set; }
        public Action<T> AuditCallback { protected get; set; }
        public Action<T> ValidationCallback { protected get; set; }
        public Action<T> ConcurrencyDbWinCallback { protected get; set; }
        public Action<T> ConcurrencyMemoryWinCallback { protected get; set; }

        public UniversityTester()
        {
            Air.Broadcast("Creating db tester instance.");

            DataSupplyCallback = delegate { Air.Broadcast("DataSupplyCallback>> Not provided."); };
            AuditCallback = delegate { Air.Broadcast("AuditCallback>> Not provided."); };
            ValidationCallback = delegate { Air.Broadcast("ValidationCallback>> Not provided."); };
            ConcurrencyDbWinCallback = delegate { Air.Broadcast("ConcurrencyDbWinCallback>> Not provided."); };
            ConcurrencyMemoryWinCallback = delegate { Air.Broadcast("ConcurrencyMemoryWinCallback>> Not provided."); };
        }

        public void RunTests(T context)
        {
            Air.Broadcast("RunTests>>");
            Air.Broadcast("-----------------------------------");
            Air.Broadcast("DataSupply Stage:");
            RunTest(DataSupplyCallback, context);
            Air.Broadcast("-----------------------------------");
            Air.Broadcast("Audit Stage:");
            AuditCallback(context);
            Air.Broadcast("-----------------------------------");
            Air.Broadcast("Validation Stage:");
            RunTest(ValidationCallback, context);
            Air.Broadcast("-----------------------------------");
            Air.Broadcast("Concurrency Db Wins Stage:");
            RunTest(ConcurrencyDbWinCallback, context);
            Air.Broadcast("-----------------------------------");
            Air.Broadcast("Concurrency Memory Wins Stage:");
            RunTest(ConcurrencyMemoryWinCallback, context);
            Air.Broadcast("-----------------------------------");
        }

        protected void RunTest(Action<T> callback, T context)
        {
            if (callback != null)
            {
                callback(context);
            }
            else
            {
                Air.Broadcast(callback.GetType().Name + ": Not provided.");
            }
        }
    }
}