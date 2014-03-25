using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Data.SqlClient;

namespace ConsoleApplication1.Company
{
    class Collaborator : Person
    {
        public Int32 CollaboratorId { get; set; }
        public String ManagerCode { get; set; }
        public virtual Manager Manager { get; set; }
    }
}
