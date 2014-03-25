namespace UniversityCodeFirst.Migrations
{
    using System;
    using System.Data.Entity.Migrations;
    
    public partial class AddStudentPatronymic : DbMigration
    {
        public override void Up()
        {
            AddColumn("dbo.Students", "Patronymic", c => c.String(maxLength: 20));
        }
        
        public override void Down()
        {
            DropColumn("dbo.Students", "Patronymic");
        }
    }
}
