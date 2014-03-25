using System.Data.Entity;
using System.Diagnostics;

namespace UniversityCodeFirst.UniversityDatabase.Context
{
    public class UniversityInitializer<T> : IDatabaseInitializer<T> where T : DbContext, new()
    {
        public void InitializeDatabase(T context)
        {
            if (context.Database.Exists())
            {
                Air.Broadcast("InitializeDatabase>> Connection exists. Closing...");

                context.Database.ExecuteSqlCommand("ALTER DATABASE " + context.Database.Connection.Database + " SET SINGLE_USER WITH ROLLBACK IMMEDIATE");
                context.Database.ExecuteSqlCommand("USE [master] DROP DATABASE " + context.Database.Connection.Database);
                Air.Broadcast("InitializeDatabase>> Closed.");
            }

            Air.Broadcast("InitializeDatabase>> Creating...");
            context.Database.Create();
            Air.Broadcast("InitializeDatabase>> Created.");
        }
    }
}