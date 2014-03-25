using System;
using System.Collections.Generic;
using System.Data.Entity;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CodeFirstMigrations
{
    class PeopleContext : DbContext
    {
        public DbSet<Person> People { get; set; }

        public PeopleContext()
            : base("PeopleContext")
        {

        }

        protected override void OnModelCreating(DbModelBuilder modelBuilder)
        {
            Console.WriteLine(
                "Event-Hook: PeopleContext::OnModelCreation(DbModelBuilder)"
                );

            Console.WriteLine("   > Person: Id = PK");

            modelBuilder.Entity<Person>()
                .HasKey(p => p.Id);

            Console.WriteLine("   > Person: FirstName");

            modelBuilder.Entity<Person>()
                .Property(p => p.FirstName)
                .IsConcurrencyToken()
                .IsVariableLength()
                .IsMaxLength()
                .IsRequired();

            Console.WriteLine("   > Person: LastName");

            modelBuilder.Entity<Person>()
                .Property(p => p.LastName)
                .IsConcurrencyToken()
                .IsVariableLength()
                .IsMaxLength()
                .IsRequired();
        }
    }
}
