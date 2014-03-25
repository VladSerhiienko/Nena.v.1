namespace UniversityCodeFirst.Migrations
{
    using System;
    using System.Data.Entity.Migrations;
    using System.Diagnostics;
    using System.Linq;
    using UniversityCodeFirst.UniversityDatabase.Context;
    using UniversityCodeFirst.UniversityDatabase.Entities;

    internal sealed class Configuration : DbMigrationsConfiguration<UniversityCodeFirst.UniversityDatabase.Context.University>
    {
        public Configuration()
        {

            AutomaticMigrationDataLossAllowed = true;
            AutomaticMigrationsEnabled = true;
        }

        protected override void Seed(UniversityCodeFirst.UniversityDatabase.Context.University context)
        {
            var init = new UniversityInitializer<University>();
            init.InitializeDatabase(context);

            Samples.SupplyData(context);
            

            var student1 = new Student
            {
                Id = 1,
                Name = "Vladyslav",
                Surname = "Serhiienko",
                Patronymic = "Romanovych",
                Group = context.Groups.SingleOrDefault(g => g.Id == 1)
            };

            var student2 = new Student
            {
                Name = "Vasyl",
                Surname = "Romanchak",
                Patronymic = "Igorovych",
                Group = context.Groups.Where(g => g.Acronym == "CAD").Where(g => g.DepartmentIndex == 3).FirstOrDefault()
            };

            var student3 = new Student
            {
                Name = "Carlos",
                Surname = "Norris",
                Patronymic = "McConnor",
                Group = context.Groups.SingleOrDefault(g => g.Id == 6)
            };


            context.Students.AddOrUpdate(student1, student2);
            context.Students.AddOrUpdate(s => s.Surname, student3);

            context.Database.ExecuteSqlCommand("UPDATE [dbo].[Disciplines] SET [dbo].[Disciplines].[Factor] = 2 WHERE [dbo].[Disciplines].[Factor] IS NULL");

            var MathDiscipline = context.Disciplines.SingleOrDefault(d => d.Title == "Mathematics");
            MathDiscipline.Factor = 5.0f;
            var PhysicsDiscipline = context.Disciplines.SingleOrDefault(d => d.Title == "Physics");
            PhysicsDiscipline.Factor = 5.0f;
            var PaintingDiscipline = context.Disciplines.SingleOrDefault(d => d.Title == "Painting");
            PaintingDiscipline.Factor = 6.0f;
            var MusicDiscipline = context.Disciplines.SingleOrDefault(d => d.Title == "Music");
            MusicDiscipline.Factor = 7.0f;

            context.Disciplines.AddOrUpdate(
                MathDiscipline,
                PhysicsDiscipline,
                PaintingDiscipline,
                MathDiscipline
                );
        }
    }
}