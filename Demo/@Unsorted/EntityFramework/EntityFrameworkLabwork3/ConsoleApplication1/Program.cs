using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ConsoleApplication1.Company;
using System.Data.Entity;

namespace ConsoleApplication1
{
    class Program
    {
        static void Main(string[] args)
        {
            Database.SetInitializer<CompanyContext>(
                new DropCreateDatabaseAlways<CompanyContext>()
                );

            Samples.Creation();
        }
    }

    class Samples
    {
        public static void Creation()
        {
            // If there is no department, add one.

            using (var database = new CompanyContext())
            {
                var department = database.Departments
                    .FirstOrDefault(d => d.Name == "IT");

                if (department == null)
                {
                    department = new Department { Name = "IT" };
                    database.Departments.Add(department);
                }

                // Add new manager
                //Console.WriteLine("Enter manager name: ");
                //var managerName = Console.ReadLine();

                var manager = new Manager
                {
                    ManagerCode = "JDO",
                    FirstName = "Vladyslav",
                    LastName = "Serhiienko",
                    Department = department
                };

                database.Managers.Add(manager);

                //Console.WriteLine("Enter collaborator name: ");
                //var collaboratorName = Console.ReadLine();

                var collaborator = new Collaborator
                {
                    FirstName = "Vasyl",
                    LastName = "Romanchak",
                    Department = department,
                    Manager = manager
                };

                database.Collaborators.Add(collaborator);

                int recordsAffected = 0;
                recordsAffected = database.SaveChanges();

                Console.WriteLine(
                    recordsAffected + " were saved to the database."
                    );

                var ITManagers =
                    database.Managers.Where(m => m.Department.Name == "IT");
                
                foreach (var m in ITManagers)
                {
                    Console.WriteLine(" - " + m.FirstName + " " + m.LastName);
                }

                var ITCollaborators = 
                    database.Collaborators.Where(c => c.Department.Name == "IT");

                foreach (var c in ITCollaborators)
                {
                    Console.WriteLine(" - " + c.FirstName);
                }
                
                
                Console.ReadKey(false);
            }
        }

        public static void Audit()
        {

            using (var context = new CompanyContext())
            {
                var manager = context.Managers.Find("JDO");

                // Change value directly in the DB
                using (var contextDB = new CompanyContext())
                {
                    contextDB.Database.ExecuteSqlCommand(
                        "UPDATE managers SET Name = Name + '_DB' WHERE ManagerCode = 'JDO'");
                }

                // Change the current value in memory
                manager.FirstName = manager.FirstName + "_Memory";

                string value = context.Entry(manager).Property(m => m.FirstName).OriginalValue;
                Console.WriteLine(string.Format("Original Value : {0}", value));

                value = context.Entry(manager).Property(m => m.FirstName).CurrentValue;
                Console.WriteLine(string.Format("Current Value : {0}", value));

                value = context.Entry(manager).GetDatabaseValues().GetValue<string>("Name");
                Console.WriteLine(string.Format("DB Value : {0}", value));

                Console.ReadKey();
            }

        }

        public static void Validation()
        {
            using (var context = new CompanyContext())
            {
                var manager = new Manager() { FirstName = string.Empty };
                context.Managers.Add(manager);

                var validationErrors = context.GetValidationErrors()
                    .Where(vr => !vr.IsValid)
                    .SelectMany(vr => vr.ValidationErrors);

                foreach (var error in validationErrors)
                {
                    Console.WriteLine(error.ErrorMessage);
                }

                Console.ReadKey();
            }
        }
    }
}
