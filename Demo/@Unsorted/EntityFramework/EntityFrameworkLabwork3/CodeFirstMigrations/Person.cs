using System;
using System.Collections.Generic;
using System.ComponentModel.DataAnnotations;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CodeFirstMigrations
{
    class Person
    {
        [Key]
        public Int32 Id { get; set; }

        [Required]
        [MinLength(5)]
        [MaxLength(20)]
        [ConcurrencyCheck]
        public String FirstName { get; set; }

        [Required]
        [MinLength(5)]
        [MaxLength(20)]
        [ConcurrencyCheck]
        public String LastName { get; set; }

        public DateTime BirthData { get; set; }

        public String Address { get; set; }
    }
}
