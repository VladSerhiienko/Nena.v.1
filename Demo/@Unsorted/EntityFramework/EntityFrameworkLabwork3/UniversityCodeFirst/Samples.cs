namespace UniversityCodeFirst
{
    using System;
    using System.Diagnostics;
    using System.Linq;
    using System.Data.Entity.Infrastructure;
    using System.Data.Entity.Validation;
    using UniversityDatabase.Context;
    using UniversityDatabase.Entities;

    public class Samples
    {
        public static void SupplyData(University context)
        {
            Air.Broadcast("Filling database with the sample data...");

            context.Database.ExecuteSqlCommand("INSERT [dbo].[Groups] ([Acronym], [Course], [DepartmentIndex]) VALUES (N'CAD', 1, 1)");
            context.Database.ExecuteSqlCommand("INSERT [dbo].[Groups] ([Acronym], [Course], [DepartmentIndex]) VALUES (N'CAD', 1, 2)");
            context.Database.ExecuteSqlCommand("INSERT [dbo].[Groups] ([Acronym], [Course], [DepartmentIndex]) VALUES (N'CAD', 1, 3)");
            context.Database.ExecuteSqlCommand("INSERT [dbo].[Groups] ([Acronym], [Course], [DepartmentIndex]) VALUES (N'AI', 1, 1)");
            context.Database.ExecuteSqlCommand("INSERT [dbo].[Groups] ([Acronym], [Course], [DepartmentIndex]) VALUES (N'AI', 1, 2)");
            context.Database.ExecuteSqlCommand("INSERT [dbo].[Groups] ([Acronym], [Course], [DepartmentIndex]) VALUES (N'AI', 1, 3)");
            context.Database.ExecuteSqlCommand("INSERT [dbo].[Groups] ([Acronym], [Course], [DepartmentIndex]) VALUES (N'NET', 1, 1)");
            context.Database.ExecuteSqlCommand("INSERT [dbo].[Groups] ([Acronym], [Course], [DepartmentIndex]) VALUES (N'NET', 1, 2)");
            context.Database.ExecuteSqlCommand("INSERT [dbo].[Groups] ([Acronym], [Course], [DepartmentIndex]) VALUES (N'NET', 1, 3)");

            context.Database.ExecuteSqlCommand("INSERT [dbo].[Disciplines] ([Title]) VALUES (N'Mathematics')");
            context.Database.ExecuteSqlCommand("INSERT [dbo].[Disciplines] ([Title]) VALUES (N'Physics')");
            context.Database.ExecuteSqlCommand("INSERT [dbo].[Disciplines] ([Title]) VALUES (N'Chemistry')");
            context.Database.ExecuteSqlCommand("INSERT [dbo].[Disciplines] ([Title]) VALUES (N'Economics')");
            context.Database.ExecuteSqlCommand("INSERT [dbo].[Disciplines] ([Title]) VALUES (N'Geography')");
            context.Database.ExecuteSqlCommand("INSERT [dbo].[Disciplines] ([Title]) VALUES (N'Painting')");
            context.Database.ExecuteSqlCommand("INSERT [dbo].[Disciplines] ([Title]) VALUES (N'Music')");
            context.Database.ExecuteSqlCommand("INSERT [dbo].[Disciplines] ([Title]) VALUES (N'History')");
            context.Database.ExecuteSqlCommand("INSERT [dbo].[Disciplines] ([Title]) VALUES (N'English')");
            context.Database.ExecuteSqlCommand("INSERT [dbo].[Disciplines] ([Title]) VALUES (N'Deutsch')");

            context.Database.ExecuteSqlCommand("INSERT [dbo].[Students] ([Surname], [Name], [Group_Id]) VALUES (N'Cooper', N'Charlotte', 1)");
            context.Database.ExecuteSqlCommand("INSERT [dbo].[Students] ([Surname], [Name], [Group_Id]) VALUES (N'Burke', N'Shelley', NULL)");
            context.Database.ExecuteSqlCommand("INSERT [dbo].[Students] ([Surname], [Name], [Group_Id]) VALUES (N'Murphy', N'Regina', 2)");
            context.Database.ExecuteSqlCommand("INSERT [dbo].[Students] ([Surname], [Name], [Group_Id]) VALUES (N'Wilson', N'Peter', 2)");
            context.Database.ExecuteSqlCommand("INSERT [dbo].[Students] ([Surname], [Name], [Group_Id]) VALUES (N'Peterson', N'Lars', NULL)");
            context.Database.ExecuteSqlCommand("INSERT [dbo].[Students] ([Surname], [Name], [Group_Id]) VALUES (N'Diaz', N'Carlos', 5)");
            context.Database.ExecuteSqlCommand("INSERT [dbo].[Students] ([Surname], [Name], [Group_Id]) VALUES (N'Bein', N'Martin', NULL)");
            context.Database.ExecuteSqlCommand("INSERT [dbo].[Students] ([Surname], [Name], [Group_Id]) VALUES (N'Rossi', N'Elio', NULL)");
            context.Database.ExecuteSqlCommand("INSERT [dbo].[Students] ([Surname], [Name], [Group_Id]) VALUES (N'Petersen', N'Sven ', 7)");
            context.Database.ExecuteSqlCommand("INSERT [dbo].[Students] ([Surname], [Name], [Group_Id]) VALUES (N'Leka', N'Chandra', 9)");
            context.Database.ExecuteSqlCommand("INSERT [dbo].[Students] ([Surname], [Name], [Group_Id]) VALUES (N'Heikkonen', N'Anne ', 9)");
            context.Database.ExecuteSqlCommand("INSERT [dbo].[Students] ([Surname], [Name], [Group_Id]) VALUES (N'Lauzon', N'Jean-Guy', 8)");

            Air.Broadcast("Changes: " + context.SaveChanges());
            foreach (var item in context.GetValidationErrors())
            {
                Air.Broadcast(item.ToString());
            }

            var rand = new Random();

            foreach (var discipline in context.Disciplines)
            {
                int num = rand.Next(3, 6);

                for (int i = 0; i < num; i++)
                {
                    var index = rand.Next(1, 10);
                    var group = context.Groups.Single(g => g.Id == index);

                    if (!discipline.Groups.Contains(group))
                    {
                        discipline.Groups.Add(group);
                        group.Disciplines.Add(discipline);
                    }
                    else
                    {
                        i--;
                    }
                }
            }

            Air.Broadcast("Changes: " + context.SaveChanges());
            foreach (var item in context.GetValidationErrors())
            {
                Air.Broadcast(item.ToString());
            }

            Air.Broadcast("Test output:");

            foreach (var group in context.Groups)
            {
                Air.Broadcast("Group: " + group.Acronym + group.Course + group.DepartmentIndex);

                if (group.Disciplines != null)
                {
                    if (group.Disciplines.Count == 0)
                    {
                        Air.Broadcast("No disciplines.");
                    }
                    foreach (var discipline in group.Disciplines)
                    {
                        Air.Broadcast("Discipline: " + discipline.Title + ".");
                    }
                }
            }

            foreach (var student in context.Students)
            {
                if (student.Group != null)
                {
                    Air.Broadcast(student.Surname + " " + student.Group.Acronym + "1" + student.Group.DepartmentIndex + ".");
                }
                else
                {
                    Air.Broadcast(student.Surname + " - not studying.");
                }
            }
        }

        public static void Audit(University context)
        {
            using (var db = new University())
            {
                var studentId = 3;

                var student = db.Students.Find(studentId);

                db.Database.ExecuteSqlCommand(
                    "UPDATE [dbo].[Students] SET [dbo].[Students].[Surname] = [dbo].[Students].[Surname] + '_Db' " +
                    "WHERE [dbo].[Students].[Id] = " + studentId
                    );

                student.Surname = student.Surname + "_Memory";

                var surname = string.Empty;

                surname = db.Entry(student)
                    .Property(s => s.Surname)
                    .OriginalValue;
                Air.Broadcast("OriginalValue: " + surname);

                surname = db.Entry(student)
                    .Property(s => s.Surname)
                    .CurrentValue;
                Air.Broadcast("CurrentValue: " + surname);

                surname = db.Entry(student)
                   .GetDatabaseValues().GetValue<string>("Surname");
                Air.Broadcast("DatabaseValue: " + surname);
            }
        }

        public static void Validation(University context)
        {
            using (var db = new University())
            {
                var student = new Student
                {
                    Name = "Li",
                    Surname = "Errungenschaftsgemeinschaft"
                };

                Air.Broadcast(
                    "Adding student: Name=" + student.Name + "(" + student.Name.Length + ") " +
                    "Surname=" + student.Surname + "(" + student.Surname.Length + ")"
                    );

                db.Students.Add(student);

                try
                {
                    db.SaveChanges();
                }
                catch (DbEntityValidationException ex)
                {
                    Air.Broadcast("External Validation Exception Handler>>");
                    Air.Broadcast(ex.Message);
                }
                catch (System.Exception ex)
                {
                    Air.Broadcast(ex.Message);
                }
            }
        }

        public static void FirstWins(University context)
        {
            var studentId = 3;

            var student = context.Students.Find(studentId);

            Air.Broadcast(
                    "Find student: Name=" + student.Name + "(" + student.Name.Length + ") " +
                    "Surname=" + student.Surname + "(" + student.Surname.Length + ") " +
                    "Id=" + student.Id
                    );
            Air.Broadcast("Update student surname in db: FirstWins");
            context.Database.ExecuteSqlCommand(
                "UPDATE [dbo].[Students] SET [dbo].[Students].[Surname] = 'FirstWins' " +
                "WHERE [dbo].[Students].[Id] = " + studentId
                );
            Air.Broadcast("Update student surname in memory: MemorySurname1");
            student.Surname = "MemorySurname1";

            try
            {
                context.SaveChanges();
            }
            catch (DbUpdateConcurrencyException ex)
            {
                Air.Broadcast("Reloading conflicting entries...");

                ex.Entries.ToList()
                    .ForEach(e => e.Reload());
            }

            student = context.Students.Find(studentId);

            Air.Broadcast(
                    "Find student: Name=" + student.Name + "(" + student.Name.Length + ") " +
                    "Surname=" + student.Surname + "(" + student.Surname.Length + ") " +
                    "Id=" + student.Id
                    );

            Air.Broadcast("Db or Memory ??? => " + student.Surname);
        }

        public static void LastWins(University context)
        {
            var studentId = 3;

            var student = context.Students.Find(studentId);

            Air.Broadcast(
                "Find student: Name=" + student.Name + "(" + student.Name.Length + ") " +
                "Surname=" + student.Surname + "(" + student.Surname.Length + ") " +
                "Id=" + student.Id
                );
            Air.Broadcast("Update student surname in db: LastWins");
            context.Database.ExecuteSqlCommand(
                "UPDATE [dbo].[Students] SET [dbo].[Students].[Surname] = 'LastWins' " +
                "WHERE [dbo].[Students].[Id] = " + studentId
                );
            Air.Broadcast("Update student surname in memory: MemorySurname");
            student.Surname = "MemorySurname";

            try
            {
                context.SaveChanges();
            }
            catch (DbUpdateConcurrencyException ex)
            {
                Air.Broadcast("Applying database values...");

                ex.Entries.ToList()
                    .ForEach(e => e.OriginalValues.SetValues(
                        e.GetDatabaseValues()
                        )
                    );
            }

            student = context.Students.Find(studentId);

            Air.Broadcast(
                    "Find student: Name=" + student.Name + "(" + student.Name.Length + ") " +
                    "Surname=" + student.Surname + "(" + student.Surname.Length + ") " +
                    "Id=" + student.Id
                    );

            Air.Broadcast("Db or Memory ??? => " + student.Surname);
        }



        public class JoinedData
        {
            public long? StudentId { get; set; }
            public long? GroupId { get; set; }

            public string ToString(University uni)
            {
                return string.Format(
                    "{0, -20} {1}",
                    StudentToString(StudentId, uni),
                    GroupToString(GroupId, uni)
                    );
            }

            public static string GroupToString(long? groupId, University uni)
            {
                if (groupId.Equals(null))
                {
                    return "Not found".ToUpper();
                }
                else
                {
                    var group = uni.Groups.ToArray()[(int)groupId - 1];
                    return group.Acronym + "-" + group.Course.ToString() +
                        group.DepartmentIndex.ToString();
                }
            }

            public static string StudentToString(long? studentId, University uni)
            {
                if (studentId.Equals(null))
                {
                    return "Not found".ToUpper();
                }
                else
                {
                    var student = uni.Students.ToArray()[(int)studentId - 1];
                    return student.Name + " " + student.Surname;
                }
            }
        }

        public class StudentGroupRawData
        {
            public string StudentFullName { get;set; }
            public string GroupTitle { get; set; }

            public StudentGroupRawData()
            {
                StudentFullName = string.Empty;
                GroupTitle = string.Empty;
            }
        }

        public static void DataMaintenance(University uni)
        {
            Air.Broadcast("\nStudents from CAD:");
            var students = from student in uni.Students
                       where (student.Group.Acronym == "CAD")
                       orderby student.Name ascending
                       select student;
            foreach (var student in students)
            {
                Air.Broadcast(
                    "student>> {0} {1}",
                    student.Name, student.Surname
                    );
            }

            Air.Broadcast("\nStudents of the 1st course grouped by their group acronym:");
            var studentsGroupedByGroupAcronym = uni.Students.
                Where(s => (s.Group.Course == (byte)1)).
                GroupBy(x => (x.Group.Acronym));

            foreach (var group in studentsGroupedByGroupAcronym)
            {
                Air.Broadcast("key>> " + group.Key);
                foreach (var student in group)
                {
                    Air.Broadcast(
                        "student>> {0} {1}",
                        student.Name, student.Surname
                        );
                }
            }

            Air.Broadcast("\nStudents of the 1st course grouped by surname in the alphabet order:");
            var studentsGroupedBySurnameAsc =
                from student in uni.Students
                where (student.Group.Course == (byte)1)
                group student by student.Surname.Substring(0, 1) into StudentsGroup
                orderby StudentsGroup.Key ascending
                select StudentsGroup;
            foreach (var group in studentsGroupedBySurnameAsc)
            {
                Air.Broadcast("key>> " + group.Key);
                foreach (var student in group)
                {
                    Air.Broadcast(
                        string.Format(
                            "student>> {1} {0}",
                            student.Name, student.Surname
                            )
                        );
                }
            }

            Air.Broadcast("\nStudents, grouped by the first letter of their name ordered descending:");
            Air.Broadcast("Constrain: More then 1 element within");
            var studentsGroupedByNameDesc =
                //Nested selection
                 from studentsGroup in
                     from student in uni.Students
                     let letter = student.Name.Substring(0, 1)
                     where (student.Group.Course == (byte)1)
                     group student by letter into innerStudentsGroup
                     orderby innerStudentsGroup.Key descending
                     select innerStudentsGroup
                 where studentsGroup.Count() > 1
                 select studentsGroup;
            // HAVING-like syntax
            /*
                 (from student in uni.Students
                 let letter = student.Name.Substring(0, 1)
                 where (student.Group.Course == (byte)1)
                 group student by letter into StudentsGroup
                 orderby StudentsGroup.Key descending
                 select StudentsGroup)
                 .Where(g => g.Count() > 1);
            */

            foreach (var group in studentsGroupedByNameDesc)
            {
                Air.Broadcast("key>> " + group.Key);
                foreach (var student in group)
                {
                    Air.Broadcast(
                        "\tstudent>> {0} {1}",
                        student.Name, student.Surname
                        );
                }
            }

            Air.Broadcast("\nLeft outer join: dbo.Students-dbo.Groups");
            var leftOuterJoinStudentsGroups =
                from s in uni.Students
                join g in uni.Groups on s.Id equals g.Id into collection
                from item in collection.DefaultIfEmpty()
                let groupId = (long?)item.Id
                let studentId = s.Id
                select new JoinedData()
                {
                    GroupId = (long?)item.Id,
                    StudentId = s.Id
                };

            foreach (var item in leftOuterJoinStudentsGroups)
            {
                Air.Broadcast("joined>> " + item.ToString(uni));
            }

            Air.Broadcast("\nRight outer join: dbo.Students-dbo.Groups");
            var rightOuterJoinStudentsGroups =
                from g in uni.Groups
                join s in uni.Students on g.Id equals s.Id into collection
                from item in collection.DefaultIfEmpty()
                let groupId = g.Id
                let studentId = (long?)item.Id
                select new JoinedData()
                {
                    GroupId = groupId,
                    StudentId = studentId
                };

            foreach (var item in rightOuterJoinStudentsGroups)
            {
                Air.Broadcast("joined>> " + item.ToString(uni));
            }

            Air.Broadcast("\nInner join: dbo.Students-dbo.Groups");

            var innerJoinStudentsGroups =
                // Can be yielded with the intersection ...
                leftOuterJoinStudentsGroups.Intersect(rightOuterJoinStudentsGroups);
            // Or queried ...
            /*
                from s in uni.Students
                join g in uni.Groups on s.GroupId equals g.Id
                let groupId = g.Id
                let studentId = s.Id
                select new JoinedData()
                {
                    GroupId = groupId,
                    StudentId = studentId
                };
             */

            foreach (var item in innerJoinStudentsGroups)
            {
                Air.Broadcast("joined>> " + item.ToString(uni));

            }

            Air.Broadcast("\nFull join: dbo.Students-dbo.Groups");
            var fullJoin =
                // Yielded with the union ...
                leftOuterJoinStudentsGroups.Union(rightOuterJoinStudentsGroups);
            foreach (var item in fullJoin)
            {
                Air.Broadcast("joined>> " + item.ToString(uni));

            }

            Air.Broadcast("\nGrouped join: dbo.Students-dbo.Groups");
            //Air.Broadcast("Constrain: More then 1 element within");
            var groupedJoinStudentsGroups =
                from j in
                    from g in uni.Groups
                    join s in uni.Students on g.Id equals s.Id into studentCollection
                    select new { Group = g, StudentCollection = studentCollection }
                //where j.StudentCollection.Count() > 1
                select j;

            foreach (var item in groupedJoinStudentsGroups)
            {
                Air.Broadcast("group>> " + JoinedData.GroupToString(item.Group.Id, uni));

                foreach (var student in item.StudentCollection)
                {
                    Air.Broadcast("\tstudent>> " + JoinedData.StudentToString(student.Id, uni));
                }
            }


            Air.Broadcast("\nLeft outer join: dbo.Students-dbo.Groups (in raw)");
            var query = uni.Database.SqlQuery((new StudentGroupRawData()).GetType(),
            @"
                SELECT 
	                [dbo].[Students].[Name] + ' ' + [dbo].[Students].[Surname] AS StudentFullName,
	                [dbo].[Groups].[Acronym] + '-' + CAST([dbo].[Groups].[Course] AS NVARCHAR) + CAST([dbo].[Groups].[DepartmentIndex] AS NVARCHAR) AS GroupTitle
                 FROM [dbo].[Students] 
                 LEFT OUTER JOIN [dbo].[Groups] ON [dbo].[Students].[Group_Id] = [dbo].[Groups].[Id]
            ");

            foreach (StudentGroupRawData chunk in query)
            {
                Air.Broadcast(chunk.StudentFullName + " " + chunk.GroupTitle);
            }
        }
    }
}

#region Sql Insertion

    //context.Database.ExecuteSqlCommand("INSERT [dbo].[Groups] ([Acronym], [Course], [DepartmentIndex]) VALUES (N'CAD', 1, 1)");
    //context.Database.ExecuteSqlCommand("INSERT [dbo].[Groups] ([Acronym], [Course], [DepartmentIndex]) VALUES (N'CAD', 1, 2)");
    //context.Database.ExecuteSqlCommand("INSERT [dbo].[Groups] ([Acronym], [Course], [DepartmentIndex]) VALUES (N'CAD', 1, 3)");
    //context.Database.ExecuteSqlCommand("INSERT [dbo].[Groups] ([Acronym], [Course], [DepartmentIndex]) VALUES (N'AI', 1, 1)");
    //context.Database.ExecuteSqlCommand("INSERT [dbo].[Groups] ([Acronym], [Course], [DepartmentIndex]) VALUES (N'AI', 1, 2)");
    //context.Database.ExecuteSqlCommand("INSERT [dbo].[Groups] ([Acronym], [Course], [DepartmentIndex]) VALUES (N'AI', 1, 3)");
    //context.Database.ExecuteSqlCommand("INSERT [dbo].[Groups] ([Acronym], [Course], [DepartmentIndex]) VALUES (N'NET', 1, 1)");
    //context.Database.ExecuteSqlCommand("INSERT [dbo].[Groups] ([Acronym], [Course], [DepartmentIndex]) VALUES (N'NET', 1, 2)");
    //context.Database.ExecuteSqlCommand("INSERT [dbo].[Groups] ([Acronym], [Course], [DepartmentIndex]) VALUES (N'NET', 1, 3)");

    //context.Database.ExecuteSqlCommand("INSERT [dbo].[Disciplines] ([Title]) VALUES (N'Mathematics')");
    //context.Database.ExecuteSqlCommand("INSERT [dbo].[Disciplines] ([Title]) VALUES (N'Physics')");
    //context.Database.ExecuteSqlCommand("INSERT [dbo].[Disciplines] ([Title]) VALUES (N'Chemistry')");
    //context.Database.ExecuteSqlCommand("INSERT [dbo].[Disciplines] ([Title]) VALUES (N'Economics')");
    //context.Database.ExecuteSqlCommand("INSERT [dbo].[Disciplines] ([Title]) VALUES (N'Geography')");
    //context.Database.ExecuteSqlCommand("INSERT [dbo].[Disciplines] ([Title]) VALUES (N'Painting')");
    //context.Database.ExecuteSqlCommand("INSERT [dbo].[Disciplines] ([Title]) VALUES (N'Music')");
    //context.Database.ExecuteSqlCommand("INSERT [dbo].[Disciplines] ([Title]) VALUES (N'History')");
    //context.Database.ExecuteSqlCommand("INSERT [dbo].[Disciplines] ([Title]) VALUES (N'English')");
    //context.Database.ExecuteSqlCommand("INSERT [dbo].[Disciplines] ([Title]) VALUES (N'Deutsch')");

    //context.Database.ExecuteSqlCommand("INSERT [dbo].[Disciplines_Groups] ([Discipline_Id], [Group_Id]) VALUES (1, 2)");
    //context.Database.ExecuteSqlCommand("INSERT [dbo].[Disciplines_Groups] ([Discipline_Id], [Group_Id]) VALUES (1, 3)");
    //context.Database.ExecuteSqlCommand("INSERT [dbo].[Disciplines_Groups] ([Discipline_Id], [Group_Id]) VALUES (1, 4)");
    //context.Database.ExecuteSqlCommand("INSERT [dbo].[Disciplines_Groups] ([Discipline_Id], [Group_Id]) VALUES (2, 3)");
    //context.Database.ExecuteSqlCommand("INSERT [dbo].[Disciplines_Groups] ([Discipline_Id], [Group_Id]) VALUES (2, 2)");
    //context.Database.ExecuteSqlCommand("INSERT [dbo].[Disciplines_Groups] ([Discipline_Id], [Group_Id]) VALUES (2, 1)");
    //context.Database.ExecuteSqlCommand("INSERT [dbo].[Disciplines_Groups] ([Discipline_Id], [Group_Id]) VALUES (3, 5)");
    //context.Database.ExecuteSqlCommand("INSERT [dbo].[Disciplines_Groups] ([Discipline_Id], [Group_Id]) VALUES (3, 6)");
    //context.Database.ExecuteSqlCommand("INSERT [dbo].[Disciplines_Groups] ([Discipline_Id], [Group_Id]) VALUES (3, 7)");
    //context.Database.ExecuteSqlCommand("INSERT [dbo].[Disciplines_Groups] ([Discipline_Id], [Group_Id]) VALUES (4, 5)");
    //context.Database.ExecuteSqlCommand("INSERT [dbo].[Disciplines_Groups] ([Discipline_Id], [Group_Id]) VALUES (4, 3)");
    //context.Database.ExecuteSqlCommand("INSERT [dbo].[Disciplines_Groups] ([Discipline_Id], [Group_Id]) VALUES (5, 5)");
    //context.Database.ExecuteSqlCommand("INSERT [dbo].[Disciplines_Groups] ([Discipline_Id], [Group_Id]) VALUES (5, 6)");
    //context.Database.ExecuteSqlCommand("INSERT [dbo].[Disciplines_Groups] ([Discipline_Id], [Group_Id]) VALUES (5, 1)");
    //context.Database.ExecuteSqlCommand("INSERT [dbo].[Disciplines_Groups] ([Discipline_Id], [Group_Id]) VALUES (5, 2)");
    //context.Database.ExecuteSqlCommand("INSERT [dbo].[Disciplines_Groups] ([Discipline_Id], [Group_Id]) VALUES (6, 3)");
    //context.Database.ExecuteSqlCommand("INSERT [dbo].[Disciplines_Groups] ([Discipline_Id], [Group_Id]) VALUES (6, 4)");
    //context.Database.ExecuteSqlCommand("INSERT [dbo].[Disciplines_Groups] ([Discipline_Id], [Group_Id]) VALUES (6, 7)");
    //context.Database.ExecuteSqlCommand("INSERT [dbo].[Disciplines_Groups] ([Discipline_Id], [Group_Id]) VALUES (8, 9)");
    //context.Database.ExecuteSqlCommand("INSERT [dbo].[Disciplines_Groups] ([Discipline_Id], [Group_Id]) VALUES (9, 1)");
    //context.Database.ExecuteSqlCommand("INSERT [dbo].[Disciplines_Groups] ([Discipline_Id], [Group_Id]) VALUES (9, 2)");
    //context.Database.ExecuteSqlCommand("INSERT [dbo].[Disciplines_Groups] ([Discipline_Id], [Group_Id]) VALUES (9, 5)");
    //context.Database.ExecuteSqlCommand("INSERT [dbo].[Disciplines_Groups] ([Discipline_Id], [Group_Id]) VALUES (10, 1)");
    //context.Database.ExecuteSqlCommand("INSERT [dbo].[Disciplines_Groups] ([Discipline_Id], [Group_Id]) VALUES (10, 2)");

    //INSERT [dbo].[Students] ([Id], [Surname], [Name], [Patronymic], [Age], [GroupId]) VALUES (1, N'Cooper', N'Charlotte', N'Giovanni', 24, 1)
    //INSERT [dbo].[Students] ([Id], [Surname], [Name], [Patronymic], [Age], [GroupId]) VALUES (2, N'Burke', N'Shelley', N'Beate', 25, NULL)
    //INSERT [dbo].[Students] ([Id], [Surname], [Name], [Patronymic], [Age], [GroupId]) VALUES (3, N'Murphy', N'Regina', NULL, 23, 2)
    //INSERT [dbo].[Students] ([Id], [Surname], [Name], [Patronymic], [Age], [GroupId]) VALUES (4, N'Wilson', N'Peter', N'Antonio', 25, 2)
    //INSERT [dbo].[Students] ([Id], [Surname], [Name], [Patronymic], [Age], [GroupId]) VALUES (5, N'Peterson', N'Lars', N'Peter', 22, NULL)
    //INSERT [dbo].[Students] ([Id], [Surname], [Name], [Patronymic], [Age], [GroupId]) VALUES (6, N'Diaz', N'Carlos', NULL, 26, 5)
    //INSERT [dbo].[Students] ([Id], [Surname], [Name], [Patronymic], [Age], [GroupId]) VALUES (7, N'Bein', N'Martin', NULL, 26, NULL)
    //INSERT [dbo].[Students] ([Id], [Surname], [Name], [Patronymic], [Age], [GroupId]) VALUES (8, N'Rossi', N'Elio', N'Peter', 27, NULL)
    //INSERT [dbo].[Students] ([Id], [Surname], [Name], [Patronymic], [Age], [GroupId]) VALUES (9, N'Petersen', N'Sven ', NULL, 31, 7)
    //INSERT [dbo].[Students] ([Id], [Surname], [Name], [Patronymic], [Age], [GroupId]) VALUES (10, N'Leka', N'Chandra', N'Grant', 18, 9)
    //INSERT [dbo].[Students] ([Id], [Surname], [Name], [Patronymic], [Age], [GroupId]) VALUES (11, N'Heikkonen', N'Anne ', N'Antonio', 30, 9)
    //INSERT [dbo].[Students] ([Id], [Surname], [Name], [Patronymic], [Age], [GroupId]) VALUES (12, N'Lauzon', N'Jean-Guy', N'Grant', 19, 8)

    /*
    var g1 = new Group { Acronym = "CAD", Course = 1, DepartmentIndex = 1 };
    var g2 = new Group { Acronym = "CAD", Course = 1, DepartmentIndex = 2 };
    var g3 = new Group { Acronym = "CAD", Course = 1, DepartmentIndex = 3 };
    var g4 = new Group { Acronym = "AI", Course = 1, DepartmentIndex = 1 };
    var g5 = new Group { Acronym = "AI", Course = 1, DepartmentIndex = 2 };
    var g6 = new Group { Acronym = "AI", Course = 1, DepartmentIndex = 3 };
    var g7 = new Group { Acronym = "NET", Course = 1, DepartmentIndex = 1 };
    var g8 = new Group { Acronym = "NET", Course = 1, DepartmentIndex = 2 };
    var g9 = new Group { Acronym = "NET", Course = 1, DepartmentIndex = 3 };

    var d1 = new Discipline { Title = "Mathematics" };
    var d2 = new Discipline { Title = "Physics" };
    var d3 = new Discipline { Title = "Chemistry" };
    var d4 = new Discipline { Title = "Economics" };
    var d5 = new Discipline { Title = "Geography" };
    var d6 = new Discipline { Title = "Painting" };
    var d7 = new Discipline { Title = "Music" };
    var d8 = new Discipline { Title = "History" };
    var d9 = new Discipline { Title = "English" };
    var d10 = new Discipline { Title = "Deutsch" };

    context.Groups.Add(g1);
    context.Groups.Add(g2);
    context.Groups.Add(g3);
    context.Groups.Add(g4);
    context.Groups.Add(g5);
    context.Groups.Add(g6);
    context.Groups.Add(g7);
    context.Groups.Add(g8);
    context.Groups.Add(g9);

    context.Disciplines.Add(d1);
    context.Disciplines.Add(d2);
    context.Disciplines.Add(d3);
    context.Disciplines.Add(d4);
    context.Disciplines.Add(d5);
    context.Disciplines.Add(d6);
    context.Disciplines.Add(d7);
    context.Disciplines.Add(d8);
    context.Disciplines.Add(d9);
    context.Disciplines.Add(d10);

 */

#endregion Sql Insertion