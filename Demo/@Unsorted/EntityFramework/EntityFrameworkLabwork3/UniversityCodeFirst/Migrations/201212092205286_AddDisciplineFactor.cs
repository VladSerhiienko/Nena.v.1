namespace UniversityCodeFirst.Migrations
{
    using System;
    using System.Data.Entity.Migrations;
    
    public partial class AddDisciplineFactor : DbMigration
    {
        public override void Up()
        {
            AddColumn("dbo.Disciplines", "Factor", c => c.Single(nullable: false));
        }
        
        public override void Down()
        {
            DropColumn("dbo.Disciplines", "Factor");
        }
    }
}
