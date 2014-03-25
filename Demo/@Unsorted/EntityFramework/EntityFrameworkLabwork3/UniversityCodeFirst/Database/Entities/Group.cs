using System.Collections.Generic;

namespace UniversityCodeFirst.UniversityDatabase.Entities
{
    public class Group
    {
        public int Id { get; set; }

        public string Acronym { get; set; }

        public int Course { get; set; }

        public int DepartmentIndex { get; set; }

        public virtual ICollection<Student> Students { get; set; }

        public virtual ICollection<Discipline> Disciplines { get; set; }

        public Group()
        {
            Students = new List<Student>();
            Disciplines = new List<Discipline>();
        }
    }
}