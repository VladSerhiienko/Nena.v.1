using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Data;
using System.ComponentModel;
using System.ComponentModel.DataAnnotations;

namespace ConsoleApplication1.Company
{
    abstract class Person
    {
        [Required]
        public String FirstName { get; set; }
        public String LastName { get; set; }
        public Int32 DepartmentId { get; set; }
        public virtual Department Department { get; set; }
    }
}
