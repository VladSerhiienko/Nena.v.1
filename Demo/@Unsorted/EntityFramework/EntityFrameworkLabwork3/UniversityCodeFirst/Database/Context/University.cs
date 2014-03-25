using System.Data.Entity;
using System.Diagnostics;
using System.Linq;

namespace UniversityCodeFirst.UniversityDatabase.Context
{
    using Entities;
    using System.Data.Entity.Validation;

    public class University : DbContext
    {
        public DbSet<Discipline> Disciplines { get; set; }

        public DbSet<Student> Students { get; set; }

        public DbSet<Group> Groups { get; set; }

        public University()
            : base("UniversityCodeFirstDatabase")
        {
        }

        public override int SaveChanges()
        {
            try
            {
                Air.Broadcast("Db>> Trying to save the changes...");

                return base.SaveChanges();
            }
            catch (DbEntityValidationException ex)
            {
                Air.Broadcast("Internal Validation Exception Handler>>");

                int currentErrorNum = 1;
                ex.EntityValidationErrors.ToList()
                    .ForEach(result => result.ValidationErrors.ToList()
                        .ForEach(e =>
                        {
                            Air.Broadcast("Error#" + currentErrorNum++);
                            Air.Broadcast("   >> Type: " + result.Entry.Entity.GetType().Name);
                            Air.Broadcast("   >> Property: " + e.PropertyName);
                            Air.Broadcast("   >> Error: " + e.ErrorMessage);
                        })
                    );

                throw;
            }
        }

        protected override void OnModelCreating(DbModelBuilder modelBuilder)
        {
            // Programmatically define constrains applicable to the domain.

            modelBuilder.Entity<Group>().HasKey(g => g.Id);
            modelBuilder.Entity<Group>().Property(g => g.Acronym).IsRequired();
            modelBuilder.Entity<Group>()
            .Property(g => g.Acronym)
            .IsConcurrencyToken(true)
            .IsVariableLength()
            .HasMaxLength(5);

            modelBuilder.Entity<Student>().HasKey(s => s.Id);
            modelBuilder.Entity<Student>().Property(s => s.Name).IsRequired();
            modelBuilder.Entity<Student>().Property(s => s.Surname).IsRequired();

            modelBuilder.Entity<Discipline>().HasKey(d => d.Id);
            modelBuilder.Entity<Discipline>().Property(d => d.Title).IsRequired();
            modelBuilder.Entity<Discipline>().Property(d => d.Factor).IsOptional();
            modelBuilder.Entity<Discipline>()
                 .Property(d => d.Title)
                 .IsConcurrencyToken(true)
                 .IsVariableLength()
                 .HasMaxLength(50);

            // Programmatically describe the relation between tables and columns.

            modelBuilder.Entity<Discipline>()
                .HasMany(d => d.Groups)
                .WithMany(g => g.Disciplines)
                .Map(manyToManyConfig =>
                {
                    manyToManyConfig.MapLeftKey("Discipline_Id");
                    manyToManyConfig.MapRightKey("Group_Id");
                    manyToManyConfig.ToTable("Disciplines_Groups");
                });
        }
    }
}