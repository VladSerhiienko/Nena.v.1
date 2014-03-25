using System.Collections.Generic;

namespace UniversityCodeFirst.UniversityDatabase.Entities
{
    public class Discipline
    {
        public int Id { get; set; }

        public string Title { get; set; }

        public float? Factor { get; set; }

        public virtual ICollection<Group> Groups { get; set; }

        public Discipline()
        {
            Groups = new List<Group>();
        }
    }
}