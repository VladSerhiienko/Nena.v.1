using System;
using System.Collections.Generic;
using System.ComponentModel.DataAnnotations;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ConsoleApplication1.Company
{
    class Department
    {
        public Int32 DepartmentId { get; set; }

        [Required]
        public String Name { get; set; }

        public virtual ICollection<Collaborator> Collaborators { get; set; }
    }
}
