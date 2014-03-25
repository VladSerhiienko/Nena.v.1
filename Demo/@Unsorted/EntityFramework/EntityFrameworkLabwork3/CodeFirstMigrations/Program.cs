using System;
using System.Collections.Generic;
using System.Data.Entity;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CodeFirstMigrations
{
    class Program
    {
        static void Main(string[] args)
        {
            Console.WriteLine("App: Main(String[]).");

            //Database.SetInitializer<PeopleContext>(
            //    new DropCreateDatabaseAlways<PeopleContext>()
            //    );

            Console.WriteLine("App: Create database.");

            using (var database = new PeopleContext())
            {

                Console.WriteLine("App: Check if filled.");

                var person = database.People.FirstOrDefault();

                if (person == null)
                {
                    Console.WriteLine("App: Empty. Adding new person.");

                    person = new Person
                    {
                        Id = 1,
                        FirstName = "Vlad Serh",
                        BirthData = new DateTime(1993, 5, 8),
                        Address = "Ukraine, Lwiw"
                    };

                    database.People.Add(person);

                    int entriesAffected = database.SaveChanges();
                    Console.WriteLine(entriesAffected);
                }

                Console.WriteLine("App: Print people` names.");

                foreach(var p in database.People)
                {
                    Console.WriteLine(p.FirstName);
                }
            }
        }
    }
}
