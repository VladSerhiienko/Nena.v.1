using System;
using System.ComponentModel.DataAnnotations;

namespace UniversityCodeFirst.UniversityDatabase.Entities
{
    public class Student
    {
        public int Id { get; set; }

        [ConcurrencyCheck]
        [MinLength(3, ErrorMessage = "The name is too short. Consider it to have more then 3 digits.")]
        [MaxLength(20, ErrorMessage = "The name is too big. Consider it to have less then 20 digits.")]
        public string Name { get; set; }

        [ConcurrencyCheck]
        [MinLength(3, ErrorMessage = "The surname is too short. Consider it to have more then 3 digits.")]
        [MaxLength(20, ErrorMessage = "The surname is too big. Consider it to have less then 20 digits.")]
        public String Surname { get; set; }

        [ConcurrencyCheck]
        [MinLength(3, ErrorMessage = "The patronymic is too short. Consider it to have more then 3 digits.")]
        [MaxLength(20, ErrorMessage = "The patronymic is too big. Consider it to have less then 20 digits.")]
        public String Patronymic { get; set; }

        public virtual Group Group { get; set; }
    }
}