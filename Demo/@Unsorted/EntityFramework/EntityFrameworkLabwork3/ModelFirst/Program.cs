using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ModelFirst
{
    class Program
    {
        static void Main(string[] args)
        {
            using (var db = new BloggingModelContainer())
            {
                Console.WriteLine("Enter the name of the new blog: ");
                var blogName = Console.ReadLine();

                var blog = new Blog
                {
                    Name = blogName,
                    Url = "Unknown",
                };

                db.Blogs.Add(blog);
                db.SaveChanges();

                var query = from b in db.Blogs orderby b.Name select b;

                Console.WriteLine("All the blogs in the database: ");
                foreach (var item in query)
                {
                    Console.WriteLine(item.Name);
                }

                Console.ReadLine();
            }
        }
    }
}
