using System;
using System.Linq;
using System.Data;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace UniversityCodeFirstTests
{
    using UniversityCodeFirst;
    using UniversityCodeFirst.UniversityDatabase;
    using UniversityCodeFirst.UniversityDatabase.Context;
    using UniversityCodeFirst.UniversityDatabase.Entities;
    using System.Diagnostics;
    using System.Data.Entity.Validation;

    [TestClass]
    public class UniversityDbTests
    {
        [TestMethod]
        public void Test_UniversityContextCreation()
        {
            using (var db = new University())
            {
                Debug.WriteLine(
                                "Connection>> \n" +
                                "" + db.Database.Connection.
                                ConnectionString.
                                Replace("\n", "").
                                Replace(";", "\n\t").Insert(0, "\t")
                                );

                Assert.AreNotEqual(db.Students, null);
                Assert.AreNotEqual(db.Disciplines, null);
                Assert.AreNotEqual(db.Groups, null);
            }
        }

        [TestMethod]
        public void Test_UniversityContextAudit()
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

        [TestMethod]
        public void Test_UniversityContextValidation()
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

        [TestMethod]
        public void Test_UniversityContextQueries()
        {
            using (var db = new University())
            {
                Air.Broadcast("\nStudents, grouped by the first letter of their name ordered descending:");
                Air.Broadcast("Constrain: More then 1 element within");
                var studentsGroupedByNameDesc =
                    //Nested selection
                     from studentsGroup in
                         from student in db.Students
                         let letter = student.Name.Substring(0, 1)
                         where (student.Group.Course == (byte)1)
                         group student by letter into innerStudentsGroup
                         orderby innerStudentsGroup.Key descending
                         select innerStudentsGroup
                     where studentsGroup.Count() > 1
                     select studentsGroup;
                // HAVING-like syntax
                /*
                     (from student in db.Students
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
                    from s in db.Students
                    join g in db.Groups on s.Id equals g.Id into collection
                    from item in collection.DefaultIfEmpty()
                    let groupId = (long?)item.Id
                    let studentId = s.Id
                    select new UniversityCodeFirst.Samples.JoinedData()
                    {
                        GroupId = (long?)item.Id,
                        StudentId = s.Id
                    };

                foreach (var item in leftOuterJoinStudentsGroups)
                {
                    Air.Broadcast("joined>> " + item.ToString(db));
                }

                Air.Broadcast("\nRight outer join: dbo.Students-dbo.Groups");
                var rightOuterJoinStudentsGroups =
                    from g in db.Groups
                    join s in db.Students on g.Id equals s.Id into collection
                    from item in collection.DefaultIfEmpty()
                    let groupId = g.Id
                    let studentId = (long?)item.Id
                    select new UniversityCodeFirst.Samples.JoinedData()
                    {
                        GroupId = groupId,
                        StudentId = studentId
                    };

                foreach (var item in rightOuterJoinStudentsGroups)
                {
                    Air.Broadcast("joined>> " + item.ToString(db));
                }

                Air.Broadcast("\nInner join: dbo.Students-dbo.Groups");

                var innerJoinStudentsGroups =
                    // Can be yielded with the intersection ...
                    leftOuterJoinStudentsGroups.Intersect(rightOuterJoinStudentsGroups);
                // Or queried ...
                /*
                    from s in db.Students
                    join g in db.Groups on s.GroupId equals g.Id
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
                    Air.Broadcast("joined>> " + item.ToString(db));

                }

                Air.Broadcast("\nFull join: dbo.Students-dbo.Groups");
                var fullJoin =
                    // Yielded with the union ...
                    leftOuterJoinStudentsGroups.Union(rightOuterJoinStudentsGroups);
                foreach (var item in fullJoin)
                {
                    Air.Broadcast("joined>> " + item.ToString(db));

                }

                Air.Broadcast("\nGrouped join: dbo.Students-dbo.Groups");
                //Air.Broadcast("Constrain: More then 1 element within");
                var groupedJoinStudentsGroups =
                    from j in
                        from g in db.Groups
                        join s in db.Students on g.Id equals s.Id into studentCollection
                        select new { Group = g, StudentCollection = studentCollection }
                    //where j.StudentCollection.Count() > 1
                    select j;

                foreach (var item in groupedJoinStudentsGroups)
                {
                    Air.Broadcast("group>> " + UniversityCodeFirst.Samples.JoinedData.GroupToString(item.Group.Id, db));

                    foreach (var student in item.StudentCollection)
                    {
                        Air.Broadcast("\tstudent>> " + UniversityCodeFirst.Samples.JoinedData.StudentToString(student.Id, db));
                    }
                }


                Air.Broadcast("\nLeft outer join: dbo.Students-dbo.Groups (in raw)");
                var query = db.Database.SqlQuery((new UniversityCodeFirst.Samples.StudentGroupRawData()).GetType(),
                @"
                    SELECT 
            	        [dbo].[Students].[Name] + ' ' + [dbo].[Students].[Surname] AS StudentFullName,
            	        [dbo].[Groups].[Acronym] + '-' + CAST([dbo].[Groups].[Course] AS NVARCHAR) + CAST([dbo].[Groups].[DepartmentIndex] AS NVARCHAR) AS GroupTitle
                        FROM [dbo].[Students] 
                        LEFT OUTER JOIN [dbo].[Groups] ON [dbo].[Students].[Group_Id] = [dbo].[Groups].[Id]
                ");

                foreach (UniversityCodeFirst.Samples.StudentGroupRawData chunk in query)
                {
                    Air.Broadcast(chunk.StudentFullName + " " + chunk.GroupTitle);
                }
            }
        }
    }
}
