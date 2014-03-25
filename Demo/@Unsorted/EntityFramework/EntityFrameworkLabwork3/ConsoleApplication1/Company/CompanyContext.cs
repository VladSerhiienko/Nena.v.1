using System;
using System.Collections.Generic;
using System.Data.Entity;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ConsoleApplication1.Company
{
    class CompanyContext : DbContext
    {
        public DbSet<Manager> Managers { get; set; }
        public DbSet<Department> Departments { get; set; }
        public DbSet<Collaborator> Collaborators { get; set; }

        public CompanyContext()
            : base("CompanyContext")
        {


        }

        protected override void OnModelCreating(DbModelBuilder modelBuilder)
        {
            // Programmatically define constrains applicable to the domain.

            modelBuilder.Entity<Department>()
                .Property(Department => Department.Name)
                .IsRequired();

            modelBuilder.Entity<Manager>()
                .HasKey(Manager => Manager.ManagerCode);

            modelBuilder.Entity<Manager>()
                .Property(Manager => Manager.FirstName)
                .IsConcurrencyToken(true)
                .IsVariableLength()
                .HasMaxLength(20);

            // Programmatically describe the relation between tables and columns. 

            modelBuilder.Entity<Manager>()
                .HasRequired(Manager => Manager.Department)
                .WithMany()
                .HasForeignKey(Manager => Manager.DepartmentId)
                .WillCascadeOnDelete();
        }
    }
}
