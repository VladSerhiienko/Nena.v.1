using System;
using System.Collections.Generic;
using System.ComponentModel.DataAnnotations;
using System.Linq;
using System.Text;

namespace ConsoleApplication1.Company
{
    class Manager : Person
    {
        [Key]
        public String ManagerCode { get; set; }

        [ConcurrencyCheck]
        [MinLength(5, ErrorMessage="The name is too short.")]
        [MaxLength(20, ErrorMessage="The name is too big.")]
        public new String FirstName { get; set; }
        public new String LastName { get; set; }
        public virtual ICollection<Collaborator> Collaborators { get; set; }
    }
}
